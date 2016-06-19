/* 

Amine Rehioui
Created: July 22nd 2013

*/

#pragma once

#include "Mutex.h"

namespace shoot
{
	//! C++ 11 mutex class
	class MutexC11 : public Mutex
	{
	public:

		//! constructor
		MutexC11();

		//! destructor
		~MutexC11();

		//! Attempts to lock the mutex
		void Lock();

		//! Unlocks the mutex
		void Unlock();

	private:

		void* m_pMutex;
	};	
}

