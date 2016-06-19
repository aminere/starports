/* 

Amine Rehioui
Created: August 7th 2011

*/

#include "ShootEditorCommon.h"
#include "ShootEditorEvents.h"
#include "EditorEvents.h"
#include "EventManager.h"

#include "CameraManager.h"

#include "CameraPathElement.h" // HandleSetCameraPathElementInfoEvent

namespace shoot
{
	//! constructor
	ShootEditorEventsHandler::ShootEditorEventsHandler()
	{
		EventManager::Instance()->RegisterListener<EditorEvents::SetCameraPathElementInfoEvent>(this, (EventManager::EventHandler)&ShootEditorEventsHandler::HandleSetCameraPathElementInfoEvent);
	}

	//! destructor
	ShootEditorEventsHandler::~ShootEditorEventsHandler()
	{
		EventManager::Instance()->UnregisterListener<EditorEvents::SetCameraPathElementInfoEvent>(this);
	}

	//! handles SetCameraPathElementInfoEvent
	void ShootEditorEventsHandler::HandleSetCameraPathElementInfoEvent(Event* pEvent)
	{
		CameraPathElement* pElement = DYNAMIC_CAST(ShootEditor::Instance()->GetSelectedEntity(), CameraPathElement);
		SHOOT_ASSERT(pElement, "HandleSetCameraPathElementInfoEvent entity not of type CameraPathElement");

		CameraManager::Instance()->SetCameraPathElementInfo(true);

		// move camera to path element location
		ShootEditor::Instance()->GetViewPort()->SwitchToEditor3DCamera();
		Camera* pCamera = EntityRenderer::Instance()->Get3DCamera();
		pCamera->SetPosition(pElement->GetAbsolutePosition());
		pCamera->SetLookAt(pElement->GetCameraLookAt());
	}
}
