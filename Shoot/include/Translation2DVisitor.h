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
	class Entity2D;

	//! Rotation2DVisitor class
	class Translation2DVisitor : public AnimationVisitor
	{
	public:

		DECLARE_OBJECT(Translation2DVisitor, AnimationVisitor);

		//! constructor
		Translation2DVisitor();

		//! destructor
		virtual ~Translation2DVisitor();

		//! Reads/Writes struct properties from/to a stream
		virtual void Serialize(PropertyStream& stream);		

		//! visits a particular entity
		virtual void Visit(Entity* pTarget);

		//! updates the visitor
		virtual void Update();

	protected:

		Entity2D* m_p2DEntity;

		float m_fCurrentTime;	
		Vector2 m_vSrcPos;
		Vector2 m_vDestPos;

		// properties
		ESP<Vector2> m_Src;
		ESP<Vector2> m_Dest;
		float m_fDuration;		
	};
}

