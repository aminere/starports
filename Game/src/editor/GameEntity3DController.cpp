/* 

Amine Rehioui
Created: November 8th 2014

*/

#include "GameEditorCommon.h"

#include "GameEntity3DController.h"
#include "PathwayGraphicComponent.h"
#include "Pathway.h"

namespace shoot
{
	//! OnTranslate
	void GameEntity3DController::OnTranslate(const Vector3& localDelta)
	{
		super::OnTranslate(localDelta);
		
		if(auto pathwayGC = m_Entity->GetComponent<PathwayGraphicComponent>())
		{
			pathwayGC->UpdatePath();
		}
		else if(auto pathwayElement = DYNAMIC_CAST(m_Entity.Get(), PathwayElement))
		{
			if(auto pathwayGC = pathwayElement->GetParent()->GetComponent<PathwayGraphicComponent>())
				pathwayGC->UpdatePath();
		}
	}
}

