/*

Amine Rehioui
Created: Feb. 16th 2015

*/

#include "Game.h"

#include "RangeIndicator.h"

namespace shoot
{
	DEFINE_OBJECT(RangeIndicator);

	//! constructor
	RangeIndicator::RangeIndicator()
		// properties
		: m_NumVertices(50)
	{
	}

	//! serializes the entity to/from a PropertyStream
	void RangeIndicator::Serialize(PropertyStream& stream)
	{
		super::Serialize(stream);

		stream.Serialize("NumVertices", &m_NumVertices);
	}

	//! called during the initialization of the entity
	void RangeIndicator::Init()
	{
		GraphicComponent* gc = GetComponent<GraphicComponent>();
		if (!gc)
		{
			gc = snew GraphicComponent();
			gc->SetRenderingPass(GraphicComponent::RP_3DDepthOff);
			AddComponent(gc, true);
		}

		auto angleStep = Math::PITimes2 / m_NumVertices;
		auto angle = 0.0f;
		auto vertices = snew Vertex3D[m_NumVertices];
		for (int i = 0; i < m_NumVertices; ++i)
		{
			vertices[i].Pos = Vector3::Create(Math::Cos(angle), Math::Sin(angle), 0.0f);
			vertices[i].UV = Vector2::Zero;
			angle += angleStep;
		}

		auto vb = GraphicsDriver::Instance()->CreateVertexBuffer();
		vb->SetPrimitiveType(GraphicsDriver::PT_LineLoop);
		vb->SetName(GetClassName());
		vb->SetVertices(vertices, m_NumVertices, Vertex3D::VF_Pos|Vertex3D::VF_UV);
		vb->SetDynamic(true);
		gc->SetVertexBuffer(vb);

		super::Init();
	}

	//! called during the update of the entity
	void RangeIndicator::Update()
	{
	}
}

