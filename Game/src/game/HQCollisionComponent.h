/* 

Amine Rehioui
Created: December 31st 2013

*/

#pragma once


#include "RigidBodyComponent.h"

namespace shoot
{
	//! HQ collision component class
	class HQCollisionComponent : public RigidBodyComponent
	{
	public:
		
		DECLARE_OBJECT(HQCollisionComponent, RigidBodyComponent);

		//! called when a collision with another collision component is detected
		void OnCollision(CollisionComponent* pOther, std::vector<CollisionShape::CollisionInfo>& infos);
	};
}



