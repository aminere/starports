/* 

Amine Rehioui
Created: April 5th 2014

*/

#include "Game.h"

#include "GroundCollisionComponent.h"
#include "NuclearTower.h"

namespace shoot
{
	DEFINE_OBJECT(GroundCollisionComponent);

	//! called when a collision with another collision component is detected
	void GroundCollisionComponent::OnCollision(CollisionComponent* pOther, std::vector<CollisionShape::CollisionInfo>& infos)
	{
		Actor* pActor = pOther->GetOwner()->GetAncestor<Actor>();
		if(NuclearTower* pNuclearTower = DYNAMIC_CAST(pActor, NuclearTower))
		{
			pNuclearTower->OnMissileLanded(infos[0].vIntersection);
		}
		else if(pActor)
		{
			pOther->SetEnabled(false);
			pActor->OnShot(infos[0].vIntersection, this, 9999.0f);
		}
	}
}

