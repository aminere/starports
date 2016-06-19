/* 

Amine Rehioui
Created: April 2nd 2011

*/

#include "Shoot.h"	

#include "Visitor.h"

namespace shoot
{
	DEFINE_OBJECT(Visitor);

	//! constructor
	Visitor::Visitor()
		: m_pTarget(NULL)
		, m_bActive(false)

		// properties
		, m_bAutoStart(true)
	{
	}

	//! destructor
	Visitor::~Visitor()
	{
		if(IsActive())
		{
			Leave();
		}
	}

	//! Reads/Writes struct properties from/to a stream
	void Visitor::Serialize(PropertyStream& stream)
	{
		super::Serialize(stream);
		stream.Serialize("AutoStart", &m_bAutoStart);
	}

	//! Initializes the component
	void Visitor::Init()
	{
		if( m_bAutoStart
		&& ((m_pTarget != m_pOwner) || !IsActive()))
		{
			Visit(m_pOwner);
		}		
	}

	//! visits a particular entity
	void Visitor::Visit(Entity* pTarget)
	{
		SHOOT_ASSERT(pTarget, "Cannot Visit a NULL entity");
		m_pTarget = pTarget;
		m_bActive = true;
	}

	//! Leaves the visited entity
	void Visitor::Leave()
	{
		m_pTarget = NULL;
		m_bActive = false;
	}
}

