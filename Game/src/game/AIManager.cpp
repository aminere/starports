/* 

Amine Rehioui
Created: May 1st 2013

*/

#include "Game.h"

#include "AIManager.h"

#include "GameManager.h"

namespace shoot
{
	DEFINE_OBJECT(AIManager);

	//! constructor
	AIManager::AIManager()
	{
	}

	//! destructor
	AIManager::~AIManager()
	{
	}

	//! serializes the entity to/from a PropertyStream
	void AIManager::Serialize(PropertyStream& stream)
	{
		super::Serialize(stream);
	}

	//! called during the initialization of the entity
	void AIManager::Init()
	{
		super::Init();		
	}

	//! called during the update of the entity
	void AIManager::Update()
	{
	}

	//! called when an actor has been spawned
	void AIManager::OnActorSpawned(Actor* pActor)
	{
		auto list = GetActorList(pActor->GetType());
		list->push_back(Handle<Actor>(pActor));
	}

	//! called when an actor has been destroyed
	void AIManager::OnActorDestroyed(Actor* pActor)
	{
		auto list = GetActorList(pActor->GetType());
		Handle<Actor> actor(pActor);
		auto it = std::find(list->begin(), list->end(), actor);
		if(it != list->end())
			list->erase(it);
	}

	//! returns info about the closest actor with respect to a pellet
	bool AIManager::GetClosestActor(Actor::E_Type type, const Vector3& vPosition, const float& fMinCos, Vector3& vActorPos, float& fDistToActor)
	{
		auto list = GetActorList(type);
		if(list->empty())
			return false;

		Actor* pClosestActor = NULL;
		float fDistToClosestActorSquared = 0.0f;
		for(auto it = list->begin(); it != list->end(); )
		{
			if((*it).IsValid())
			{
				if((*it)->GetHitPoints() >= 0.0f)
				{
					Vector3 vActorPos = (*it)->GetMeshEntity()->GetAbsolutePosition();
					float fDistSquared = (vActorPos-vPosition).GetLengthSquared();
					Vector3 vToActor = (vActorPos-vPosition).Normalize();	
					float fDotP = Vector3::Create(0.0f, 1.0f, 0.0f).DotProduct(vToActor);
					if((pClosestActor == NULL || fDistSquared < fDistToClosestActorSquared)
						&& (fDotP > fMinCos))
					{
						pClosestActor = (*it).Get();
						fDistToClosestActorSquared = fDistSquared;
					}
				}
				++it;
			}
			else
			{
				it = list->erase(it);
			}
		}

		if(!pClosestActor)
			return false;

		vActorPos = pClosestActor->GetMeshEntity()->GetAbsolutePosition();
		fDistToActor = Math::SquareRoot(fDistToClosestActorSquared);
		return fDistToActor > Math::Epsilon;
	}

	//! returns a list given an actor type
	std::list< Handle<Actor> >* AIManager::GetActorList(Actor::E_Type type)
	{
		switch(type)
		{
		case Actor::T_Enemy: return &m_lEnemies;
		default: return &m_lPlayerUnits;
		}
	}
}

