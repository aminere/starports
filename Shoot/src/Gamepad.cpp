/* 

Amine Rehioui
Created: May 19th 2014

*/

#include "Shoot.h"

#include "Gamepad.h"

#ifndef SHOOT_NO_GAMEPAD
	#if SHOOT_PLATFORM == SHOOT_PLATFORM_WIN32
		#include "GamepadDX11.h"
	#endif
#endif // SHOOT_NO_GAMEPAD

namespace shoot
{
	DEFINE_OBJECT(GamepadProvider);

	//! constructor
	GamepadProvider::GamepadProvider()
		: m_bAvailable(false)
	{
		m_State.Reset();
	}

	//! Constructor
	Gamepad::Gamepad()
	{
#ifndef SHOOT_NO_GAMEPAD
	#if SHOOT_PLATFORM == SHOOT_PLATFORM_WIN32
		m_Provider = snew GamepadDX11();
		return;
	#endif
#endif // SHOOT_NO_GAMEPAD

		m_Provider = new GamepadProvider();
	}

	//! Destructor
	Gamepad::~Gamepad()
	{		
	}
}

