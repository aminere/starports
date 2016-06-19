/* 

Amine Rehioui
Created: July 22nd 2013

*/

#include "Shoot.h"

#include "MutexC11.h"

#include <mutex>

namespace shoot
{
	//! constructor
	MutexC11::MutexC11()
	{
		m_pMutex = new std::mutex();
	}

	//! destructor
	MutexC11::~MutexC11()
	{
		std::mutex* pMutex = static_cast<std::mutex*>(m_pMutex);
		delete pMutex;
	}

	//! Attempts to lock the mutex
	void MutexC11::Lock()
	{
		static_cast<std::mutex*>(m_pMutex)->lock();
	}

	//! Unlocks the mutex
	void MutexC11::Unlock()
	{
		static_cast<std::mutex*>(m_pMutex)->unlock();
	}
}

