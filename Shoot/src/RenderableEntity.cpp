/* 

Amine Rehioui
Created: January 4th 2012

*/

#include "Shoot.h"

#include "RenderableEntity.h"

namespace shoot
{
	DEFINE_OBJECT(RenderableEntity);

	//! registers the entity and its children for rendering
	void RenderableEntity::RegisterForRendering()
	{
		EntityRenderer::Instance()->RegisterEntity(this);
	}
}

