/* 

Amine Rehioui
Created: July 23rd 2011

*/

#pragma once


namespace shoot
{
	//! Event class
	class Event : public Object
	{
		DECLARE_OBJECT(Event, Object);
	public:

		virtual ~Event()
		{
		}
	};

	//! trigger event
	class TriggerEvent : public Event
	{
		DECLARE_OBJECT(TriggerEvent, Event);
	public:

		Handle<Object> Target;
		std::string Name;
	};

	//! UI event
	class UIEvent : public Event
	{
		DECLARE_OBJECT(UIEvent, Event);
	public:

		Handle<Object> m_Sender;
		std::string m_Command;
		bool m_bChecked;
	};

	//! WheelEvent
	class WheelEvent : public Event
	{
		DECLARE_OBJECT(WheelEvent, Event);
	public:
		bool m_Up;
	};
}



