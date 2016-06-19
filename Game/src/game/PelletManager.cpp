/* 

Amine Rehioui
Created: May 1st 2013

*/

#include "Game.h"

#include "PelletManager.h"

#include "MaterialProvider.h"

#include "AIManager.h"

#include "GraphicExtensionHandler.h"

namespace shoot
{
	DEFINE_OBJECT(PelletManager);

	//! Constructor
	PelletManager::PelletManager()
		: m_fMinCos(0.0f)
		// properties
		, m_fBulletSize(8.0f)
		, m_fMinDistToActor(10.0f)
		, m_fHomingFactor(3.0f)
		, m_fMaxHomingAngle(45.0f)
	{
	}

	//! destructor
	PelletManager::~PelletManager()
	{
	}

	//! serializes the entity to/from a PropertyStream
	void PelletManager::Serialize(PropertyStream& stream)
	{
		super::Serialize(stream);

		stream.Serialize("BulletSize", &m_fBulletSize);
		stream.Serialize("MinDistToActor", &m_fMinDistToActor);
		stream.Serialize("HomingFactor", &m_fHomingFactor);
		stream.Serialize("MaxHomingAngle", &m_fMaxHomingAngle);

		m_fMinCos = Math::Cos(m_fMaxHomingAngle*Math::DegToRadFactor);
	}

	//! called during the initialization of the entity
	void PelletManager::Init()
	{
		super::Init();
		
		GraphicComponent* pGraphic = GetComponent<GraphicComponent>();

		if(!pGraphic->GetMaterial())
		{
			Material* pMaterial = snew Material();
			pMaterial->SetShader(Material::m_strDefaultShader);
			pGraphic->SetMaterial(pMaterial);
		}

		int maxBullets = m_MaxBullets;
		VertexBuffer* pVB = GraphicsDriver::Instance()->CreateVertexBuffer();
		pVB->SetName(GetClassName());
		pVB->SetPrimitiveType(GraphicsDriver::PT_Triangle);
		pVB->SetVertices(snew Vertex3D[maxBullets*6], maxBullets*6, Vertex3D::VF_Pos|Vertex3D::VF_UV);
		pVB->SetNumVertices(0);
		pGraphic->SetVertexBuffer(pVB);

		pVB->SetDynamic(true);
	}	

	//! called during the update of the entity		
	void PelletManager::Update()
	{
		VertexBuffer* pVB = GetComponent<GraphicComponent>()->GetVertexBuffer();
		pVB->SetNumVertices(0);

		if(m_Pool.GetElementCount())
		{
			int bulletIndex = 0;
			for (size_t i = 0; i < m_Pool.GetCapacity(); ++i)			
			{
				auto bullet = m_Pool.GetElement(i);
				if (!bullet)
					continue;

				Vector3 vClosestActor = Vector3::Zero;
				float fDistToClosestActor = Math::Epsilon;
				if (AIMGR->GetClosestActor(Actor::T_Enemy, bullet->vPosition, m_fMinCos, vClosestActor, fDistToClosestActor))
				{
					float fDistFactor = m_fMinDistToActor/fDistToClosestActor;
					float fInterpolator = m_fHomingFactor*fDistFactor*fDistFactor*g_fDeltaTime;
					fInterpolator = Math::Clamp(fInterpolator, 0.0f, 1.0f);
					Vector3 vDirectionToActor = (vClosestActor - bullet->vPosition).Normalize();
					bullet->vDirection = (vDirectionToActor - bullet->vDirection)*fInterpolator + bullet->vDirection;
				}
				
				bullet->vPosition += bullet->vDirection*bullet->fSpeed*g_fDeltaTime;
				SetupRendering(bullet, bulletIndex++, pVB);

				bullet->fLife -= g_fDeltaTime;
				if (bullet->fLife < 0.0f)
					m_Pool.Free(bullet);
			}

			pVB->SetDirty(true);
		}
	}

	//! Prepares a bullet for rendering
	void PelletManager::SetupRendering(Bullet* pBullet, int index, VertexBuffer* pVB)
	{
		Vertex3D* pVertices = reinterpret_cast<Vertex3D*>(pVB->GetVertices());
		const Matrix44& viewInverse = EntityRenderer::Instance()->Get3DCamera()->GetViewMatrixInverse();
		const float fHalfSize = pBullet->fRadius;
		const int vtx = index*6;
		pVertices[vtx+0].UV = Vector2::Create(0.0f, 1.0f); pVertices[vtx+0].Pos = Vector3::Create(-fHalfSize, -fHalfSize, 0.0f);
		pVertices[vtx+1].UV = Vector2::Create(0.0f, 0.0f); pVertices[vtx+1].Pos = Vector3::Create(-fHalfSize, fHalfSize, 0.0f);
		pVertices[vtx+2].UV = Vector2::Create(1.0f, 0.0f); pVertices[vtx+2].Pos = Vector3::Create(fHalfSize, fHalfSize, 0.0f);
		pVertices[vtx+3].UV = Vector2::Create(0.0f, 1.0f); pVertices[vtx+3].Pos = Vector3::Create(-fHalfSize, -fHalfSize, 0.0f);
		pVertices[vtx+4].UV = Vector2::Create(1.0f, 0.0f); pVertices[vtx+4].Pos = Vector3::Create(fHalfSize, fHalfSize, 0.0f);
		pVertices[vtx+5].UV = Vector2::Create(1.0f, 1.0f); pVertices[vtx+5].Pos = Vector3::Create(fHalfSize, -fHalfSize, 0.0f);
		Matrix44 transform = viewInverse;
		transform.SetTranslation(pBullet->vPosition);
		for(int i=0; i<6; ++i)
		{
			pVertices[vtx+i].Pos = transform.TransformVect(pVertices[vtx+i].Pos);
		}
		pVB->SetNumVertices(vtx+6);
	}
}

