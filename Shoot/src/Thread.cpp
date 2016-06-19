/* 

Amine Rehioui
Created: November 27th 2011

*/

#include "Shoot.h"

#include "Thread.h"

#include "ThreadSDL.h"
#include "ThreadPT.h"
#include "ThreadC11.h"

namespace shoot
{
	//! creates a thread depending on the platform
	Thread* Thread::CreateThread()
	{
#if SHOOT_PLATFORM == SHOOT_PLATFORM_WP8
		return snew ThreadC11();
#elif SHOOT_PLATFORM == SHOOT_PLATFORM_IOS || SHOOT_PLATFORM == SHOOT_PLATFORM_ANDROID
		return snew ThreadPT();
#else
		return snew ThreadSDL();
#endif
	}

	//! yields a thread execution
	void Thread::YieldThread()
	{
#if SHOOT_PLATFORM == SHOOT_PLATFORM_WP8
		ThreadC11::YieldThread();
#elif SHOOT_PLATFORM == SHOOT_PLATFORM_IOS || SHOOT_PLATFORM == SHOOT_PLATFORM_ANDROID
		ThreadPT::YieldThread();
#else
		Sleep(1);
#endif
	}

	//! constructor
	Thread::Thread()
	{
	}

	//! destructor
	Thread::~Thread()
	{
	}
}

