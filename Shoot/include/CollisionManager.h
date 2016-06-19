/* 

Amine Rehioui
Created: December 28th 2010

*/

#pragma once


#include "CollisionComponent.h"

namespace shoot
{
	//! Collision manager
	class CollisionManager : public Object
	{
		DECLARE_OBJECT(CollisionManager, Object);

	public:

		//! performs collision detection
		void Update();

		//! adds a collision component
		void Add(CollisionComponent* pComponent);

		//! sets the parent context
		inline void SetContext(Context* pContext) { m_Context = pContext; }
		
	private:

		//! checks for collision between two components
		void CheckCollision(CollisionComponent* pComponent1, CollisionComponent* pComponent2);

		typedef std::list< Handle<CollisionComponent> > CollisionComponentList;
		typedef std::map< CollisionComponent::E_Group, CollisionComponentList > CollisionMap;
		CollisionMap m_CollisionMap;

		Handle<Context> m_Context;
	};
}



