/* 

Amine Rehioui
Created: May 18th 2012

*/

#include "Shoot.h"

#include "HighResTimer.h"

#if SHOOT_PLATFORM == SHOOT_PLATFORM_IOS
#include <sys/time.h>
#endif

namespace shoot
{

#ifdef _WIN32
	double HighResTimer::m_Frequency = HighResTimer::GetFrequency();

	//! calculates timer frequency
	double HighResTimer::GetFrequency()
	{
		LARGE_INTEGER li;
		QueryPerformanceFrequency(&li);
		return double(li.QuadPart)/1000000.0;
	}
#endif

	//! stats the timer
	void HighResTimer::Start()
	{
#ifdef _WIN32
		LARGE_INTEGER li;		
		QueryPerformanceCounter(&li);
		m_TimeStart = li.QuadPart;
#elif SHOOT_PLATFORM == SHOOT_PLATFORM_IOS        
        gettimeofday(&m_TimeSpec, NULL);        
#else
		clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &m_TimeSpec);
#endif 
	}

	//! stops the timer and returns the number of micro seconds
	int HighResTimer::GetDuration()
	{
#ifdef _WIN32
		LARGE_INTEGER li;
		QueryPerformanceCounter(&li);
		return int(double(li.QuadPart-m_TimeStart)/m_Frequency);
#elif SHOOT_PLATFORM == SHOOT_PLATFORM_IOS
        struct timeval current;
        gettimeofday(&current, NULL);
        long micros = current.tv_usec - m_TimeSpec.tv_usec;
		return micros;
#else
		timespec current;
		clock_gettime(CLOCK_REALTIME, &current);
		long micros = (current.tv_nsec - m_TimeSpec.tv_nsec)/1000;
		return micros;
#endif		
	}	
}


