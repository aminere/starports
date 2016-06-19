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
	class Scale2DVisitor : public AnimationVisitor
	{
	public:

		DECLARE_OBJECT(Scale2DVisitor, AnimationVisitor);

		//! constructor
		Scale2DVisitor();

		//! destructor
		virtual ~Scale2DVisitor();

		//! Reads/Writes struct properties from/to a stream
		virtual void Serialize(PropertyStream& stream);		

		//! visits a particular entity
		virtual void Visit(Entity* pTarget);

		//! updates the visitor
		virtual void Update();

	protected:

		Entity2D* m_p2DEntity;

		float m_fCurrentTime;	
		Vector2 m_vSrcScale;
		Vector2 m_vDestScale;

		// properties
		ESP<Vector2> m_Src;
		ESP<Vector2> m_Dest;
		float m_fDuration;		
	};
}

