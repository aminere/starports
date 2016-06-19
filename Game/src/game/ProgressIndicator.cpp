/*

Amine Rehioui
Created: April. 18th 2015

*/

#include "Game.h"

#include "ProgressIndicator.h"

namespace shoot
{
	DEFINE_OBJECT(ProgressIndicator);

	//! constructor
	ProgressIndicator::ProgressIndicator()
		: m_Time(0.0f)
		, m_Finished(false)
		, m_RemoveOnCompletion(true)
		// properties
		, m_NumSegments(10)
		, m_Radius(10.0f)
		, m_Thickness(1.0f)
		, m_Duration(1.0f)
	{
	}

	//! serializes the entity to/from a PropertyStream
	void ProgressIndicator::Serialize(PropertyStream& stream)
	{
		super::Serialize(stream);

		stream.Serialize("Segments", &m_NumSegments);
		stream.Serialize("Radius", &m_Radius);
		stream.Serialize("Thickness", &m_Thickness);
		stream.Serialize("Duration", &m_Duration);
	}

	//! called during the initialization of the entity
	void ProgressIndicator::Init()
	{
		GraphicComponent* gc = GetComponent<GraphicComponent>();
		if (!gc)
		{
			gc = snew GraphicComponent();
			gc->SetRenderingPass(GraphicComponent::RP_3DDepthOff);
			AddComponent(gc, true);
		}

		auto numVertices = m_NumSegments * 6;
		auto vertices = snew Vertex3D[numVertices];
		for (int i = 0; i < numVertices; ++i)		
			vertices[i].UV = Vector2::Zero;		
		m_VB = GraphicsDriver::Instance()->CreateVertexBuffer();
		m_VB->SetName(GetClassName());
		m_VB->SetVertices(vertices, numVertices, Vertex3D::VF_Pos|Vertex3D::VF_UV);
		m_VB->SetNumVertices(0);
		m_VB->SetDynamic(true);
		gc->SetVertexBuffer(m_VB);

		m_TipIndicator = static_cast<Entity3D*>(GetChildByName("Tip"));
		if (m_TipIndicator.IsValid())
			m_TipIndicator->SetVisible(false);

		super::Init();
	}

	//! called during the update of the entity
	void ProgressIndicator::Update()
	{
		if (m_Finished)
		{
			if (m_RemoveOnCompletion)
				GetParent()->RemoveChild(this);
			else
				SetVisibleAndEnabled(false);
			return;
		}

		if (m_Time < m_Duration)
		{
			UpdateProgress(m_Time / m_Duration);

			m_Time += g_fDeltaTime;
			m_VB->SetDirty(true);
		}
		else
		{
			UpdateProgress(1.0f, true);
			m_Finished = true;
		}
	}

	//! UpdateProgress
	void ProgressIndicator::UpdateProgress(float factor, bool finished /*= false*/)
	{
		auto vertices = reinterpret_cast<Vertex3D*>(m_VB->GetVertices());
		auto angleRange = 360.0f * factor;
		auto angleStep = 360.0f / m_NumSegments;
		auto fNumSegments = angleRange / angleStep;
		angleStep *= Math::DegToRadFactor;
		auto numSegments = (int)Math::Floor(fNumSegments);
		auto lastSegmentFract = Math::Fraction(fNumSegments);
		if (lastSegmentFract > Math::Epsilon)
			numSegments++;

		if (finished)
			lastSegmentFract = 1.0f;

		auto angle = 0.0f;
		auto lateral0 = Vector3::Create(-1.0f, 0.0f, 0.0f) * m_Thickness;
		auto lateral1 = lateral0;
		auto point1 = Vector3::Create(Math::Cos(angle), -Math::Sin(angle), 0.0f)*m_Radius;
		m_TipIndicator->SetVisible(true);
		m_TipIndicator->SetPosition(point1);
		int vtx = 0;
		for (int i = 0; i < numSegments; ++i)
		{
			bool lastSegment = (i == numSegments - 1);
			if (lastSegment)
				angleStep *= lastSegmentFract;

			auto point2 = Vector3::Create(Math::Cos(angle + angleStep), -Math::Sin(angle + angleStep), 0.0f)*m_Radius;

			if (lastSegment)
				m_TipIndicator->SetPosition(point2);

			auto dir = (point2 - point1).Normalized();
			auto lateral2 = dir.CrossProduct(Vector3::Up) * m_Thickness;

			if (i == (m_NumSegments - 1))
				lateral2 = lateral0;

			SHOOT_ASSERT(vtx + 5 < (int)m_VB->GetMaxVertices(), "Buffer Overflow detected");
			vertices[vtx + 0].Pos = point1 + lateral1;
			vertices[vtx + 1].Pos = point1 - lateral1;
			vertices[vtx + 2].Pos = point2 + lateral2;
			vertices[vtx + 3].Pos = point2 + lateral2;
			vertices[vtx + 4].Pos = point1 - lateral1;
			vertices[vtx + 5].Pos = point2 - lateral2;

			angle += angleStep;
			lateral1 = lateral2;
			point1 = point2;
			vtx += 6;
		}

		m_VB->SetNumVertices(vtx);
		m_VB->SetDirty(true);
	}

	//! UpdateDuration
	void ProgressIndicator::UpdateDuration(float newDuration)
	{
		auto factor = m_Time / m_Duration;
		auto newTime = factor * newDuration;
		m_Duration = newDuration;
		m_Time = newTime;
	}

	//! StartProgress
	void ProgressIndicator::StartProgress(float duration)
	{
		m_Time = 0.0f;
		m_Duration = duration;
		m_VB->SetNumVertices(0);
		SetVisibleAndEnabled(true);
		m_Finished = false;
	}
}

