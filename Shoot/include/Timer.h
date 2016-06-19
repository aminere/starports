/* 

Amine Rehioui
Created: August 7th 2011

*/

#pragma once


namespace shoot
{
	// forwards
	class TimeManager;

	//! Timer class
	class Timer
	{
	public:

		//! constructor
		Timer();

		//! destructor
		~Timer();

		//! starts the timer
		void Start(float fDuration);

		//! stops the timer
		void Stop();

		//! returns true if the timer is running
		inline bool IsRunning() const { return m_bRunning; }

		//! returns stopped status
		inline bool IsStopped() const { return !m_bRunning; }

		//! return the progress remaining
		inline float GetProgressRemaining() const { return m_fTimeLeft; }

		//! returns the total duration
		inline float GetDuration() const { return m_fDuration; }

		//! returns the progress ratio
		float GetProgressRatio() const;

		//! returns the progress duration
		float GetProgressDuration() const;

		//! advances the timer
		void Advance(float fDeltaTime);

	private:

		float m_fDuration;
		float m_fTimeLeft;
		bool m_bRunning;
	};
}



