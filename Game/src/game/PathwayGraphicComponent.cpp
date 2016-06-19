/* 

Amine Rehioui
Created: December 27th 2013

*/

#include "Game.h"

#include "PathwayGraphicComponent.h"

#include "Pathway.h"

namespace shoot
{
	DEFINE_OBJECT(PathwayGraphicComponent);

	//! constructor
	PathwayGraphicComponent::PathwayGraphicComponent()
		: m_fTextureOffsetX(0.0f)
		// properties
		, m_NumSegments(20)
		, m_fAnimSpeed(1.0f)
		, m_fThickness(2.0f)
		, _UPerSegment(1.0f)
	{
		SetRenderingPass(GraphicComponent::RP_3DTransparent);
	}

	//! serializes the entity to/from a PropertyStream
	void PathwayGraphicComponent::Serialize(PropertyStream& stream)
	{
		super::Serialize(stream);

		stream.Serialize("NumSegments", &m_NumSegments);
		stream.Serialize("AnimSpeed", &m_fAnimSpeed);
		stream.Serialize("Thickness", &m_fThickness);
		stream.Serialize("UPerSegment", &_UPerSegment);		

		if(GetOwner() 
		&& GetOwner()->IsInitialized() 
		&& stream.GetMode() == SM_Read)
		{
			SetNumSegments(m_NumSegments);
			GetVertexBuffer()->SetNumVertices(0);
			UpdatePath();
		}
	}

	//! Initializes the component
	void PathwayGraphicComponent::Init()
	{
		SHOOT_ASSERT(GetOwner()->IsA(Pathway::TypeID), "PathwayGraphicComponent's parent not of type Pathway");
		
		VertexBuffer* pVB = GraphicsDriver::Instance()->CreateVertexBuffer();		
		pVB->SetPrimitiveType(GraphicsDriver::PT_Triangle);
		pVB->SetName(GetClassName());
		pVB->SetVertices(snew Vertex3D[m_NumSegments*6], m_NumSegments*6, Vertex3D::VF_Pos|Vertex3D::VF_UV);
		pVB->SetNumVertices(0);
		pVB->SetDynamic(true);
		SetVertexBuffer(pVB);

		UpdatePath();
	}

	//! Updates the component
	void PathwayGraphicComponent::Update()
	{
		UpdatePath();

		// update animation
		m_fTextureOffsetX -= m_fAnimSpeed * g_fDeltaTime;
		Matrix44 textureTransform = GetTextureTransform();
		textureTransform.SetTranslation(Vector3::Create(m_fTextureOffsetX, 0.0f, 0.0f));
		SetTextureTransform(textureTransform);
	}

	//! Updates the path
	void PathwayGraphicComponent::UpdatePath()
	{
		Pathway* pPath = static_cast<Pathway*>(GetOwner());
		if (pPath->GetChildCount() < 2)
			return;

		const float fStep = 1.0f/float(m_NumSegments);
		VertexBuffer* pVB = GetVertexBuffer();
		pVB->SetNumVertices(0);
		Vertex3D* pVertices = reinterpret_cast<Vertex3D*>(pVB->GetVertices());

		uint vtx = 0;
		auto texU = 0.0f;
		for(float f=0.0f; f<1.0f && vtx<pVB->GetMaxVertices(); f+=fStep)
		{
			Vector3 v1 = pPath->GetPathPosition(f);
			Vector3 v11 = pPath->GetPathPosition(f+Math::Epsilon);
			Vector3 v2 = pPath->GetPathPosition(f+fStep);
			Vector3 v22 = pPath->GetPathPosition(f+fStep+Math::Epsilon);
			Vector3 vLateralAxe1 = ((v11-v1).Normalize()).CrossProduct(Vector3::Up)*m_fThickness;
			Vector3 vLateralAxe2 = ((v22-v2).Normalize()).CrossProduct(Vector3::Up)*m_fThickness;
			pVertices[vtx+0].UV = Vector2::Create(texU, 0.0f); pVertices[vtx+0].Pos = v1+vLateralAxe1;
			pVertices[vtx+1].UV = Vector2::Create(texU, 1.0f); pVertices[vtx+1].Pos = v1-vLateralAxe1;
			pVertices[vtx+2].UV = Vector2::Create(texU+_UPerSegment, 0.0f); pVertices[vtx+2].Pos = v2+vLateralAxe2;
			pVertices[vtx+3].UV = Vector2::Create(texU+_UPerSegment, 0.0f); pVertices[vtx+3].Pos = v2+vLateralAxe2;
			pVertices[vtx+4].UV = Vector2::Create(texU, 1.0f); pVertices[vtx+4].Pos = v1-vLateralAxe1;
			pVertices[vtx+5].UV = Vector2::Create(texU+_UPerSegment, 1.0f); pVertices[vtx+5].Pos = v2-vLateralAxe2;

			vtx += 6;
			texU += _UPerSegment;
		}

		pVB->SetNumVertices(vtx);
		pVB->SetDirty(true);
	}

	//! SetNumSegments
	void PathwayGraphicComponent::SetNumSegments(int segments)
	{
		auto numVertices = segments * 6;
		if(GetVertexBuffer()->GetMaxVertices() == numVertices)
			return;

		m_NumSegments = segments;
		GetVertexBuffer()->SetVertices(snew Vertex3D[numVertices], numVertices, Vertex3D::VF_Pos|Vertex3D::VF_UV);
	}
}

