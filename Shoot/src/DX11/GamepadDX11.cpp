/* 

Amine Rehioui
Created: May 19th 2014

*/

#include "Shoot.h"

#include "GamepadDX11.h"

namespace shoot
{
	DEFINE_OBJECT(GamepadDX11);

	//! update the provider
	void GamepadDX11::Update()
	{
		memset(&m_StateDX11, 0, sizeof(XINPUT_STATE));
		m_bAvailable = (XInputGetState(0, &m_StateDX11) == ERROR_SUCCESS);
		if(!m_bAvailable)
		{
			return;
		}

		m_State.Reset();

		// Convert from [0, 255] to [0, 1]
		if(m_StateDX11.Gamepad.bRightTrigger && m_StateDX11.Gamepad.bRightTrigger < XINPUT_GAMEPAD_TRIGGER_THRESHOLD)
		{
			m_State.m_fRightTrigger = m_StateDX11.Gamepad.bRightTrigger/255.0f;
		}

		if(m_StateDX11.Gamepad.bLeftTrigger && m_StateDX11.Gamepad.bLeftTrigger < XINPUT_GAMEPAD_TRIGGER_THRESHOLD)
		{
			m_State.m_fLeftTrigger = m_StateDX11.Gamepad.bLeftTrigger/255.0f;
		}

		if(m_StateDX11.Gamepad.wButtons & XINPUT_GAMEPAD_A) m_State.m_bButtons[GB_A] = true;
		if(m_StateDX11.Gamepad.wButtons & XINPUT_GAMEPAD_B) m_State.m_bButtons[GB_B] = true;
		if(m_StateDX11.Gamepad.wButtons & XINPUT_GAMEPAD_X) m_State.m_bButtons[GB_X] = true;
		if(m_StateDX11.Gamepad.wButtons & XINPUT_GAMEPAD_Y) m_State.m_bButtons[GB_Y] = true;
		if(m_StateDX11.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) m_State.m_bButtons[GB_LEFT_SHOULDER] = true;
		if(m_StateDX11.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) m_State.m_bButtons[GB_RIGHT_SHOULDER] = true;
		if(m_StateDX11.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP) m_State.m_bButtons[GB_DPAD_UP] = true;
		if(m_StateDX11.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN) m_State.m_bButtons[GB_DPAD_DOWN] = true;
		if(m_StateDX11.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT) m_State.m_bButtons[GB_DPAD_LEFT] = true;
		if(m_StateDX11.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) m_State.m_bButtons[GB_DPAD_RIGHT] = true;
		if(m_StateDX11.Gamepad.wButtons & XINPUT_GAMEPAD_START) m_State.m_bButtons[GB_START] = true;
		if(m_StateDX11.Gamepad.wButtons & XINPUT_GAMEPAD_BACK) m_State.m_bButtons[GB_BACK] = true;
		if(m_StateDX11.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB) m_State.m_bButtons[GB_LEFT_THUMB] = true;
		if(m_StateDX11.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB) m_State.m_bButtons[GB_RIGHT_THUMB] = true;
		if(m_StateDX11.Gamepad.bLeftTrigger) m_State.m_bButtons[GB_LEFT_TRIGGER] = true;
		if(m_StateDX11.Gamepad.bRightTrigger) m_State.m_bButtons[GB_RIGHT_TRIGGER] = true;
		
		

		if(m_StateDX11.Gamepad.sThumbLX > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE 
		|| m_StateDX11.Gamepad.sThumbLX < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE
		|| m_StateDX11.Gamepad.sThumbLY > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE 
		|| m_StateDX11.Gamepad.sThumbLY < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
		{
			m_State.m_vLeftStick.X = (static_cast<float>(m_StateDX11.Gamepad.sThumbLX) / 32768.0f);
			m_State.m_vLeftStick.Y = (static_cast<float>(m_StateDX11.Gamepad.sThumbLY) / 32768.0f);
		}

		if(m_StateDX11.Gamepad.sThumbRX > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE 
		|| m_StateDX11.Gamepad.sThumbRX < -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE
		|| m_StateDX11.Gamepad.sThumbRY > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE 
		|| m_StateDX11.Gamepad.sThumbRY < -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)
		{
			m_State.m_vRightStick.X = (static_cast<float>(m_StateDX11.Gamepad.sThumbRX) / 32768.0f);
			m_State.m_vRightStick.Y = (static_cast<float>(m_StateDX11.Gamepad.sThumbRY) / 32768.0f);
		}
	}
}

