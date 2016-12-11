/* 

Amine Rehioui
Created: March 18th 2010

*/

#pragma once


#include "InputManager.h"

#include "SDL.h"

namespace shoot
{
	//! Interface to input devices, uses SDL 
	class InputManagerSDL : public InputManager
	{
		typedef InputManager super;

	public:
	
		//! constructor
		InputManagerSDL();

		//! destructor
		~InputManagerSDL();

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
		const TouchState& GetTouchState() const { return m_TouchState; }

		//! returns the raw touch state - does not take 2D camera transformation into account
		const TouchState& GetRawTouchState() const { return m_RawTouchState; }
	
		//! sets the touch state
		void SetTouchState(const TouchState& state);

		//! force key status
		void SetKeyPressed(E_KeyType eKey, bool bSet);

		//! returns shoot type for an SDL type
		inline E_KeyType GetShootKeyType(SDL_Keycode sdlKey) const 
		{
			auto it = m_aShootKeyTypes.find(sdlKey);
			SHOOT_ASSERT(it != m_aShootKeyTypes.end(), "Unsupported SDL Key");
			return it->second;
		}

	private:

		// converts an SDL key type to a shoot key type
		std::map<SDL_Keycode, E_KeyType> m_aShootKeyTypes;

		bool m_aKeyStates[KT_Count];
		bool m_aPreviousKeyStates[KT_Count];
		TouchState m_RawTouchState;
		TouchState m_TouchState;
		TouchState m_PreviousTouchState;
	};
}


