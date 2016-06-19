/* 

Amine Rehioui
Created: April 7th 2013

*/

#pragma once


#include "Thread.h"

namespace shoot
{
	//! ThreadPT class
	class ThreadPT : public Thread
	{
	public:

		//! constructor
		ThreadPT();

		//! destructor
		virtual ~ThreadPT();

		//! starts the thread
		void Start(ThreadFunc threadFunc, void* pParam = 0);

		//! yields the thread execution
		static void YieldThread();

	private:

		void* m_pThread;
	};
}



