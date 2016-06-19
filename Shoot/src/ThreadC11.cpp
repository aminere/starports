/* 

Amine Rehioui
Created: July 25th 2013

*/

#include "Shoot.h"

#include "ThreadC11.h"

#include <thread>

namespace shoot
{
	//! constructor
	ThreadC11::ThreadC11()
		: m_pThread(NULL)
	{
	}

	//! destructor
	ThreadC11::~ThreadC11()
	{
		std::thread* pThread = static_cast<std::thread*>(m_pThread);
		pThread->join(); // wait for thread to finish before deleting
		delete pThread;
	}

	//! starts the thread
	void ThreadC11::Start(ThreadFunc threadFunc, void* pParam /*= 0*/)
	{
		m_pThread = new std::thread(threadFunc, pParam);
	}

	//! yields the thread execution
	void ThreadC11::YieldThread()
	{
		std::this_thread::yield();
	}
}

