/* 

Amine Rehioui
Created: December 2nd 2011

*/

#include "Shoot.h"

#include "Clock.h"
#include "TimeManager.h"

namespace shoot
{
	//! constructor
	Clock::Clock()
		: m_fTime(0.0f)
		, m_bStarted(false)
	{
		TimeManager::Instance()->RegisterClock(this);
	}

	//! destructor
	Clock::~Clock()
	{
		TimeManager::Instance()->UnregisterClock(this);
	}

	//! starts the timer
	void Clock::Start()
	{
		m_fTime = 0.0f;
		m_bStarted = true;
	}

	//! stops the timer
	void Clock::Stop()
	{
		m_bStarted = false;
	}

	//! advances the timer
	void Clock::Advance(float fDeltaTime)
	{
		if(m_bStarted)
		{
			m_fTime += fDeltaTime;
		}
	}
}
