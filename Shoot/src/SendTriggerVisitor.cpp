/* 

Amine Rehioui
Created: July 14th 2012

*/

#include "Shoot.h"	

#include "SendTriggerVisitor.h"

#include "EventManager.h"

namespace shoot
{
	DEFINE_OBJECT(SendTriggerVisitor);

	//! Reads/Writes struct properties from/to a stream
	void SendTriggerVisitor::Serialize(PropertyStream& stream)
	{
		super::Serialize(stream);
		stream.Serialize("TriggerName", &m_TriggerName);
	}

	//! visits a particular entity
	void SendTriggerVisitor::Visit(Entity* pTarget)
	{
		TriggerEvent* pEvent = snew TriggerEvent();
		pEvent->Target = pTarget;
		pEvent->Name = m_TriggerName;
		EventManager::Instance()->SendEvent(pEvent);

		super::Visit(pTarget);

		Leave();
	}
}


