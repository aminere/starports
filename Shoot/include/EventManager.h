/* 

Amine Rehioui
Created: July 23rd 2011

*/

#pragma once


#include "Event.h"

namespace shoot
{
	//! Event manager class
	class EventManager : public Singleton<EventManager>
	{
		DECLARE_SINGLETON(EventManager);

	public:

		//! Event Handler signature
		typedef void (Object::*EventHandler)(Event* pEvent);

		//! Event Listener Info
		struct ListenerInfo
		{
			ListenerInfo(const std::string& _strEventName, Object* _pListener, EventHandler _handler)
				: strEventName(_strEventName)
				, pListener(_pListener)
				, handler(_handler)
			{
			}

			std::string strEventName;
			Object* pListener;
			EventHandler handler;
		};

		//! Register a listener
		template <class EventType>
		void RegisterListener(Object* pListener, EventHandler handler)
		{
			SHOOT_ASSERT(GetListenerIndex(EventType::GetStaticClassName(), pListener) == -1, "Trying to register the same listener twice for an event");
			ListenerInfo* pInfo = snew ListenerInfo(EventType::GetStaticClassName(), pListener, handler);			
			m_aListeners.push_back(pInfo);
		}

		//! Unregister a listener
		template <class EventType>
		void UnregisterListener(Object* pListener)
		{
			auto index = GetListenerIndex(EventType::GetStaticClassName(), pListener);
			SHOOT_ASSERT(index >= 0, "Trying to unregister an unexisting listener for an event");

			// make sure no pending events remain for this listener
			for(std::vector<PendingEventInfo>::iterator it = m_aPendingEventInfo.begin();
				it != m_aPendingEventInfo.end();)
			{
				if((*it).pListenerInfo->pListener == pListener)
					it = m_aPendingEventInfo.erase(it);
				else
					++it;
			}

			ListenerInfo* pInfo = *(m_aListeners.begin()+index);
			sdelete(pInfo);
			m_aListeners.erase(m_aListeners.begin()+index);
		}

		//! returns true if a listener is registered
		bool IsRegistered(Object *pListener);

		//! Sends an event
		void SendEvent(Event* pEvent);

		//! SendEvent
		void SendEvent(std::function<void()>& func, float delay = -1.0f);

		//! Update events
		void Update();		

	private:

		//! Finds a listener index
		int GetListenerIndex(const char* strEventName, Object* pListener);

		std::vector<ListenerInfo*> m_aListeners;

		//! pending event info
		struct PendingEventInfo
		{
			ListenerInfo* pListenerInfo;
			Reference<Event> event;
		};

		//! PendingFuncInfo
		struct PendingFuncInfo
		{
			std::function<void()> m_Func;
			float m_Timer;
		};

		std::vector<PendingEventInfo> m_aPendingEventInfo;
		std::vector<PendingFuncInfo> m_aPendingFuncInfo;
	};
}



