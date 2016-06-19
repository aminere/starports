/* 

Amine Rehioui
Created: April 7th 2013

*/

#pragma once


#include "SSemaphore.h"

namespace shoot
{
	//! SemaphoreSDL class
	class SemaphoreSDL : public Semaphore
	{
	public:

		//! constructor
		SemaphoreSDL(int value);

		//! destructor
		~SemaphoreSDL();

		//! Decrement the semaphore and block the thread if necessary
		void Wait();

		//! Increment the semaphore and unlock waiting threads if necessary
		void Post();

	private:

		void* m_pSemaphore;
	};	
}



