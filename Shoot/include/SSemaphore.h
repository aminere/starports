/* 

Amine Rehioui
Created: April 7th 2013

*/

#pragma once


namespace shoot
{
	//! Semaphore class
	class Semaphore
	{
	public:

		//! constructor
		static Semaphore* Create(int value);

		//! destructor
		virtual ~Semaphore()
		{
		}

		//! Decrement the semaphore and block the thread if necessary
		virtual void Wait() = 0;

		//! Increment the semaphore and unlock waiting threads if necessary
		virtual void Post() = 0;
	};
}



