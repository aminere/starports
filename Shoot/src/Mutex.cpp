/* 

Amine Rehioui
Created: August 12th 2011

*/

#include "Shoot.h"

#include "Mutex.h"

#include "MutexSDL.h"
#include "MutexPT.h"
#include "MutexC11.h"

namespace shoot
{
	//! constructor
	Mutex* Mutex::Create()
	{
#if SHOOT_PLATFORM == SHOOT_PLATFORM_WP8
		return new MutexC11();
#elif SHOOT_PLATFORM == SHOOT_PLATFORM_IOS || SHOOT_PLATFORM == SHOOT_PLATFORM_ANDROID
		return new MutexPT();
#else
		return new MutexSDL();
#endif
	}
}

