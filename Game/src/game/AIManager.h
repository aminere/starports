/* 

Amine Rehioui
Created: May 1st 2013

*/

#pragma once


namespace shoot
{
	// forwards
	class Actor;	

	//! item manager class
	class AIManager : public Entity
	{
		DECLARE_OBJECT(AIManager, Entity);

	public:

		//! constructor
		AIManager();

		//! destructor
		virtual ~AIManager();

		//! serializes the entity to/from a PropertyStream
		void Serialize(PropertyStream& stream);

		//! called during the initialization of the entity
		void Init();

		//! called during the update of the entity
		void Update();

		//! called when an actor has been spawned
		void OnActorSpawned(Actor* pActor);

		//! called when an enemy has been destroyed
		void OnActorDestroyed(Actor* pActor);

		//! returns info about the closest actor with respect to a pellet
		bool GetClosestActor(Actor::E_Type type, const Vector3& vPosition, const float& fMinCos, Vector3& vActorPos, float& fDistToActor);

	private:

		//! returns a list given an actor type
		std::list< Handle<Actor> >* GetActorList(Actor::E_Type type);

		std::list< Handle<Actor> > m_lEnemies;
		std::list< Handle<Actor> > m_lPlayerUnits;
	};
}



