/* 

Amine Rehioui
Created: July 14th 2013

*/

#include "Shoot.h"

namespace shoot
{
	//! constructor
	GraphicObject::GraphicObject()
	{
		GraphicsDriver::Instance()->RegisterObject(this);
	}

	//! destructor
	GraphicObject::~GraphicObject()
	{
		GraphicsDriver::Instance()->UnregisterObject(this);
	}
}

