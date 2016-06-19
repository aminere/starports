/* 

Amine Rehioui
Created: November 3rd 2013

*/

#include "Shoot.h"	

#include "EntityVisitor.h"

namespace shoot
{
	DEFINE_OBJECT(EntityVisitor);

	//! constructor
	EntityVisitor::EntityVisitor()
		// properties
		: m_bEntityVisible(true)
		, m_bCollisionEnabled(true)
	{
	}

	//! Reads/Writes struct properties from/to a stream
	void EntityVisitor::Serialize(PropertyStream& stream)
	{
		super::Serialize(stream);
		
		stream.Serialize("EntityVisible", &m_bEntityVisible);
		stream.Serialize("CollisionEnabled", &m_bCollisionEnabled);	
	}

	//! visits a particular entity
	void EntityVisitor::Visit(Entity* pTarget)
	{
		pTarget->SetVisible(m_bEntityVisible);
		if(CollisionComponent* pCollision = pTarget->GetComponent<CollisionComponent>())
			pCollision->SetEnabled(m_bCollisionEnabled);
	}	
}

