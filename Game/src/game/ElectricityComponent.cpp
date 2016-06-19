/* 

Amine Rehioui
Created: April 16th 2014

*/

#include "Game.h"

#include "ElectricityComponent.h"
#include "MaterialProvider.h"
#include "Pathway.h"
#include "Basis3.h"

namespace shoot
{
	DEFINE_OBJECT(ElectricityComponent);

	//! constructor
	ElectricityComponent::ElectricityComponent()
		: m_bActive(false)
		// properties
		, m_NumSegments(20)
		, m_CoilCount(2)
		, m_rAmplitude(0.3f, 1.0f)
		, m_ChargeDuration(1.0f, 2.0f)
		, m_fBeamThickness(1.0f)
	{
		SetRenderingPass(GraphicComponent::RP_3DTransparent);
	}

	//! serializes the entity to/from a PropertyStream
	void ElectricityComponent::Serialize(PropertyStream& stream)
	{
		super::Serialize(stream);

		stream.Serialize("ChargeDuration", &m_ChargeDuration);

		stream.Serialize("AmplitudeRange", &m_rAmplitude);
		stream.Serialize("NumSegments", &m_NumSegments);
		stream.Serialize("CoilCount", &m_CoilCount);
		stream.Serialize("BeamThickness", &m_fBeamThickness);
	}

	//! Initializes the component
	void ElectricityComponent::Init()
	{
		SHOOT_ASSERT(GetOwner()->IsA(Pathway::TypeID), "PathwayGraphicComponent's parent not of type Pathway");

		// coils VB
		VertexBuffer* pCoilsVB = GraphicsDriver::Instance()->CreateVertexBuffer();
		pCoilsVB->SetPrimitiveType(GraphicsDriver::PT_Line);
		pCoilsVB->SetName(GetClassName());
		pCoilsVB->SetDynamic(true);
		pCoilsVB->SetVertices(snew Vertex3D[m_NumSegments*2*m_CoilCount], m_NumSegments*2*m_CoilCount, Vertex3D::VF_Pos|Vertex3D::VF_UV);
		pCoilsVB->SetApplyWorldTransforms(false);
		SetVertexBuffer(pCoilsVB, 0);

		// beam VB
		VertexBuffer* pBeamVB = GraphicsDriver::Instance()->CreateVertexBuffer();		
		pBeamVB->SetPrimitiveType(GraphicsDriver::PT_Triangle);
		pBeamVB->SetName(GetClassName());
		pBeamVB->SetVertices(snew Vertex3D[m_NumSegments*6], m_NumSegments*6, Vertex3D::VF_Pos|Vertex3D::VF_UV);
		pBeamVB->SetDynamic(true);
		pBeamVB->SetApplyWorldTransforms(false);
		SetVertexBuffer(pBeamVB, 1);

		InitCoils();
		InitBeam();
	}

	//! Updates the component
	void ElectricityComponent::Update()
	{
		VertexBuffer* pCoilsVB = GetVertexBuffer(0);
		VertexBuffer* pBeamVB = GetVertexBuffer(1);
		Vertex3D* pCoilVertices = reinterpret_cast<Vertex3D*>(pCoilsVB->GetVertices());
		Vertex3D* pBeamVertices = reinterpret_cast<Vertex3D*>(pBeamVB->GetVertices());
		Pathway* pPath = static_cast<Pathway*>(GetOwner());

		if(m_bActive)
		{
			// Coils
			pCoilsVB->SetNumVertices(pCoilsVB->GetMaxVertices());
			for(int i=0; i<m_CoilCount; ++i)
			{
				m_ChargeTimers[i] -= g_fDeltaTime;
				if(m_ChargeTimers[i] < 0.0f)
				{
					int currentVertex = m_NumSegments*2*i;
					GenerateCoil(pCoilVertices, currentVertex, pCoilsVB->GetMaxVertices());
					m_ChargeTimers[i] = Random::GetFloat(m_ChargeDuration.Min, m_ChargeDuration.Max);
					pCoilsVB->SetDirty(true);
				}
			}
		}

		// Beam
		const float fStep = 1.0f/float(m_NumSegments);
		pBeamVB->SetNumVertices(0);
		uint vtx = 0;
		for(float f=0.0f; f<1.0f && vtx<pBeamVB->GetMaxVertices(); f+=fStep)
		{
			Vector3 v1 = pPath->GetPathPosition(f);
			Vector3 v11 = pPath->GetPathPosition(f+Math::Epsilon);
			Vector3 v2 = pPath->GetPathPosition(f+fStep);
			Vector3 v22 = pPath->GetPathPosition(f+fStep+Math::Epsilon);
			Vector3 vLateralAxe1 = ((v11-v1).Normalize()).CrossProduct(Vector3::Up)*m_fBeamThickness;
			Vector3 vLateralAxe2 = ((v22-v2).Normalize()).CrossProduct(Vector3::Up)*m_fBeamThickness;
			pBeamVertices[vtx+0].Pos = v1+vLateralAxe1;
			pBeamVertices[vtx+1].Pos = v1-vLateralAxe1;
			pBeamVertices[vtx+2].Pos = v2+vLateralAxe2;
			pBeamVertices[vtx+3].Pos = v2+vLateralAxe2;
			pBeamVertices[vtx+4].Pos = v1-vLateralAxe1;
			pBeamVertices[vtx+5].Pos = v2-vLateralAxe2;
			vtx += 6;
		}

		pBeamVB->SetNumVertices(vtx);
		pBeamVB->SetDirty(true);
	}

	//! Initializes the coils
	void ElectricityComponent::InitCoils()
	{
		VertexBuffer* pCoilVB = GetVertexBuffer(0);
		Vertex3D* pCoilVertices = reinterpret_cast<Vertex3D*>(pCoilVB->GetVertices());
		m_ChargeTimers.clear();
		pCoilVB->SetNumVertices(0);

		int currentVertex = 0;
		for(int i=0; i<m_CoilCount; ++i)
		{
			GenerateCoil(pCoilVertices, currentVertex, pCoilVB->GetMaxVertices());
			m_ChargeTimers.push_back(-1.0f);
		}
	}

	//! Initializes the beam
	void ElectricityComponent::InitBeam()
	{
		VertexBuffer* pBeamVB = GetVertexBuffer(1);
		Vertex3D* pBeamVertices = reinterpret_cast<Vertex3D*>(pBeamVB->GetVertices());
		pBeamVB->SetNumVertices(0);
		for(uint vtx = 0; vtx<pBeamVB->GetMaxVertices(); vtx += 6)
		{
			pBeamVertices[vtx+0].UV = Vector2::Create(0.0f, 0.0f);
			pBeamVertices[vtx+1].UV = Vector2::Create(0.0f, 1.0f);
			pBeamVertices[vtx+2].UV = Vector2::Create(1.0f, 0.0f);
			pBeamVertices[vtx+3].UV = Vector2::Create(1.0f, 0.0f);
			pBeamVertices[vtx+4].UV = Vector2::Create(0.0f, 1.0f);
			pBeamVertices[vtx+5].UV = Vector2::Create(1.0f, 1.0f);
		}
	}

	//! generates a coil
	void ElectricityComponent::GenerateCoil(Vertex3D* pVertices, int& currentVertex, int maxVertices)
	{
		Pathway* pPath = static_cast<Pathway*>(GetOwner());
		const float fStep = 1.0f/float(m_NumSegments);
		int localVtx = 0;
		for(float f=0.0f; f<1.0f && currentVertex<maxVertices; f+=fStep)
		{
			Vector3 v1 = pPath->GetPathPosition(f);
			Vector3 v2 = pPath->GetPathPosition(f+fStep);
			Vector3 vDirection = (v2-v1).Normalize();
			Basis3 localBasis(Vector3::Zero, vDirection);
			float fAmplitude = Random::GetFloat(m_rAmplitude.Min, m_rAmplitude.Max);
			float fLateralFactor = Random::GetFloat(-fAmplitude, fAmplitude);
			float fVerticalFactor = Random::GetFloat(-fAmplitude, fAmplitude);
			if(localVtx == 0)
			{
				pVertices[currentVertex+0].Pos = v1 + localBasis.Lateral*fLateralFactor + localBasis.Vertical*fVerticalFactor;
				pVertices[currentVertex+1].Pos = v2 + localBasis.Lateral*fLateralFactor + localBasis.Vertical*fVerticalFactor;
			}
			else
			{
				pVertices[currentVertex+0].Pos = pVertices[currentVertex-1].Pos;
				pVertices[currentVertex+1].Pos = v2 + localBasis.Lateral*fLateralFactor + localBasis.Vertical*fVerticalFactor;
			}
			currentVertex += 2;
			localVtx += 2;
		}
	}

	//! Activates the electricity
	void ElectricityComponent::Activate(bool bActive)
	{
		m_bActive = bActive; 
		for (size_t i=0; i<m_ChargeTimers.size(); ++i)
		{
			m_ChargeTimers[i] = -1.0f;
		}
	}
}

