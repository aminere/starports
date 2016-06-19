/* 

Amine Rehioui
Created: December 30th 2010

*/

#include "Shoot.h"

#include "CollisionManager.h"

namespace shoot
{
	DEFINE_OBJECT(CollisionManager);

	//! performs collision detection
	void CollisionManager::Update()
	{
		static bool bCollisionChecked[CollisionComponent::NumCollisionGroups][CollisionComponent::NumCollisionGroups];
		memset(bCollisionChecked, 0, CollisionComponent::NumCollisionGroups*CollisionComponent::NumCollisionGroups*sizeof(bool));

		int group1Index = 0;
		for(CollisionMap::iterator group1 = m_CollisionMap.begin(); group1 != m_CollisionMap.end();	++group1)
		{
			int group2Index = 0;
			for(CollisionMap::iterator group2 = m_CollisionMap.begin(); group2 != m_CollisionMap.end(); ++group2)
			{
				if(!bCollisionChecked[group1Index][group2Index]
				&& group1Index != group2Index)
				{
					CollisionComponentList& group1Components = (*group1).second;
					CollisionComponentList& group2Components = (*group2).second;

					for(CollisionComponentList::iterator it = group1Components.begin(); it != group1Components.end(); )
					{
						for(CollisionComponentList::iterator it2 = group2Components.begin(); it2 != group2Components.end(); )
						{
							if(CollisionComponent* pComponent1 = (*it).Get())
							{
								if(CollisionComponent* pComponent2 = (*it2).Get())
								{
									CheckCollision(pComponent1, pComponent2);									
									++it2;
								}
								else
								{
									it2 = group2Components.erase(it2);
								}
							}
							else
							{
								break;
							}
						}
						if((*it).IsValid())
						{
							++it;
						}
						else
						{
							it = group1Components.erase(it);
						}
					}

					bCollisionChecked[group1Index][group2Index] = true;
					bCollisionChecked[group2Index][group1Index] = true;
				}
				++group2Index;
			}
			++group1Index;
		}

		// check with QuadTree components, if available
		if(QuadTree* pQuadTree = m_Context->GetQuadTree())
		{
			const std::list<QuadTreeNode*>& activeNodes = pQuadTree->GetActiveNodes();
			for(CollisionMap::iterator it = m_CollisionMap.begin(); it != m_CollisionMap.end();	++it)
			{
				CollisionComponentList& components = (*it).second;
				for(CollisionComponentList::iterator it2 = components.begin(); it2 != components.end(); )					
				{
					if(CollisionComponent* pComponent1 = (*it2).Get())
					{
						for(std::list<QuadTreeNode*>::const_iterator it3 = activeNodes.begin(); it3 != activeNodes.end(); ++it3)
						{
							std::list< Handle<EnvironmentCollisionComponent> >& envComponents = (*it3)->GetCollisionComponents();
							for(std::list< Handle<EnvironmentCollisionComponent> >::iterator it4 = envComponents.begin(); it4 != envComponents.end(); )
							{
								if(CollisionComponent* pComponent2 = (*it4).Get())
								{
									CheckCollision(pComponent1, pComponent2);
									++it4;
								}
								else
								{
									it4 = envComponents.erase(it4);
								}
							}
						}
						++it2;
					}
					else
					{
						it2 = components.erase(it2);
					}
				}
			}
		}
	}

	//! checks for collision between two components
	void CollisionManager::CheckCollision(CollisionComponent* pComponent1, CollisionComponent* pComponent2)
	{
		SHOOT_ASSERT(pComponent1 != pComponent2, "Trying to check collision with self.");
		if(pComponent1->CanCollideWith(pComponent2->GetGroup())
		&& pComponent2->CanCollideWith(pComponent1->GetGroup()))
		{
			if(pComponent1->GetPriority() >= pComponent2->GetPriority())
			{
				pComponent1->CheckCollisions(pComponent2);
			}
			else
			{
				pComponent2->CheckCollisions(pComponent1);
			}
		}
	}

	//! adds a collision component
	void CollisionManager::Add(CollisionComponent* pComponent)
	{
		if(!(pComponent->IsA(EnvironmentCollisionComponent::TypeID) && m_Context->GetQuadTree()))
		{
			Handle<CollisionComponent> component(pComponent);
			CollisionComponentList& components = m_CollisionMap[pComponent->GetGroup()];
			CollisionComponentList::iterator it = std::find(components.begin(), components.end(), component);
			SHOOT_ASSERT(it == components.end(), "Trying to add a CollisionComponent twice");
			components.push_back(component);
		}
	}
}
