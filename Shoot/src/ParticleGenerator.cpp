/* 

Amine Rehioui
Created: May 30th 2010

*/

#include "Shoot.h"

#include "ParticleGenerator.h"

#include "Quaternion.h"
#include "Basis3.h"

namespace shoot
{
	DEFINE_OBJECT(ParticleGenerator);

	//! literals
	const char* const g_strEmitTypeLiterals[] =
	{
		"ET_Box",
		"ET_Explosive",
		"ET_Cone",
		0
	};

	const char* const g_strParticleShapeLiterals[] =
	{
		"PS_Quad",
		"PS_Line",
		0
	};

	const char* const g_strParticleSizeFuncLiterals[] =	
	{
		"SF_Constant",
		"SF_Grow",
		"SF_Shrink",
		0
	};

	//! constructor
	ParticleGenerator::ParticleGenerator()
	: m_bActive(true)
	, m_EmitFunctor(&ParticleGenerator::EmitBox)
	, m_PreEmitFunctor(&ParticleGenerator::PreEmitNull)
	, m_TesselateFunctor(&ParticleGenerator::TesselateQuad)
	, m_SizeFuncFunctor(&ParticleGenerator::SizeFuncGrow)
	, m_fHalfSize(0.5f)
	// properties
	, m_vDirection(Vector3::Create(0.0f, 0.0f, 1.0f))
	, m_vGravity(Vector3::Zero)
	, m_EmitBox(-Vector3::One, Vector3::One)
	, m_MaxParticles(200)
	, m_ParticlesPerSecond(30)
	, m_rParticleLife(1.0f, 2.0f)
	, m_rParticleInitialSpeed(1.0f, 1.5f)
	, m_fParticleSize(1.0f)
	, m_fNewParticlesCounter(0.0f)
	, m_fActiveTime(-1.0f)
	, m_eEmitType(ET_Box)
	, m_eParticleShape(PS_Quad)
	, m_bApplyWorldTransform(true)
	, m_bInstancedRendering(false)
	, m_fConeAngle(45.0f)
	, m_fSparkFactor(1.0f)
	, m_SizeFunc(SF_Grow)
	, m_SizeFuncFactor(2.0f)
	{
	}

	//! serializes the entity to/from a PropertyStream
	void ParticleGenerator::Serialize(PropertyStream& stream)
	{
		super::Serialize(stream);
		
		stream.Serialize("MaxParticles", &m_MaxParticles);
		stream.Serialize("ParticlesPerSecond", &m_ParticlesPerSecond);
		stream.Serialize("ParticleLife", &m_rParticleLife);
		stream.Serialize("ParticleSpeed", &m_rParticleInitialSpeed);
		stream.Serialize("ParticleSize", &m_fParticleSize);
		stream.Serialize("Direction", &m_vDirection);
		stream.Serialize("Gravity", &m_vGravity);
		stream.Serialize("EmitBox", &m_EmitBox);
		stream.Serialize("ActiveTime", &m_fActiveTime);
		stream.Serialize("EmitType", &m_eEmitType, ENUM_PARAMS1(g_strEmitTypeLiterals));
		E_ParticleShape shape = m_eParticleShape;
		stream.Serialize("ParticleShape", &shape, ENUM_PARAMS1(g_strParticleShapeLiterals));
		stream.Serialize("ConeAngle", &m_fConeAngle);
		stream.Serialize("SparkFactor", &m_fSparkFactor);		
		stream.Serialize("SizeFunc", &m_SizeFunc, ENUM_PARAMS1(g_strParticleSizeFuncLiterals));
		stream.Serialize("SizeFuncFactor", &m_SizeFuncFactor);
		stream.Serialize("ApplyWorldTransform", &m_bApplyWorldTransform);
		stream.Serialize("InstancedRendering", &m_bInstancedRendering);

		if(stream.GetMode() != SM_Read)
			return;
		
		switch(m_eEmitType)
		{
		case ET_Box:
			m_PreEmitFunctor = &ParticleGenerator::PreEmitNull;
			m_EmitFunctor = &ParticleGenerator::EmitBox;			
			break;

		case ET_Explosion:
			m_PreEmitFunctor = &ParticleGenerator::PreEmitNull;
			m_EmitFunctor = &ParticleGenerator::EmitExplosion;
			break;

		case ET_Cone:
			m_PreEmitFunctor = &ParticleGenerator::PreEmitCone;
			m_EmitFunctor = &ParticleGenerator::EmitCone;
			break;
		}

		switch (m_SizeFunc)
		{
		case SF_Constant:
			m_SizeFuncFunctor = &ParticleGenerator::SizeFuncConstant;
			break;

		case SF_Grow:
			m_SizeFuncFunctor = &ParticleGenerator::SizeFuncGrow;
			break;

		case SF_Shrink:
			m_SizeFuncFunctor = &ParticleGenerator::SizeFuncShrink;
			break;
		}
		
		m_vDirection.Normalize();
		m_fHalfSize = m_fParticleSize/2.0f;
		m_bActive = true;
		auto shapeChanged = (m_eParticleShape != shape);
		m_eParticleShape = shape;

		if(!IsInitialized())
			return;

		auto vb = GetComponent<GraphicComponent>()->GetVertexBuffer();
		vb->SetApplyWorldTransforms(m_bApplyWorldTransform);

		if (shapeChanged)
		{
			auto numParticleVertices = OnParticleShapeChanged(vb);
			vb->SetVertices(snew Vertex3D[m_MaxParticles*numParticleVertices], m_MaxParticles*numParticleVertices, Vertex3D::VF_Pos | Vertex3D::VF_UV | Vertex3D::VF_Color);			
		}	
	}	

	//! called during the initialization of the entity
	void ParticleGenerator::Init()
	{
		super::Init();

		m_Pool.SetCapacity(m_MaxParticles);

		VertexBuffer* pVB = GraphicsDriver::Instance()->CreateVertexBuffer(m_bInstancedRendering);		
		pVB->SetName(GetClassName());
		pVB->SetDynamic(true);
		pVB->SetApplyWorldTransforms(m_bApplyWorldTransform);
		auto numParticleVertices = OnParticleShapeChanged(pVB);
		pVB->SetVertices(snew Vertex3D[m_MaxParticles*numParticleVertices], m_MaxParticles*numParticleVertices, Vertex3D::VF_Pos|Vertex3D::VF_UV|Vertex3D::VF_Color);
		pVB->SetNumVertices(0);

		GetComponent<GraphicComponent>()->SetVertexBuffer(pVB);
	}

	//! called during the update of the entity
	void ParticleGenerator::Update()
	{
		auto camera = EntityRenderer::Instance()->Get3DCamera();
		if (!camera)
			return;

		// generate new particles
		if(m_bActive)
		{
			m_fNewParticlesCounter += g_fDeltaTime * m_ParticlesPerSecond;
			int numNewParticles = int(m_fNewParticlesCounter);
			if(m_Pool.GetElementCount() + numNewParticles > m_MaxParticles)
				numNewParticles = m_MaxParticles - (int)m_Pool.GetElementCount();

			// determine emission center
			Vector3 vCenter = m_EmitBox.Min()+m_EmitBox.Size()/2.0f;
			if(!m_bApplyWorldTransform)
				vCenter = super::GetWorldTransform().TransformVect(vCenter);

			Matrix44 speedTransform = super::GetWorldTransform();
			speedTransform.SetTranslation(Vector3::Zero);

			// Pre-emit
			void* pEmitData = (this->*m_PreEmitFunctor)();

			for(int i=0; i<numNewParticles; ++i)
			{
				auto pParticle = m_Pool.Alloc();
				if(!pParticle)
					continue;

				pParticle->InitialLife = Random::GetFloat(m_rParticleLife.Min, m_rParticleLife.Max);
				pParticle->Life = pParticle->InitialLife;

				pParticle->Pos = Vector3::Create(Random::GetFloat(m_EmitBox.Min().X, m_EmitBox.Max().X),
					Random::GetFloat(m_EmitBox.Min().Y, m_EmitBox.Max().Y),
					Random::GetFloat(m_EmitBox.Min().Z, m_EmitBox.Max().Z));				

				if(!m_bApplyWorldTransform)				
					pParticle->Pos = super::GetWorldTransform().TransformVect(pParticle->Pos);
				pParticle->PreviousPos = pParticle->Pos;

				// Emit
				(this->*m_EmitFunctor)(pParticle, vCenter, pEmitData);

				if(!m_bApplyWorldTransform)
					pParticle->Speed = speedTransform.TransformVect(pParticle->Speed);				

				pParticle->Alpha = 1.0f;
			}

			m_fNewParticlesCounter -= numNewParticles;
			if(m_fActiveTime >= 0.0f)
			{
				m_fActiveTime -= g_fDeltaTime;
				if(m_fActiveTime < 0.0f)
					m_bActive = false;
			}
		}

		// update particles
		GraphicComponent* pGraphic = GetComponent<GraphicComponent>();
		VertexBuffer* pVB = pGraphic->GetVertexBuffer();
		Vertex3D* pVertices = reinterpret_cast<Vertex3D*>(pVB->GetVertices());
		const Vector3 vScale = GetAbsoluteScale();
		m_ViewInverse = camera->GetViewMatrixInverse();
		m_ParticleColor = pGraphic->GetMaterial() ? pGraphic->GetMaterial()->GetColor() : Color::White;

		int vtx = 0;
		for (size_t i = 0; i < m_Pool.GetCapacity(); ++i)
		{			
			auto particle = m_Pool.GetElement(i);
			if (!particle)
				continue;

			particle->Pos += particle->Speed * g_fDeltaTime;
			particle->Speed += m_vGravity * g_fDeltaTime;
			auto lifeFactor = particle->Life / particle->InitialLife;
			particle->Alpha = lifeFactor*lifeFactor;
			particle->Size = (this->*m_SizeFuncFunctor)(particle);

			// Tesselate
			auto numParticleVertices = (this->*m_TesselateFunctor)(particle, pVertices, vtx, vScale);

			vtx += numParticleVertices;
			particle->Life -= g_fDeltaTime;
			if(particle->Life < 0.0f)			
				m_Pool.Free(particle);			
		}

		pVB->SetNumVertices(vtx);
		pVB->SetDirty(true);
	}

	//! returns the affine transformation matrix
	Matrix44 ParticleGenerator::GetWorldTransform()
	{
		return Matrix44::Identity;
	}

	void ParticleGenerator::EmitBox(Particle* pParticle, const Vector3& vCenter, void* pEmitData)
	{
		pParticle->Speed = Vector3::Create(m_vDirection.X * Random::GetFloat(m_rParticleInitialSpeed.Min, m_rParticleInitialSpeed.Max),
			m_vDirection.Y * Random::GetFloat(m_rParticleInitialSpeed.Min, m_rParticleInitialSpeed.Max),
			m_vDirection.Z * Random::GetFloat(m_rParticleInitialSpeed.Min, m_rParticleInitialSpeed.Max));
	}

	void ParticleGenerator::EmitExplosion(Particle* pParticle, const Vector3& vCenter, void* pEmitData)
	{
		pParticle->Speed = (pParticle->Pos-vCenter).Normalize();
		pParticle->Speed *= Random::GetFloat(m_rParticleInitialSpeed.Min, m_rParticleInitialSpeed.Max);
	}

	void ParticleGenerator::EmitCone(Particle* pParticle, const Vector3& vCenter, void* pEmitData)
	{
		EmitConeData* pData = static_cast<EmitConeData*>(pEmitData);
		float fAngle1 = Random::GetFloat(-pData->fAngle, pData->fAngle);
		float fAngle2 = Random::GetFloat(-pData->fAngle, pData->fAngle);
		Quaternion q1(pData->vLateralAxe, fAngle1);
		Quaternion q2(pData->vVerticalAxe, fAngle2);
		pParticle->Speed = q1.Rotate(m_vDirection);
		pParticle->Speed = q2.Rotate(pParticle->Speed);
		pParticle->Speed *= Random::GetFloat(m_rParticleInitialSpeed.Min, m_rParticleInitialSpeed.Max);
	}

	void* ParticleGenerator::PreEmitNull()
	{
		return NULL;
	}

	void* ParticleGenerator::PreEmitCone()
	{
		Basis3 localBasis(Vector3::Zero, m_vDirection);
		m_EmitConeData.vLateralAxe = localBasis.Lateral.Normalize();
		m_EmitConeData.vVerticalAxe = localBasis.Vertical.Normalize();
		m_EmitConeData.fAngle = m_fConeAngle/2.0f*Math::DegToRadFactor;
		return &m_EmitConeData;
	}

	int ParticleGenerator::TesselateQuad(Particle* pParticle, Vertex3D* pVertices, int current, const Vector3& vScale)
	{
		auto halfSize = pParticle->Size;
		pVertices[current+0].UV = Vector2::Create(0.0f, 1.0f); pVertices[current+0].Pos = Vector3::Create(-halfSize, -halfSize, 0.0f)*vScale;
		pVertices[current+1].UV = Vector2::Create(0.0f, 0.0f); pVertices[current+1].Pos = Vector3::Create(-halfSize, halfSize, 0.0f)*vScale;
		pVertices[current+2].UV = Vector2::Create(1.0f, 0.0f); pVertices[current+2].Pos = Vector3::Create(halfSize, halfSize, 0.0f)*vScale;
		pVertices[current+3].UV = Vector2::Create(0.0f, 1.0f); pVertices[current+3].Pos = Vector3::Create(-halfSize, -halfSize, 0.0f)*vScale;
		pVertices[current+4].UV = Vector2::Create(1.0f, 0.0f); pVertices[current+4].Pos = Vector3::Create(halfSize, halfSize, 0.0f)*vScale;
		pVertices[current+5].UV = Vector2::Create(1.0f, 1.0f); pVertices[current+5].Pos = Vector3::Create(halfSize, -halfSize, 0.0f)*vScale;
		
		Matrix44 transform = m_ViewInverse;
		transform.SetTranslation(m_bApplyWorldTransform ? super::GetWorldTransform().TransformVect(pParticle->Pos) : pParticle->Pos);
		for(int i=0; i<6; ++i)
		{
			pVertices[current+i].Pos = transform.TransformVect(pVertices[current+i].Pos);
			pVertices[current+i].color = Color::Create(m_ParticleColor.R, m_ParticleColor.G, m_ParticleColor.B, pParticle->Alpha);
		}
		return 6;
	}

	int ParticleGenerator::TesselateLine(Particle* pParticle, Vertex3D* pVertices, int current, const Vector3& vScale)
	{
		Vector3 vDir = (pParticle->Pos - pParticle->PreviousPos).Normalized();		
		pVertices[current+0].UV = Vector2::Zero; pVertices[current+0].Pos = Vector3::Zero;
		pVertices[current+1].UV = Vector2::One;  pVertices[current+1].Pos = vDir*vScale*m_fSparkFactor;
		pParticle->PreviousPos = pParticle->Pos;

		Matrix44 transform = Matrix44::Identity;
		transform.SetTranslation(m_bApplyWorldTransform ? super::GetWorldTransform().TransformVect(pParticle->Pos) : pParticle->Pos);
		for(int i=0; i<2; ++i)
		{
			pVertices[current+i].Pos = transform.TransformVect(pVertices[current+i].Pos);
			pVertices[current+i].color = Color::Create(m_ParticleColor.R, m_ParticleColor.G, m_ParticleColor.B, pParticle->Alpha);
		}
		return 2;
	}

	float ParticleGenerator::SizeFuncConstant(Particle* particle)
	{
		return m_fHalfSize;
	}

	float ParticleGenerator::SizeFuncGrow(Particle* particle)
	{
		auto factor = (particle->InitialLife-particle->Life) / particle->InitialLife;
		return Math::Lerp(0.0f, m_fHalfSize, factor*m_SizeFuncFactor);
	}

	float ParticleGenerator::SizeFuncShrink(Particle* particle)
	{
		auto factor = (particle->InitialLife - particle->Life) / particle->InitialLife;
		return Math::Lerp(m_fHalfSize, 0.0f, factor*m_SizeFuncFactor);
	}

	//! called when particle shape has changed
	int ParticleGenerator::OnParticleShapeChanged(VertexBuffer* pVB)
	{
		if(m_eParticleShape == PS_Quad)
		{
			pVB->SetPrimitiveType(GraphicsDriver::PT_Triangle);
			m_TesselateFunctor = &ParticleGenerator::TesselateQuad;
			return 6;
		}
		else
		{
			pVB->SetPrimitiveType(GraphicsDriver::PT_Line);
			m_TesselateFunctor = &ParticleGenerator::TesselateLine;
			return 2;
		}
	}	
}

