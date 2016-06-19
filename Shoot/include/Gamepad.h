/* 

Amine Rehioui
Created: May 19th 2014

*/

#pragma once


namespace shoot
{
	//! E_GamepadButton
	enum E_GamepadButton
	{
		GB_DPAD_UP,
		GB_DPAD_DOWN,
		GB_DPAD_LEFT,
		GB_DPAD_RIGHT,
		GB_START,
		GB_BACK,
		GB_LEFT_THUMB,
		GB_RIGHT_THUMB,
		GB_LEFT_SHOULDER,
		GB_RIGHT_SHOULDER,
		GB_A,
		GB_B,
		GB_X,
		GB_Y,
		GB_LEFT_TRIGGER,
		GB_RIGHT_TRIGGER,
		GB_Max
	};

	//! GamepadState
	struct GamepadState 
	{
		bool m_bButtons[GB_Max];
		Vector2 m_vLeftStick;
		Vector2	m_vRightStick;
		float	m_fLeftTrigger;
		float m_fRightTrigger;

		//! Reset
		void Reset()
		{
			memset(m_bButtons, 0, GB_Max*sizeof(bool));
			m_vLeftStick = m_vRightStick = Vector2::Zero;
			m_fLeftTrigger = m_fRightTrigger = 0.0f;
		}
	};

	//! GamepadProvider
	class GamepadProvider : public Object
	{
		DECLARE_OBJECT(GamepadProvider, Object);

	public:
		
		//! constructor
		GamepadProvider();

		//! destructor
		virtual ~GamepadProvider()
		{
		}

		//! returns true for as long as the key is pressed
		inline bool IsPressed(E_GamepadButton eButton) const { return m_State.m_bButtons[eButton]; }

		//! returns the state
		inline const GamepadState& GetState() const { return m_State; }

		//! returns the availability
		inline bool IsAvailable() const { return m_bAvailable; }

		//! update the provider
		virtual void Update() { }

	protected:

		GamepadState m_State;
		bool m_bAvailable;
	};

	//! Gamepad 
	class Gamepad : public Singleton<Gamepad>
	{
		DECLARE_SINGLETON(Gamepad);

	public:
		
		//! returns true for as long as the key is pressed
		inline bool IsPressed(E_GamepadButton eButton) const { return m_Provider->IsPressed(eButton); }

		//! returns the state
		inline const GamepadState& GetState() const { return m_Provider->GetState(); }

		//! Update the gamepad
		inline void Update() { m_Provider->Update(); }

		//! returns availability
		inline bool IsAvailable() const { return m_Provider->IsAvailable(); }
		
	private:

		Reference<GamepadProvider> m_Provider;
	};
}



