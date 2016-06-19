/* 

Amine Rehioui
Created: April 21st 2012

*/

#pragma once


#include "Mutex.h"

namespace shoot
{
	//! mutex class
	class MutexSDL : public Mutex
	{
	public:

		//! constructor
		MutexSDL();

		//! destructor
		~MutexSDL();

		//! Attempts to lock the mutex
		void Lock();

		//! Unlocks the mutex
		void Unlock();

	private:

		void* m_pMutex;
	};	
}

