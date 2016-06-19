/* 

Amine Rehioui
Created: September 3rd 2013

*/

#include "Shoot.h"

#include "Button.h"

#include "Sprite.h"

#include "InputManager.h"
#include "EventManager.h"
#include "LayoutComponent.h"

namespace shoot
{
	DEFINE_OBJECT(Button);

	//! Constructor
	Button::Button()
		: m_bPressed(false)
		// properties
		, m_bCheckBox(false)
		, m_bChecked(true)
		, m_bCustomBBox(false)
		, m_UIEnabled(true)
	{
	}

	//! serializes the entity to/from a PropertyStream
	void Button::Serialize(PropertyStream& stream)
	{
		super::Serialize(stream);

		stream.Serialize("UIEnabled", &m_UIEnabled);
		stream.Serialize("Command", &m_Command);
		stream.Serialize("CheckBox", &m_bCheckBox);
		stream.Serialize("CustomBBox", &m_bCustomBBox);
	}

	//! called during the initialization of the entity
	void Button::Init()
	{
		super::Init();

		m_Icon = static_cast<Sprite*>(GetChildByName("Icon"));

		// Init bounding box
		if(!m_bCustomBBox)
		{
			if(Entity2D* pBG = static_cast<Entity2D*>(GetChildByName("BG")))
				m_BoundingBox = pBG->GetBoundingBox() + pBG->GetPosition();
			else if(m_Icon.IsValid())		
				m_BoundingBox = m_Icon->GetBoundingBox() + m_Icon->GetPosition();	
		}
	}

	//! called during the update of the entity
	void Button::Update()
	{
		if (!m_bVisible)
			return;

		if(InputManager::Instance()->IsTouchJustPressed(this))
		{
			m_vOriginalPosition = m_vPosition;
			if (IsTouched())
			{
				if (m_UIEnabled)
					SetPressed(true);
				InputManager::Instance()->RequestTouchFocus(this);
			}
		}
		else if (InputManager::Instance()->IsTouchPressed(this))
		{
			if(m_bPressed && !IsTouched())
				SetPressed(false);
		}
		else if (InputManager::Instance()->IsTouchJustReleased(this))
		{
			if(!m_bPressed)
				return;
			
			if(m_bCheckBox)
				SetChecked(!m_bChecked);

			SetPressed(false);
			SendUIEvent();
		}
	}

	//! returns true if the button is touched
	bool Button::IsTouched() const
	{
		if(auto layout = GetComponent<LayoutComponent>())
		{
			auto _this = const_cast<Button*>(this);
			auto touchPos = InputManager::Instance()->GetRawTouchState().vPosition;
			auto transform = layout->Align(_this, m_vOriginalPosition);
			auto invTransform = Matrix44::Identity;
			transform.GetInverse(invTransform);
			auto invTouchPos = invTransform.TransformVect(Vector3::Create(touchPos, 0.0f));
			return GetBoundingBox().Contains(invTouchPos.XY);
		}

		Matrix44 transformation = Matrix44::Identity;
		transformation.Translate(-Vector3::Create(m_vCenter.X, m_vCenter.Y, 0.0f));
		transformation.Scale(Vector3::Create(m_vScale.X, m_vScale.Y, 1.0f));
		transformation.Rotate(Vector3::Create(0.0f, 0.0f, m_fRotation*Math::DegToRadFactor));		
		transformation.Translate(Vector3::Create(m_vOriginalPosition.X, m_vOriginalPosition.Y, 0.0f));
		if(Entity2D* p2DEntity = GetAncestor<Entity2D>())
			transformation *= p2DEntity->GetWorldTransform();

		Matrix44 inverse;
		if(transformation.GetInverse(inverse))
		{
			Vector2 vTouchPos = InputManager::Instance()->GetTouchState().vPosition;
			Vector3 invTouchPos3D = inverse.TransformVect(Vector3::Create(vTouchPos.X, vTouchPos.Y, 0.0f));
			Vector2 vInvTouchPos = Vector2::Create(invTouchPos3D.X, invTouchPos3D.Y);
			return GetBoundingBox().Contains(vInvTouchPos);
		}

		return false;
	}

	//! sends the UI event
	void Button::SendUIEvent()
	{
		UIEvent* pEvent = snew UIEvent();
		pEvent->m_Sender = this;
		pEvent->m_Command = m_Command;
		pEvent->m_bChecked = m_bChecked;
		EventManager::Instance()->SendEvent(pEvent);
	}

	//! changes pressed status
	void Button::SetPressed(bool bPressed)
	{
		if(bPressed)
			m_vPosition = m_vOriginalPosition + Vector2::Create(5.0f, 5.0f);
		else
			m_vPosition = m_vOriginalPosition;
		m_bLocalTransformationMatrixDirty = true;
		m_bPressed = bPressed;
	}

	//! sets checked status
	void Button::SetChecked(bool bChecked)
	{
		if(m_Icon.IsValid())
			m_Icon->SetCurrentFrame(bChecked ? 0 : 1);

		m_bChecked = bChecked;
	}
}

