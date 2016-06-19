/* 

Amine Rehioui
Created: February 5th 2012

*/

#include "Game.h"

#include "BulletManagerCollisionComponent.h"

#include "BulletManager.h"

#include "CollisionUtils.h"

#include "EnvironmentCollisionComponent.h"

#include "RigidBodyComponent.h"

#include "HQ.h"
#include "HQCollisionComponent.h"

namespace shoot
{
	DEFINE_OBJECT(BulletManagerCollisionComponent);

	//! constructor
	BulletManagerCollisionComponent::BulletManagerCollisionComponent()
	{
		m_Priority = 2;
	}

	//! Initializes the component
	void BulletManagerCollisionComponent::Init()
	{
		super::Init();

		// TODO: restrict component type depending on entity, and enforce it in the editor
		SHOOT_ASSERT(m_pOwner->IsA(BulletManager::TypeID), "BulletManagerCollisionComponent parent must be a BulletManager");
	}

	//! checks collision with another component
	void BulletManagerCollisionComponent::CheckCollisions(CollisionComponent* pOther)
	{
		BulletManager* pBulletManager = static_cast<BulletManager*>(GetOwner());
		MemoryPool<Bullet>& bullets = pBulletManager->GetBullets();

		if(EnvironmentCollisionComponent* pEnvCol = DYNAMIC_CAST(pOther, EnvironmentCollisionComponent))
		{
			for (size_t i = 0; i < bullets.GetCapacity(); ++i)
			{
				Bullet* pBullet = bullets.GetElement(i);
				if (!pBullet)
					continue;

				if(pEnvCol->CheckCollision(pBullet->vPosition, pBullet->vDirection*pBullet->fSpeed*g_fDeltaTime, m_fCollisionRadius))
				{
					if(OnBulletCollision(pBullet, pEnvCol))
					{
						break;
					}
				}
			}
		}
		else if(RigidBodyComponent* pRigidBody = DYNAMIC_CAST(pOther, RigidBodyComponent))
		{
			Entity3D* pOtherParent = static_cast<Entity3D*>(pOther->GetOwner());
			CollisionShapeTransform otherTransform(pOtherParent->GetWorldTransform(), pOtherParent->GetAbsoluteScale());

			for (size_t i = 0; i < bullets.GetCapacity(); ++i)
			{
				Bullet* pBullet = bullets.GetElement(i);
				if (!pBullet)
					continue;

				for (auto &shape : pRigidBody->GetShapes())
				{
					if(!shape->Get())
						continue;

					auto collided = false;
					if(CollisionShapeBox* pBox = DYNAMIC_CAST(shape->Get(), CollisionShapeBox))
					{
						collided = CollisionUtils::AABBox3DIntersectsWithSphere(pBox->GetBox()*otherTransform.GetScale()+otherTransform.GetMatrix().GetTranslation(), pBullet->vPosition+pBullet->vDirection*pBullet->fSpeed*g_fDeltaTime, pBullet->fRadius);
					}
					else if(CollisionShapeSphere* pSphere = DYNAMIC_CAST(shape->Get(), CollisionShapeSphere))
					{
						collided = CollisionUtils::SphereIntersectsWithSphere(pBullet->vPosition+pBullet->vDirection*pBullet->fSpeed*g_fDeltaTime, pBullet->fRadius, 
							pSphere->GetCenter(otherTransform.GetMatrix()), pSphere->GetRadius()*otherTransform.GetScale().X);
					}

					if(collided && OnBulletCollision(pBullet, pOther))
						return;
				}
			}
		}
	}

	//! called on a bullet collision
	bool BulletManagerCollisionComponent::OnBulletCollision(Bullet* pBullet, CollisionComponent* pComponent)
	{
		pBullet->fLife = 0.0f;
		if(Actor* pActor = pComponent->GetOwner()->GetAncestor<Actor>())
		{
			if(pActor->IsImmune())
			{
				SFXMGR->OnBulletCollided(pBullet);
			}

			if(pActor->OnShot(pBullet->vPosition, this, pBullet->damage))
			{
				pComponent->SetEnabled(false);
				return true;
			}
		}
		else if(pComponent->IsA(HQCollisionComponent::TypeID))
		{
			SFXMGR->OnDamageDone(pBullet->vPosition);
			if(pComponent->GetOwner()->GetAncestor<HQ>()->OnDamage(pBullet->damage))
			{
				pComponent->SetEnabled(false);
			}
			return true;
		}

		SFXMGR->OnBulletCollided(pBullet);
		return false;
	}
}

