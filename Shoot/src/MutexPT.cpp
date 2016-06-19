/* 

Amine Rehioui
Created: April 21st 2012

*/

#include "Shoot.h"

#include "MutexPT.h"

#include <pthread.h>

namespace shoot
{
	//! constructor
	MutexPT::MutexPT()
	{
		m_pMutex = new pthread_mutex_t();
		pthread_mutex_init((pthread_mutex_t*)m_pMutex, 0); // TODO examine 2nd param
	}

	//! destructor
	MutexPT::~MutexPT()
	{
		pthread_mutex_destroy((pthread_mutex_t*)m_pMutex);
		delete (pthread_mutex_t*)m_pMutex;
	}

	//! Attempts to lock the mutex
	void MutexPT::Lock()
	{
		pthread_mutex_lock((pthread_mutex_t*)m_pMutex);
	}

	//! Unlocks the mutex
	void MutexPT::Unlock()
	{
		pthread_mutex_unlock((pthread_mutex_t*)m_pMutex);
	}
}

