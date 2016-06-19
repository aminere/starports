/* 

Amine Rehioui
Created: November 27th 2011

*/

#pragma once


#include "Thread.h"

namespace shoot
{
	//! ThreadSDL class
	class ThreadSDL : public Thread
	{
	public:

		//! constructor
		ThreadSDL();

		//! starts the thread
		void Start(ThreadFunc threadFunc, void* pParam = 0);

	private:

		void* m_pThread;
	};
}



