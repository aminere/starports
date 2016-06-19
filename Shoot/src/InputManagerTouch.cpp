/* 

Amine Rehioui
Created:  June 21st 2013

*/

#include "Shoot.h"

#include "InputManagerTouch.h"

namespace shoot
{
	//! Constructor
	InputManagerTouch::InputManagerTouch() 
	{
		memset(m_aNextKeyStates, 0, KT_Count*sizeof(bool));
		memset(m_aCurrentKeyStates, 0, KT_Count*sizeof(bool));
		memset(m_aPreviousKeyStates, 0, KT_Count*sizeof(bool));
	}

	//! Destructor
	InputManagerTouch::~InputManagerTouch()
	{		
	}

	//! Update the input state
	void InputManagerTouch::Update()
	{
		m_PreviousTouchState = m_CurrentTouchState;
		m_CurrentTouchState = m_NextTouchState;
		m_CurrentRawTouchState = m_NextRawTouchState;

		memcpy(m_aPreviousKeyStates, m_aCurrentKeyStates, KT_Count*sizeof(bool));
		memcpy(m_aCurrentKeyStates, m_aNextKeyStates, KT_Count*sizeof(bool));
		
		super::Update();
	}

	//! returns true only during the frame in which the key moved from unpressed to pressed state
	bool InputManagerTouch::IsKeyJustPressed(E_KeyType eKey) const
	{
		return m_aCurrentKeyStates[eKey] && !m_aPreviousKeyStates[eKey];
	}

	//! returns true for as long as the key is pressed
	bool InputManagerTouch::IsKeyPressed(E_KeyType eKey) const
	{
		return m_aCurrentKeyStates[eKey];
	}

	//! returns true only during the frame in which the key moved from pressed to unpressed state
	bool InputManagerTouch::IsKeyJustReleased(E_KeyType eKey) const
	{
		return !m_aCurrentKeyStates[eKey] && m_aPreviousKeyStates[eKey];
	}

	//! returns true only during the frame in which the touch moved from unpressed to pressed state
	bool InputManagerTouch::IsTouchJustPressed(Object* requester /*= NULL*/) const
	{
		if (m_FocusHolder.IsValid() && m_FocusHolder.Get() != requester)
			return false;

		return m_CurrentTouchState.bPressed && !m_PreviousTouchState.bPressed;
	}

	//! returns true for as long as the touch is pressed
	bool InputManagerTouch::IsTouchPressed(Object* requester /*= NULL*/) const
	{
		if (m_FocusHolder.IsValid() && m_FocusHolder.Get() != requester)
			return false;

		return m_CurrentTouchState.bPressed;
	}

	//! returns true only during the frame in which the touch moved from pressed to unpressed state
	bool InputManagerTouch::IsTouchJustReleased(Object* requester /*= NULL*/) const
	{
		if (m_FocusHolder.IsValid() && m_FocusHolder.Get() != requester)
			return false;

		return !m_CurrentTouchState.bPressed && m_PreviousTouchState.bPressed;
	}
	
	//! sets the touch state
	void InputManagerTouch::SetTouchState(const TouchState& state)
	{
		m_NextTouchState = m_NextRawTouchState = state;

		// take camera transformation into account
		if(Camera* p2DCamera = EntityRenderer::Instance()->Get2DCamera())
		{
			Vector3 vCameraTranslation = p2DCamera->GetViewMatrix().GetTranslation();
			Vector3 vCameraScale = p2DCamera->GetViewMatrix().GetScale();
			m_NextTouchState.vPosition -= Vector2::Create(vCameraTranslation.X, vCameraTranslation.Y);
			m_NextTouchState.vPosition.Set(m_NextTouchState.vPosition.X/vCameraScale.X, m_NextTouchState.vPosition.Y/vCameraScale.Y);
		}
	}

	//! force key status
	void InputManagerTouch::SetKeyPressed(E_KeyType eKey, bool bSet)
	{
		m_aNextKeyStates[eKey] = bSet;
	}
}

