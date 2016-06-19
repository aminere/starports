/* 

Amine Rehioui
Created: December 31th 2013

*/

#pragma once


#include "Entity3D.h"

namespace shoot
{
	//! SpawnPoint class
	class SpawnPoint : public Entity3D
	{
	public:
		
		DECLARE_OBJECT(SpawnPoint, Entity3D);

		//! constructor
		SpawnPoint();

		//! serializes the entity to/from a PropertyStream
		void Serialize(PropertyStream& stream);
	};
}




