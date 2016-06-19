/* 

Amine Rehioui
Created: December 31th 2013

*/

#include "Game.h"

#include "SpawnPoint.h"

namespace shoot
{
	DEFINE_OBJECT(SpawnPoint);

	//! constructor
	SpawnPoint::SpawnPoint()
	{
	}

	//! serializes the entity to/from a PropertyStream
	void SpawnPoint::Serialize(PropertyStream& stream)
	{
		super::Serialize(stream);
	}
}

