/* 

Amine Rehioui
Created: April 7th 2013

*/

#include "Shoot.h"

#include "SemaphorePT.h"

#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdio.h>

namespace shoot
{
	//! constructor
	SemaphorePT::SemaphorePT(int value)
	{
		char name[16];
		sprintf(name, "%x", (size_t)this);
		m_pSemaphore = sem_open(name, O_CREAT, S_IRUSR|S_IWUSR, value);
	}

	//! destructor
	SemaphorePT::~SemaphorePT()
	{
		sem_close((sem_t*)m_pSemaphore);
		m_pSemaphore = NULL;
	}

	//! Decrement the semaphore and block the thread if necessary
	void SemaphorePT::Wait()
	{
		sem_wait((sem_t*)m_pSemaphore);
	}

	//! Increment the semaphore and unlock waiting threads if necessary
	void SemaphorePT::Post()
	{
		sem_post((sem_t*)m_pSemaphore);
	}
}

