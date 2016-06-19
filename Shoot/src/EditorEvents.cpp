/* 

Amine Rehioui
Created: August 7th 2011

*/

#include "Shoot.h"

#include "EditorEvents.h"

namespace shoot
{
	DEFINE_OBJECT(EditorEvents::SetCameraPathElementInfoEvent);

	//! registers engine events
	void EditorEvents::Register()
	{
		ObjectFactory::Instance()->Register<SetCameraPathElementInfoEvent>();
	}
}

