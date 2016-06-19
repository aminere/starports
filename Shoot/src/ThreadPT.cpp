/* 

Amine Rehioui
Created: April 7th 2013

*/

#include "Shoot.h"

#include "ThreadPT.h"

#include <pthread.h>

namespace shoot
{
	//! constructor
	ThreadPT::ThreadPT()
		: m_pThread(NULL)
	{
	}

	//! destructor
	ThreadPT::~ThreadPT()
	{
		if (auto thread = reinterpret_cast<pthread_t*>(m_pThread))
		{
			sdelete(thread);
			m_pThread = NULL;
		}
	}

	//! starts the thread
	void ThreadPT::Start(ThreadFunc threadFunc, void* pParam /*= 0*/)
	{
		m_pThread = snew pthread_t();
		pthread_create((pthread_t*)m_pThread, NULL, threadFunc, pParam);		
	}

	//! yields the thread execution
	void ThreadPT::YieldThread()
	{
		struct timespec t;
		t.tv_sec = 0;
		t.tv_nsec = 10;
		nanosleep(&t, NULL);
	}
}

