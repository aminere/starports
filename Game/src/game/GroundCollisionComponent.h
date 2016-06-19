/* 

Amine Rehioui
Created: April 5th 2014

*/

#pragma once


#include "RigidBodyComponent.h"

namespace shoot
{
	//! Ground collision component class
	class GroundCollisionComponent : public RigidBodyComponent
	{
	public:
		
		DECLARE_OBJECT(GroundCollisionComponent, RigidBodyComponent);

		//! called when a collision with another collision component is detected
		void OnCollision(CollisionComponent* pOther, std::vector<CollisionShape::CollisionInfo>& infos);
	};
}



