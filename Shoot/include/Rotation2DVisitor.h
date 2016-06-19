/* 

Amine Rehioui
Created: April 7th 2011

*/

#pragma once

#include "AnimationVisitor.h"

#include "ExtendedProperties.h"

namespace shoot
{
	// forwards 
	class Entity2D;

	//! Rotation2DVisitor class
	class Rotation2DVisitor : public AnimationVisitor
	{
	public:

		DECLARE_OBJECT(Rotation2DVisitor, AnimationVisitor);

		//! constructor
		Rotation2DVisitor();

		//! destructor
		virtual ~Rotation2DVisitor();

		//! Reads/Writes struct properties from/to a stream
		virtual void Serialize(PropertyStream& stream);		

		//! visits a particular entity
		virtual void Visit(Entity* pTarget);

		//! updates the visitor
		virtual void Update();

	protected:

		Entity2D* m_p2DEntity;

		float m_fCurrentTime;
		float m_fSrcAngle;
		float m_fDestAngle;

		// properties
		ESP<float> m_Src;
		ESP<float> m_Dest;
		float m_fDuration;
	};
}

