/* 

Amine Rehioui
Created: July 23rd 2011

*/

#pragma once

#include "Event.h"

namespace shoot
{
	//! EditorEvents handler class
	class ShootEditorEventsHandler : public Object
	{
	public:

		//! constructor
		ShootEditorEventsHandler();

		//! destructor
		~ShootEditorEventsHandler();

		//! handles SetCameraPathElementInfoEvent
		void HandleSetCameraPathElementInfoEvent(Event* pEvent);
	};
}



