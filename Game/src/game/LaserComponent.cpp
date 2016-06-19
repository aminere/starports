/* 

Amine Rehioui
Created: April 16th 2014

*/

#include "Game.h"

#include "LaserComponent.h"
#include "Quaternion.h"
#include "Basis3.h"

namespace shoot
{
	DEFINE_OBJECT(LaserComponent);

	//! constructor
	LaserComponent::LaserComponent()
		: m_bActive(false)
		, m_fLength(0.0f)
		, m_fMaxLength(10.0f)
		, m_fOcillationTimer(-1.0f)
		, m_fThickness(0.0f)
		// properties		
		, m_fFrequency(0.02f)
		, m_ThicknessRange(0.8f, 1.2f)
		, m_fDeploySpeed(10.0f)
	{
		SetRenderingPass(GraphicComponent::RP_3DTransparent);
	}

	//! serializes the entity to/from a PropertyStream
	void LaserComponent::Serialize(PropertyStream& stream)
	{
		super::Serialize(stream);
		
		stream.Serialize("DeploySpeed", &m_fDeploySpeed);
		stream.Serialize("Frequency", &m_fFrequency);
		stream.Serialize("ThicknessRange", &m_ThicknessRange);
	}

	//! Initializes the component
	void LaserComponent::Init()
	{
		SHOOT_ASSERT(GetOwner()->IsA(Entity3D::TypeID), "LaserComponent's parent not of type Entity3D");
				
		VertexBuffer* pVB = GraphicsDriver::Instance()->CreateVertexBuffer();
		int maxVertices = 24;
		Vertex3D* pVertices = snew Vertex3D[maxVertices];
		pVB->SetVertices(pVertices, maxVertices, Vertex3D::VF_Pos|Vertex3D::VF_UV);
		pVB->SetName(GetClassName());
		pVB->SetApplyWorldTransforms(false);
		pVB->SetDynamic(true);
		pVB->SetNumVertices(0);
		SetVertexBuffer(pVB, 0);
	}

	//! Updates the component
	void LaserComponent::Update()
	{
		if(!m_bActive)		
			return;		

		VertexBuffer* pVB = GetVertexBuffer(0);
		Vertex3D* pVertices = reinterpret_cast<Vertex3D*>(pVB->GetVertices());

		Vector3 vStart = m_StartPoint->GetAbsolutePosition();
		Vector3 vToTarget = m_vTarget - vStart;
		float fToTarget = vToTarget.GetLength();
		Vector3 vDirection = vToTarget / fToTarget;
		Basis3 localBasis(Vector3::Zero, vDirection);		

		if(m_fOcillationTimer < 0.0f)
		{
			m_fThickness = Random::GetFloat(m_ThicknessRange.Min, m_ThicknessRange.Max);
			m_fOcillationTimer = m_fFrequency;
		}
		else
		{
			m_fOcillationTimer -= g_fDeltaTime;
		}

		localBasis.Lateral = localBasis.Lateral*m_fThickness;
		localBasis.Vertical = localBasis.Vertical*m_fThickness;
		Vector3 vDiagonalAxe = Quaternion(vDirection, Math::PIBy4).Rotate(localBasis.Lateral);
		Vector3 vDiagonalAxe2 = Vector3::Create(vDiagonalAxe.X, vDiagonalAxe.Y, -vDiagonalAxe.Z);
		Vector3 vEnd = vStart + vDirection * m_fLength;

		pVertices[0].UV = Vector2::Create(0.0f, 0.0f); pVertices[0].Pos = vEnd+localBasis.Lateral;
		pVertices[1].UV = Vector2::Create(1.0f, 0.0f); pVertices[1].Pos = vStart+localBasis.Lateral;
		pVertices[2].UV = Vector2::Create(0.0f, 1.0f); pVertices[2].Pos = vEnd-localBasis.Lateral;
		pVertices[3].UV = Vector2::Create(0.0f, 1.0f); pVertices[3].Pos = vEnd-localBasis.Lateral;
		pVertices[4].UV = Vector2::Create(1.0f, 0.0f); pVertices[4].Pos = vStart+localBasis.Lateral;
		pVertices[5].UV = Vector2::Create(1.0f, 1.0f); pVertices[5].Pos = vStart-localBasis.Lateral;

		pVertices[6].UV = Vector2::Create(0.0f, 0.0f); pVertices[6].Pos = vEnd+localBasis.Vertical;
		pVertices[7].UV = Vector2::Create(1.0f, 0.0f); pVertices[7].Pos = vStart+localBasis.Vertical;
		pVertices[8].UV = Vector2::Create(0.0f, 1.0f); pVertices[8].Pos = vEnd-localBasis.Vertical;
		pVertices[9].UV = Vector2::Create(0.0f, 1.0f); pVertices[9].Pos = vEnd-localBasis.Vertical;
		pVertices[10].UV = Vector2::Create(1.0f, 0.0f); pVertices[10].Pos = vStart+localBasis.Vertical;
		pVertices[11].UV = Vector2::Create(1.0f, 1.0f); pVertices[11].Pos = vStart-localBasis.Vertical;

		pVertices[12].UV = Vector2::Create(0.0f, 0.0f); pVertices[12].Pos = vEnd+vDiagonalAxe;
		pVertices[13].UV = Vector2::Create(1.0f, 0.0f); pVertices[13].Pos = vStart+vDiagonalAxe;
		pVertices[14].UV = Vector2::Create(0.0f, 1.0f); pVertices[14].Pos = vEnd-vDiagonalAxe;
		pVertices[15].UV = Vector2::Create(0.0f, 1.0f); pVertices[15].Pos = vEnd-vDiagonalAxe;
		pVertices[16].UV = Vector2::Create(1.0f, 0.0f); pVertices[16].Pos = vStart+vDiagonalAxe;
		pVertices[17].UV = Vector2::Create(1.0f, 1.0f); pVertices[17].Pos = vStart-vDiagonalAxe;

		pVertices[18].UV = Vector2::Create(0.0f, 0.0f); pVertices[18].Pos = vEnd+vDiagonalAxe2;
		pVertices[19].UV = Vector2::Create(1.0f, 0.0f); pVertices[19].Pos = vStart+vDiagonalAxe2;
		pVertices[20].UV = Vector2::Create(0.0f, 1.0f); pVertices[20].Pos = vEnd-vDiagonalAxe2;
		pVertices[21].UV = Vector2::Create(0.0f, 1.0f); pVertices[21].Pos = vEnd-vDiagonalAxe2;
		pVertices[22].UV = Vector2::Create(1.0f, 0.0f); pVertices[22].Pos = vStart+vDiagonalAxe2;
		pVertices[23].UV = Vector2::Create(1.0f, 1.0f); pVertices[23].Pos = vStart-vDiagonalAxe2;		
				
		m_fLength += m_fDeploySpeed * g_fDeltaTime;		
		m_fLength = Math::Clamp(m_fLength, 0.0f, m_fMaxLength);		
		
		pVB->SetNumVertices(pVB->GetMaxVertices());
		pVB->SetDirty(true);
	}	

	//! activates the laser
	void LaserComponent::Activate(const Vector3& vTarget)
	{
		m_StartPoint = static_cast<Entity3D*>(GetOwner());
		m_vTarget = vTarget;
		Vector3 vToTarget = m_vTarget - m_StartPoint->GetAbsolutePosition();
		m_fLength = 0.0f;
		m_fMaxLength = vToTarget.GetLength();
		m_bActive = true;
	}

	//! Deactivate
	void LaserComponent::Deactivate()
	{
		GetVertexBuffer(0)->SetNumVertices(0);
		m_bActive = false;
	}
}

