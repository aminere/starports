/* 

Amine Rehioui
Created: April 7th 2013

*/

#include "Shoot.h"

#include "SemaphoreSDL.h"

#include "SDL_mutex.h"

namespace shoot
{
	//! constructor
	SemaphoreSDL::SemaphoreSDL(int value)
	{
		m_pSemaphore = SDL_CreateSemaphore(1);		
	}

	//! destructor
	SemaphoreSDL::~SemaphoreSDL()
	{
		SDL_DestroySemaphore((SDL_sem*)m_pSemaphore);
	}

	//! Decrement the semaphore and block the thread if necessary
	void SemaphoreSDL::Wait()
	{
		SDL_SemWait((SDL_sem*)m_pSemaphore);
	}

	//! Increment the semaphore and unlock waiting threads if necessary
	void SemaphoreSDL::Post()
	{
		SDL_SemPost((SDL_sem*)m_pSemaphore); 
	}
}

