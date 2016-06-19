/* 

Amine Rehioui
Created: May 18th 2012

*/

#pragma once


namespace shoot
{
	//! HighResTimer
	class HighResTimer
	{
	public:

		//! stats the timer
		void Start();

		//! return the duration in micro seconds
		int GetDuration();

	private:

#ifdef _WIN32
		static double m_Frequency;
		__int64 m_TimeStart;

		//! calculates timer frequency
		static double GetFrequency();
#elif SHOOT_PLATFORM == SHOOT_PLATFORM_IOS
        struct timeval m_TimeSpec;
#else
		timespec m_TimeSpec;
#endif
	};
}



