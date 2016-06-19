/* 

Amine Rehioui
Created: December 15th 2014

*/

#include "Shoot.h"	

#include "LayoutComponent.h"

namespace shoot
{
	DEFINE_OBJECT(LayoutComponent);

	// Alignement literals
	const char* const g_strHorizontalAlignmentLiterals[] =
	{
		"Left",
		"Center",
		"Right",
		0
	};

	const char* const g_strVerticalAlignmentLiterals[] =
	{
		"Top",
		"Center",
		"Bottom",
		0
	};

	//! constructor
	LayoutComponent::LayoutComponent()	
		// properties		
		: m_HorizontalAlignment(HA_Left)
		, m_VerticalAlignment(VA_Top)
	{
	}
	//! Reads/Writes struct properties from/to a stream
	void LayoutComponent::Serialize(PropertyStream& stream)
	{
		super::Serialize(stream);	
		
		stream.Serialize("HorizontalAlignement", &m_HorizontalAlignment, ENUM_PARAMS1(g_strHorizontalAlignmentLiterals));
		stream.Serialize("VerticalAlignement", &m_VerticalAlignment, ENUM_PARAMS1(g_strVerticalAlignmentLiterals));

#ifdef SHOOT_EDITOR
		// dirtify the target
		auto target = DYNAMIC_CAST(GetOwner(), Entity2D);
		if(target && stream.GetMode() == SM_Read)
			target->SetPosition(target->GetPosition());
#endif // SHOOT_EDITOR
	}	

	//! Align
	Matrix44 LayoutComponent::Align(Entity2D* target, const Vector2& _position) const
	{
		auto camera = EntityRenderer::Instance()->Get2DCamera();
		if(!camera)
			return Matrix44::Identity;

		auto screenSize = GraphicsDriver::Instance()->GetViewPort().Size();		
		auto deltaOffset = camera->Get2DOffset().X - camera->Get2DOffset().Y;
		AABBox2D screenBox;
		if(deltaOffset > 0.0f)
		{
			// vertical bars
			auto barWidth = camera->Get2DOffset().X;
			screenBox = AABBox2D(Vector2::Create(barWidth, 0.0f), Vector2::Create(screenSize.X-barWidth, screenSize.Y));		
		}
		else
		{
			// horizontal bars
			auto barHeight = camera->Get2DOffset().Y;
			screenBox = AABBox2D(Vector2::Create(0.0f, barHeight), Vector2::Create(screenSize.X, screenSize.Y-barHeight));			
		}
		
		auto bbox = target->GetBoundingBox() * target->GetScale();
		auto position = _position;
		switch(m_HorizontalAlignment)
		{
		case HA_Left:
			position.X = screenBox.Min().X + _position.X;
			break;
		case HA_Right:
			position.X = (screenBox.Max().X - bbox.Size().X) + _position.X;
			break;
		case HA_Center:
			position.X = (screenBox.Min().X + (screenBox.Size().X/2.0f)) - (bbox.Size().X/2.0f) + _position.X;
			break;
		}

		switch(m_VerticalAlignment)
		{
		case VA_Top:
			position.Y = screenBox.Min().Y + _position.Y;
			break;
		case VA_Bottom:
			position.Y = (screenBox.Max().Y - bbox.Size().Y) + _position.Y;
			break;
		case VA_Center:
			position.Y = (screenBox.Min().Y + (screenBox.Size().Y/2.0f)) - (bbox.Size().Y/2.0f) + _position.Y;
			break;
		}

		auto matrix = Matrix44::Identity;
		matrix.Scale(Vector3::Create(target->GetScale(), 1.0f));
		matrix.Rotate(Vector3::Create(0.0f, 0.0f, target->GetRotation()*Math::DegToRadFactor));
		matrix.Translate(Vector3::Create(Math::Floor(position.X), Math::Floor(position.Y), 0.0f));
		return matrix;
	}	
}

