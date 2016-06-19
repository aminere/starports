/* 

Amine Rehioui
Created: May 19th 2014

*/

#pragma once

#include "Gamepad.h"

#include <XInput.h>

namespace shoot
{
	//! GamepadDX11 
	class GamepadDX11 : public GamepadProvider
	{
		DECLARE_OBJECT(GamepadDX11, GamepadProvider);

	public:
	
		//! update the provider
		void Update();
		
	private:
		
		XINPUT_STATE m_StateDX11;
	};
}

