/* 

Amine Rehioui
Created: April 7th 2013

*/

#include "Shoot.h"

#include "SSemaphore.h"

#include "SemaphoreSDL.h"
#include "SemaphorePT.h"

namespace shoot
{
	//! constructor
	Semaphore* Semaphore::Create(int value)
	{
#if SHOOT_PLATFORM == SHOOT_PLATFORM_IOS \
 || SHOOT_PLATFORM == SHOOT_PLATFORM_ANDROID
		return new SemaphorePT(value);
#else
		return new SemaphoreSDL(value);
#endif
	}
}

