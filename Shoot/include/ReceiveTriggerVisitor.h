/* 

Amine Rehioui
Created: July 14th 2012

*/

#pragma once


#include "Visitor.h"

namespace shoot
{
	// forwards
	class Event;

	//! ReceiveTriggerVisitor class
	class ReceiveTriggerVisitor : public Visitor
	{
	public:

		DECLARE_OBJECT(ReceiveTriggerVisitor, Visitor);

		//! Reads/Writes struct properties from/to a stream
		void Serialize(PropertyStream& stream);

		//! visits a particular entity
		void Visit(Entity* pTarget);

		//! Leaves the visited entity
		void Leave();

		//! updates the visitor		
		void Update();

		//! shoot event listeners
		void HandleTriggerEvent(Event* pEvent);

	private:

		// properties
		std::string m_TriggerName;
		Reference<Visitor> m_VisitorToTrigger;
	};	
}



