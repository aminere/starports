/* 

Amine Rehioui
Created: December 11th 2013

*/

#include "Game.h"

#include "Pathway.h"

namespace shoot
{
	DEFINE_OBJECT(Pathway);
	DEFINE_OBJECT(PathwayElement);

	//! constructor
	PathwayElement::PathwayElement()
		// properties
		: m_fTilt(0.0f)
	{
	}

	//! serializes the entity to/from a PropertyStream
	void PathwayElement::Serialize(PropertyStream& stream)
	{
		super::Serialize(stream);
		stream.Serialize("Tilt", &m_fTilt);
	}

	//! adds a child to the entity
	void Pathway::AddChild(Entity* pChild, bool bImmediate /*= false*/)
	{
		SHOOT_ASSERT(pChild->IsA(PathwayElement::TypeID), "Pathway child is not of type PathwayElement");
		super::AddChild(pChild, bImmediate);
	}
}

