/* 

Amine Rehioui
Created: March 24th 2013

*/

#include "Game.h"

#include "LaserManagerCollisionComponent.h"

#include "LaserManager.h"

#include "CollisionUtils.h"

#include "EnvironmentCollisionComponent.h"

#include "RigidBodyComponent.h"

namespace shoot
{
	DEFINE_OBJECT(LaserManagerCollisionComponent);

	//! constructor
	LaserManagerCollisionComponent::LaserManagerCollisionComponent()
	{
		m_Priority = 2;
	}

	//! Initializes the component
	void LaserManagerCollisionComponent::Init()
	{
		super::Init();

		// TODO: restrict component type depending on entity, and enforce it in the editor
		SHOOT_ASSERT(m_pOwner->IsA(LaserManager::TypeID), "LaserManagerCollisionComponent parent must be a LaserManager");
	}

	//! checks collision with another component
	void LaserManagerCollisionComponent::CheckCollisions(CollisionComponent* pOther)
	{
		LaserManager* pLaserManager = static_cast<LaserManager*>(GetOwner());

		MemoryPool<Laser>& lasers = pLaserManager->GetLasers();
		if(EnvironmentCollisionComponent* pEnvCol = DYNAMIC_CAST(pOther, EnvironmentCollisionComponent))
		{
			for (size_t i = 0; i < lasers.GetCapacity(); ++i)
			{
				Laser* pLaser = lasers.GetElement(i);
				if (!pLaser)
					continue;

				if(pLaser->bStatic)
				{
					continue;
				}

				Vector3 vEnd = pLaser->vStart + pLaser->vDir * pLaser->fLength;
				if(pEnvCol->CheckCollision(vEnd, pLaser->vDir*pLaser->fSpeed*g_fDeltaTime, m_fCollisionRadius))
				{
					if(OnLaserCollision(pLaser, vEnd, pOther))
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

			for (size_t i = 0; i < lasers.GetCapacity(); ++i)
			{
				Laser* pLaser = lasers.GetElement(i);
				if (!pLaser)
					continue;

				if(!pLaser->bStatic && pLaser->fLife <= 0.0f && pOther->GetOwner()->GetAncestor<Actor>())				
					continue;

				for(auto &shape : pRigidBody->GetShapes())
				{
					if(!shape->Get())
						continue;

					auto collided = false;
					Vector3 vEnd = pLaser->vStart + pLaser->vDir * pLaser->fLength;
					Vector3 vIntersection = vEnd;
					float fDistToIntersection = -1.0f;

					if(CollisionShapeBox* pBox = DYNAMIC_CAST(shape->Get(), CollisionShapeBox))
					{
						collided = CollisionUtils::AABBox3DIntersectsWithSphere(pBox->GetBox()*otherTransform.GetScale()+otherTransform.GetMatrix().GetTranslation(),
							vEnd+pLaser->vDir*pLaser->fSpeed*g_fDeltaTime, m_fCollisionRadius);
						fDistToIntersection = pLaser->fLength;
					}
					else if(CollisionShapeSphere* pSphere = DYNAMIC_CAST(shape->Get(), CollisionShapeSphere))
					{
						collided = CollisionUtils::SphereIntersectsWithRay(pSphere->GetCenter(otherTransform.GetMatrix()), pSphere->GetRadius()*otherTransform.GetScale().X, pLaser->vStart, pLaser->vDir, &(pLaser->fLength), &fDistToIntersection);
						if(!collided)
						{
							Vector3 vLateralAxe = pLaser->vDir.CrossProduct(Vector3::Up);
							Vector3 vStartLeft = pLaser->vStart - vLateralAxe*pLaser->fThickness;
							collided = CollisionUtils::SphereIntersectsWithRay(pSphere->GetCenter(otherTransform.GetMatrix()), pSphere->GetRadius()*otherTransform.GetScale().X, vStartLeft, pLaser->vDir, &(pLaser->fLength), &fDistToIntersection);
							if(!collided)
							{
								Vector3 vStartRight = pLaser->vStart + vLateralAxe*pLaser->fThickness;
								collided = CollisionUtils::SphereIntersectsWithRay(pSphere->GetCenter(otherTransform.GetMatrix()), pSphere->GetRadius()*otherTransform.GetScale().X, vStartRight, pLaser->vDir, &(pLaser->fLength), &fDistToIntersection);
							}
						}
					}

					if(collided && OnLaserCollision(pLaser, pLaser->vStart + pLaser->vDir*fDistToIntersection, pOther))
						return;
				}
			}
		}
	}

	//! called on a bullet collision
	bool LaserManagerCollisionComponent::OnLaserCollision(Laser* pLaser, const Vector3& vPosition, CollisionComponent* pComponent)
	{
		if(pLaser->fLife > 0.0f)
			pLaser->fLife = 0.0f;

		if(Actor* pActor = pComponent->GetOwner()->GetAncestor<Actor>())
		{
			if(pActor->IsImmune())			
				SFXMGR->OnLaserCollided(vPosition);			

			if(pActor->OnShot(vPosition, this, pLaser->damage))
			{
				pComponent->SetEnabled(false);
				return true;
			}
		}
		
		SFXMGR->OnLaserCollided(vPosition);
		return false;
	}
}

