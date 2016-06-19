/* 

Amine Rehioui
Created: June 21st 2013

*/

#pragma once


#include "InputManager.h"

namespace shoot
{
	//! Interface to input devices on iOS
	class InputManagerTouch : public InputManager
	{
		typedef InputManager super;

	public:
	
		//! constructor
		InputManagerTouch();

		//! destructor
		~InputManagerTouch();

		//! Update the input state
		void Update();	

		//! returns true only during the frame in which the key moved from unpressed to pressed state
		bool IsKeyJustPressed(E_KeyType eKey) const;

		//! returns true for as long as the key is pressed
		bool IsKeyPressed(E_KeyType eKey) const;

		//! returns true only during the frame in which the key moved from pressed to unpressed state
		bool IsKeyJustReleased(E_KeyType eKey) const;

		//! returns true only during the frame in which the touch moved from unpressed to pressed state
		bool IsTouchJustPressed(Object* requester = NULL) const override;

		//! returns true for as long as the touch is pressed
		bool IsTouchPressed(Object* requester = NULL) const override;

		//! returns true only during the frame in which the touch moved from pressed to unpressed state
		bool IsTouchJustReleased(Object* requester = NULL) const override;

		//! returns the touch state
		const TouchState& GetTouchState() const { return m_CurrentTouchState; }

		//! returns the raw touch state - does not take 2D camera transformation into account
		const TouchState& GetRawTouchState() const { return m_CurrentRawTouchState; }
	
		//! sets the touch state
		void SetTouchState(const TouchState& state);

		//! force key status
		void SetKeyPressed(E_KeyType eKey, bool bSet);

	private:

		bool m_aNextKeyStates[KT_Count];
		bool m_aCurrentKeyStates[KT_Count];
		bool m_aPreviousKeyStates[KT_Count];
		
		TouchState m_NextTouchState;
		TouchState m_CurrentTouchState;
		TouchState m_PreviousTouchState;

		TouchState m_NextRawTouchState;
		TouchState m_CurrentRawTouchState;
	};
}


