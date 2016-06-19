/* 

Amine Rehioui
Created: December 16th 2014

*/

#include "Game.h"

#include "UIProgressBar.h"

namespace shoot
{
	DEFINE_OBJECT(UIProgressBar);

	//! Constructor
	UIProgressBar::UIProgressBar()
		: m_Progress(1.0f)
		, m_Dirty(false)
	{
	}

	//! serializes the entity to/from a PropertyStream
	void UIProgressBar::Serialize(PropertyStream& stream)
	{
		super::Serialize(stream);
	}

	//! called during the initialization of the entity
	void UIProgressBar::Init()
	{
		super::Init();
	}

	//! called during the update of the entity
	void UIProgressBar::Update()
	{
		if(!m_Dirty)
			return;

		auto vb = GetComponent<GraphicComponent>()->GetVertexBuffer();
		auto vertices = reinterpret_cast<Vertex3D*>(vb->GetVertices());
		auto size = GetBoundingBox().Size() * Vector2::Create(m_Progress, 1.0f);
		vertices[0].UV = Vector2::Create(m_Progress, 0.0f); vertices[0].Pos = Vector3::Create(size.X, 0.0f, 0.0f); // Top Right
		vertices[1].UV = Vector2::Create(m_Progress, 1.0f); vertices[1].Pos = Vector3::Create(size.X, size.Y, 0.0f); // Bottom Right				
		vb->SetDirty(true);
		m_Dirty = false;
	}	
}

