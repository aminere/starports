/* 

Amine Rehioui
Created: August 14th 2011

*/

#include "Shoot.h"

#include "CameraPathElement.h"

#include "EditorEvents.h"

namespace shoot
{
	DEFINE_OBJECT(CameraPathElement);

	//! serializes the entity to/from a PropertyStream
	void CameraPathElement::Serialize(PropertyStream& stream)
	{
		super::Serialize(stream);
		stream.Serialize("CameraLookAt", &m_vCameraLookAt);

#ifdef SHOOT_EDITOR
		IEvent setCameraInfoEvent = EditorEvents::SetCameraPathElementInfoEvent::GetStaticClassName();
		stream.Serialize("SetCameraInfo", &setCameraInfoEvent);
#endif // SHOOT_EDITOR
	}
}

