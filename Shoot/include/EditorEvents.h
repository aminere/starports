/* 

Amine Rehioui
Created: August 7th 2011

*/

#pragma once


#include "Event.h"

namespace shoot
{
	//! EditorEvents class
	class EditorEvents
	{
	public:

		//! registers engine events
		static void Register();

		//! sets a PathElement camera info
		class SetCameraPathElementInfoEvent : public Event
		{
		public:
			DECLARE_OBJECT(SetCameraPathElementInfoEvent, Event);
		};
	};
}



