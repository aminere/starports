/* 

Amine Rehioui
Created: April 21st 2012

*/

#include "Shoot.h"

#include "MutexSDL.h"

#include "SDL_mutex.h"

namespace shoot
{
	//! constructor
	MutexSDL::MutexSDL()
	{
		m_pMutex = SDL_CreateMutex();
	}

	//! destructor
	MutexSDL::~MutexSDL()
	{
		SDL_DestroyMutex((SDL_mutex*)m_pMutex);
	}

	//! Attempts to lock the mutex
	void MutexSDL::Lock()
	{
		SDL_LockMutex((SDL_mutex*)m_pMutex);
	}

	//! Unlocks the mutex
	void MutexSDL::Unlock()
	{
		SDL_UnlockMutex((SDL_mutex*)m_pMutex);
	}
}

