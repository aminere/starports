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
	class Scale3DVisitor : public AnimationVisitor
	{
	public:

		DECLARE_OBJECT(Scale3DVisitor, AnimationVisitor);

		//! constructor
		Scale3DVisitor();

		//! destructor
		virtual ~Scale3DVisitor();

		//! Reads/Writes struct properties from/to a stream
		virtual void Serialize(PropertyStream& stream);		

		//! visits a particular entity
		virtual void Visit(Entity* pTarget);

		//! updates the visitor		
		virtual void Update();

	protected:

		Entity3D* m_p3DEntity;

		float m_fCurrentTime;	
		Vector3 m_vSrcScale;
		Vector3 m_vDestScale;
		float m_fCurrentDuration;

		// properties
		ESP<Vector3> m_Src;
		ESP<Vector3> m_Dest;
		float m_fDuration;
		bool m_bRandomize;
	};
}

