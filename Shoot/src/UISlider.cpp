/* 

Amine Rehioui
Created: October 9th 2013

*/

#include "Shoot.h"

#include "UISlider.h"

#include "Image.h"

#include "InputManager.h"
#include "EventManager.h"
#include "Utils.h"

namespace shoot
{
	DEFINE_OBJECT(UISlider);

	//! Constructor
	UISlider::UISlider()
		: m_fRatio(0.0f)
		, m_bStartDragging(false)
	{
	}

	//! serializes the entity to/from a PropertyStream
	void UISlider::Serialize(PropertyStream& stream)
	{
		super::Serialize(stream);

		stream.Serialize("PositionRange", &m_PositionRange);
	}

	//! called during the initialization of the entity
	void UISlider::Init()
	{
		super::Init();

		m_Selector = static_cast<Image*>(GetChildByName("Selector"));
		m_BG = static_cast<Image*>(GetChildByName("BG"));

		// Init bounding box
		if(m_Selector.IsValid())
		{
			m_BoundingBox = m_Selector->GetBoundingBox();
		}
	}

	//! called during the update of the entity
	void UISlider::Update()
	{
		if(!GetRootContext()->IsTopContext())
		{
			return;
		}

		InputManager* pInputMgr = InputManager::Instance();
		if(pInputMgr->IsTouchJustPressed())
		{
			m_bStartDragging = IsTouched();
			if(m_bStartDragging)
			{
				m_vPreviousTouchPos = pInputMgr->GetTouchState().vPosition;
			}
		}
		else if(pInputMgr->IsTouchPressed())
		{
			if(m_bStartDragging)
			{
				Vector2 vDeltaTouch = pInputMgr->GetTouchState().vPosition-m_vPreviousTouchPos;
				Vector2 iconPos = m_Selector->GetPosition();
				iconPos.X += vDeltaTouch.X;
				iconPos.X = Math::Clamp(iconPos.X, m_PositionRange.Min, m_PositionRange.Max);
				float fRatio = (iconPos.X-m_PositionRange.Min)/(m_PositionRange.Max-m_PositionRange.Min);

				m_Selector->SetPosition(iconPos);
				m_BG->GetComponent<GraphicComponent>()->GetMaterial()->SetColor(Math::Lerp(Color::Red, Color::Green, fRatio));

				UIEvent* pEvent = snew UIEvent();
				pEvent->m_Sender = this;
				pEvent->m_Command = std::string("SLIDER ") + GetName() + " " + Utils::ToString(fRatio);
				EventManager::Instance()->SendEvent(pEvent);

				m_vPreviousTouchPos = pInputMgr->GetTouchState().vPosition;
			}
		}
	}

	//! sets the slider ratio
	void UISlider::SetRatio(float fRatio)
	{
		if(m_Selector.IsValid())
		{
			Vector2 iconPos = m_Selector->GetPosition();
			iconPos.X = Math::Lerp(m_PositionRange.Min, m_PositionRange.Max, fRatio);
			m_Selector->SetPosition(iconPos);
			m_BG->GetComponent<GraphicComponent>()->GetMaterial()->SetColor(Math::Lerp(Color::Red, Color::Green, fRatio));
		}
	}

	//! returns true if the UISlider is touched
	bool UISlider::IsTouched() const
	{
		Vector3 vTranlation = m_Selector->GetWorldTransform().GetTranslation();
		AABBox2D bbox = m_Selector->GetBoundingBox() + Vector2::Create(vTranlation.X, vTranlation.Y);
		Vector2 vEnlargeOffset = {40.0f, 0.0f };
		bbox.Set(bbox.Min()-vEnlargeOffset, bbox.Max()+vEnlargeOffset);
		return bbox.Contains(InputManager::Instance()->GetTouchState().vPosition);
	}	
}

