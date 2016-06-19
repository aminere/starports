/* 

Amine Rehioui
Created: April 9th 2011

*/

#pragma once

#include "AnimationVisitor.h"

#include "ExtendedProperties.h"

namespace shoot
{
	// forwards 
	class Entity3D;

	//! Rotation3DVisitor class
	class Translation3DVisitor : public AnimationVisitor
	{
	public:

		DECLARE_OBJECT(Translation3DVisitor, AnimationVisitor);

		//! constructor
		Translation3DVisitor();

		//! destructor
		virtual ~Translation3DVisitor();

		//! Reads/Writes struct properties from/to a stream
		virtual void Serialize(PropertyStream& stream);		

		//! visits a particular entity
		virtual void Visit(Entity* pTarget);

		//! updates the visitor		
		virtual void Update();

	protected:

		Entity3D* m_p3DEntity;

		float m_fCurrentTime;
		Vector3 m_vSrcPos;
		Vector3 m_vDestPos;

		// properties
		ESP<Vector3> m_Src;
		ESP<Vector3> m_Dest;
		float m_fDuration;
	};
}

