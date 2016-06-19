/* 

Amine Rehioui
Created: April 7th 2013

*/

#pragma once


#include "SSemaphore.h"

namespace shoot
{
	//! SemaphorePT class
	class SemaphorePT : public Semaphore
	{
	public:

		//! constructor
		SemaphorePT(int value);

		//! destructor
		~SemaphorePT();

		//! Decrement the semaphore and block the thread if necessary
		void Wait();

		//! Increment the semaphore and unlock waiting threads if necessary
		void Post();

	private:

		void* m_pSemaphore;
	};	
}



