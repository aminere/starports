/* 

Amine Rehioui
Created: November 27th 2011

*/

#include "Shoot.h"

#include "ThreadSDL.h"

#include "SDL_Thread.h"

namespace shoot
{
	//! constructor
	ThreadSDL::ThreadSDL()
		: m_pThread(NULL)
	{
	}

	//! starts the thread
	void ThreadSDL::Start(ThreadFunc threadFunc, void* pParam /*= 0*/)
	{
		typedef int SDLThreadFunc(void* pParam);
		m_pThread = SDL_CreateThread((SDLThreadFunc*)threadFunc, pParam);
	}
}

