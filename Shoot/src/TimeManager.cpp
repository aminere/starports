/* 

Amine Rehioui
Created: August 7th 2011

*/

#include "Shoot.h"

#include "TimeManager.h"
#include "Timer.h"
#include "Clock.h"

namespace shoot
{
	//! constructor
	TimeManager::TimeManager()
	{
	}

	//! Destructor
	TimeManager::~TimeManager()
	{
		SHOOT_WARNING(m_aTimers.empty(), "Destroying TimeManager while some Timers are still registered");
		SHOOT_WARNING(m_aClocks.empty(), "Destroying TimeManager while some Clocks are still registered");
	}

	//! registers a timer
	void TimeManager::RegisterTimer(Timer* pTimer)
	{
		std::vector<Timer*>::iterator it = std::find(m_aTimers.begin(), m_aTimers.end(), pTimer);
		SHOOT_ASSERT(it == m_aTimers.end(), "Calling RegisterTimer twice with the same timer");
		m_aTimers.push_back(pTimer);
	}

	//! unregisters a timer
	void TimeManager::UnregisterTimer(Timer* pTimer)
	{
		std::vector<Timer*>::iterator it = std::find(m_aTimers.begin(), m_aTimers.end(), pTimer);
		SHOOT_ASSERT(it != m_aTimers.end(), "TimeManager::UnregisterTimer: Trying to remove unexisting timer");		
		m_aTimers.erase(it);
	}

	//! registers a clock
	void TimeManager::RegisterClock(Clock* pClock)
	{
		std::vector<Clock*>::iterator it = std::find(m_aClocks.begin(), m_aClocks.end(), pClock);
		SHOOT_ASSERT(it == m_aClocks.end(), "Calling RegisterClock twice with the same Clock");
		m_aClocks.push_back(pClock);
	}

	//! unregisters a clock
	void TimeManager::UnregisterClock(Clock* pClock)
	{
		std::vector<Clock*>::iterator it = std::find(m_aClocks.begin(), m_aClocks.end(), pClock);
		SHOOT_ASSERT(it != m_aClocks.end(), "ClockManager::UnregisterClock: Trying to remove unexisting Clock");		
		m_aClocks.erase(it);
	}

	//! updates the timers
	void TimeManager::Update()
	{
		for (size_t i=0; i<m_aTimers.size(); ++i)
		{
			m_aTimers[i]->Advance(g_fDeltaTime);
		}

		for (size_t i=0; i<m_aClocks.size(); ++i)
		{
			m_aClocks[i]->Advance(g_fDeltaTime);
		}
	}
}
