/*

Amine Rehioui
Created: Feb. 9th 2015

*/

#include "Shoot.h"

#include "TrailEntity.h"

namespace shoot
{
	DEFINE_OBJECT(TrailEntity);

	//! constructor
	TrailEntity::TrailEntity()
		// properties
		: m_NumEdges(3)
		, m_TrailDuration(1.0f)
		, m_Thickness(1.0f)
		, m_Treshold(0.01f)
		, m_UPerSegment(1.0f)
		, m_FadeFactor(1.0f)
	{
	}

	//! serializes the entity to/from a PropertyStream
	void TrailEntity::Serialize(PropertyStream& stream)
	{
		super::Serialize(stream);

		stream.Serialize("NumEdges", &m_NumEdges);
		stream.Serialize("TrailDuration", &m_TrailDuration);
		stream.Serialize("Thickness", &m_Thickness);
		stream.Serialize("Treshold", &m_Treshold);
		stream.Serialize("UPerSegment", &m_UPerSegment);
		stream.Serialize("FadeFactor", &m_FadeFactor);
	}

	//! called during the initialization of the entity
	void TrailEntity::Init()
	{
		GraphicComponent* gc = GetComponent<GraphicComponent>();
		if (!gc)
		{
			gc = snew GraphicComponent();
			gc->SetRenderingPass(GraphicComponent::RP_3DTransparent);
			AddComponent(gc, true);
		}

		auto numTriangles = (m_NumEdges - 1) * 2;
		auto numVertices = numTriangles * 3;
		auto vb = GraphicsDriver::Instance()->CreateVertexBuffer();
		vb->SetName(GetClassName());
		vb->SetDynamic(true);
		vb->SetVertices(snew Vertex3D[numVertices], numVertices, Vertex3D::VF_Pos | Vertex3D::VF_UV | Vertex3D::VF_Color);
		vb->SetNumVertices(0);
		vb->SetApplyWorldTransforms(false);
		gc->SetVertexBuffer(vb);

		auto startingPos = GetAbsolutePosition();
		for (uint i = 0; i < m_NumEdges; ++i)
			m_Points.push_back(startingPos);

		super::Init();
	}

	//! called during the update of the entity
	void TrailEntity::Update()
	{
		const auto& pos = GetAbsolutePosition();
		auto vb = GetComponent<GraphicComponent>()->GetVertexBuffer();
		vb->SetNumVertices(0);

		// update points
		for (size_t i = 0; i < m_Points.size(); ++i)
		{
			auto& point = m_Points[i];
			auto factor = (float)i / (m_Points.size() - 1);
			auto duration = Math::Lerp(0.0f, m_TrailDuration, factor);
			point = Math::Damp(point, pos, g_fDeltaTime, duration);
		}

		// discard points that are too close to each other
		auto thresholdSQ = m_Treshold*m_Treshold;
		std::vector<Vector3> points;
		points.push_back(m_Points[0]);
		for (int i = 0; i < (int)m_Points.size()-1; ++i)
		{
			auto& point1 = m_Points[i];
			auto& point2 = m_Points[i+1];
			auto distSQ = (point1 - point2).GetLengthSquared();
			if (distSQ < thresholdSQ)
				continue;

			points.push_back(point2);
		}

		if (points.size() > 1)
		{
			auto vertices = reinterpret_cast<Vertex3D*>(vb->GetVertices());
			uint vtx = 0;
			auto texU = 0.0f;
			auto step = 1.0f / (points.size() - 1);
			auto factor = 1.0f;
			auto alphaFactor = 1.0f;
			for (int i = 0; i < (int)points.size() - 1; ++i)
			{
				const auto& v1 = points[i];
				const auto& v2 = points[i + 1];
				const auto& dir1 = (v2 - v1).Normalized();
				Vector3 dir2;
				auto afterNext = i+2;
				if (afterNext < (int)points.size())
					dir2 = (points[afterNext] - v2).Normalized();
				else
					dir2 = dir1;

				auto thickness1 = m_Thickness*factor;
				auto thickness2 = m_Thickness*(factor - step);
				auto vLateralAxe1 = dir1.CrossProduct(Vector3::Up)*thickness1;
				auto vLateralAxe2 = dir2.CrossProduct(Vector3::Up)*thickness2;
				vertices[vtx + 0].UV = Vector2::Create(texU, 0.0f); vertices[vtx + 0].Pos = v1 + vLateralAxe1;
				vertices[vtx + 1].UV = Vector2::Create(texU, 1.0f); vertices[vtx + 1].Pos = v1 - vLateralAxe1;
				vertices[vtx + 2].UV = Vector2::Create(texU + m_UPerSegment, 0.0f); vertices[vtx + 2].Pos = v2 + vLateralAxe2;
				vertices[vtx + 3].UV = Vector2::Create(texU + m_UPerSegment, 0.0f); vertices[vtx + 3].Pos = v2 + vLateralAxe2;
				vertices[vtx + 4].UV = Vector2::Create(texU, 1.0f); vertices[vtx + 4].Pos = v1 - vLateralAxe1;
				vertices[vtx + 5].UV = Vector2::Create(texU + m_UPerSegment, 1.0f); vertices[vtx + 5].Pos = v2 - vLateralAxe2;
								
				// update alpha
				auto alpha1 = alphaFactor;
				auto alpha2 = alphaFactor - (step * m_FadeFactor);
				auto color1 = Color::Create(Color::White.RGB, Math::Clamp(alpha1, 0.0f, alpha1));
				auto color2 = Color::Create(Color::White.RGB, Math::Clamp(alpha2, 0.0f, alpha2));
				vertices[vtx + 0].color = color1;
				vertices[vtx + 1].color = color1;
				vertices[vtx + 2].color = color2;
				vertices[vtx + 3].color = color2;
				vertices[vtx + 4].color = color1;
				vertices[vtx + 5].color = color2;

				vtx += 6;
				texU += m_UPerSegment;
				factor -= step;
				alphaFactor -= (step * m_FadeFactor);
			}
			vb->SetNumVertices(vtx);
			vb->SetDirty(true);
		}
	}
}

