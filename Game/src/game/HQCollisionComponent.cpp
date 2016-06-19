/* 

Amine Rehioui
Created: December 31st 2013

*/

#include "Game.h"

#include "HQCollisionComponent.h"
#include "HQ.h"
#include "KamikazeAttacker.h"
#include "GameSettings.h"

namespace shoot
{
	DEFINE_OBJECT(HQCollisionComponent);

	//! called when a collision with another collision component is detected
	void HQCollisionComponent::OnCollision(CollisionComponent* pOther, std::vector<CollisionShape::CollisionInfo>& infos)
	{
		if(Actor* pActor = pOther->GetOwner()->GetAncestor<Actor>())
		{
			pOther->SetEnabled(false);
			pActor->OnShot(infos[0].vIntersection, this, 9999.0f);

			// crash damage
			auto damage = GSETTINGS->m_DefaultCrashDamage;
			if (auto kamikaze = DYNAMIC_CAST(pActor, KamikazeAttacker))
			{
				auto settings = DYNAMIC_CAST(kamikaze->GetSettings(), KamikazeAttackerSettings);
				damage = settings->m_fCrashDamage;
			}
					
			if (GetOwner()->GetAncestor<HQ>()->OnDamage(damage))
				SetEnabled(false);
		}
	}
}

