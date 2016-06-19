/* 

Amine Rehioui
Created: July 23rd 2011

*/

#include "Shoot.h"

#include "EventManager.h"

namespace shoot
{
	//! Constructor
	EventManager::EventManager()
	{
	}

	//! Destructor
	EventManager::~EventManager()
	{
		SHOOT_ASSERT(m_aListeners.size() == 0, "Destroying the EventManager while some listeners are still registered");
	}

	//! Sends an event
	void EventManager::SendEvent(Event* pEvent)
	{
		int numHandlers = 0;
		for (size_t i=0; i<m_aListeners.size(); ++i)
		{
			ListenerInfo* pListenerInfo = m_aListeners[i];
			if(pListenerInfo->strEventName == pEvent->GetClassName())
			{
				PendingEventInfo info = { pListenerInfo, Reference<Event>(pEvent) };
				m_aPendingEventInfo.push_back(info);
				numHandlers++;
			}
		}

		if(numHandlers == 0)
		{
			Log << "Event " << pEvent->GetClassName() << " has no registered listeners and will not be handled\n";
			sdelete(pEvent);
		}
	}

	//! SendEvent
	void EventManager::SendEvent(std::function<void()>& func, float delay /*= -1.0f*/)
	{
		PendingFuncInfo info;
		info.m_Func = func;
		info.m_Timer = delay;
		m_aPendingFuncInfo.push_back(info);
	}

	//! Update events
	void EventManager::Update()
	{
		if(m_aPendingEventInfo.size() > 0)
		{
			for (size_t i=0; i<m_aPendingEventInfo.size(); ++i)
			{
				PendingEventInfo& pendingInfo = m_aPendingEventInfo[i];
				ListenerInfo* pListenerInfo = pendingInfo.pListenerInfo;
				(pListenerInfo->pListener->*pListenerInfo->handler)(pendingInfo.event);
			}

			m_aPendingEventInfo.clear();
		}

		for (auto it = m_aPendingFuncInfo.begin(); it != m_aPendingFuncInfo.end(); )
		{
			auto& info = (*it);
			if (info.m_Timer < 0.0f)
			{
				info.m_Func();
				it = m_aPendingFuncInfo.erase(it);
			}
			else
			{
				info.m_Timer -= g_fDeltaTime;
				++it;
			}
		}
	}

	//! Finds a listener index
	int EventManager::GetListenerIndex(const char* strEventName, Object* pListener)
	{
		for (size_t i=0; i<m_aListeners.size(); ++i)
		{
			ListenerInfo* pInfo = m_aListeners[i];
			if(pInfo->strEventName == strEventName && pInfo->pListener == pListener)
				return (int)i;
		}
		return -1;
	}

	//! returns true if a listener is registered
	bool EventManager::IsRegistered(Object *pListener)
	{
		for (size_t i=0; i<m_aListeners.size(); ++i)
		{
			if(m_aListeners[i]->pListener == pListener)
				return true;
		}
		return false;
	}	
}
