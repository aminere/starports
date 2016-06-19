/* 

Amine Rehioui
Created: November 8th 2014

*/

#pragma once

#include "Entity3DController.h"

namespace shoot
{
	//! GameEntity3DController
	class GameEntity3DController : public Entity3DController
	{
		typedef Entity3DController super;

	protected:

		//! OnEntityTranslated
		void OnTranslate(const Vector3&);
	};
}

