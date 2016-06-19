/* 

Amine Rehioui
Created: May 23rd 2013

*/

#pragma once

#include "AnimationVisitor.h"

namespace shoot
{
	// forwards 
	class Entity3D;

	//! Shake3DVisitor class
	class Shake3DVisitor : public AnimationVisitor
	{
	public:

		DECLARE_OBJECT(Shake3DVisitor, AnimationVisitor);

		//! constructor
		Shake3DVisitor();

		//! destructor
		virtual ~Shake3DVisitor();

		//! Reads/Writes struct properties from/to a stream
		virtual void Serialize(PropertyStream& stream);		

		//! visits a particular entity
		virtual void Visit(Entity* pTarget);

		//! updates the visitor		
		virtual void Update();

	protected:

		Entity3D* m_p3DEntity;
		float m_fCurrentTime;

		// properties
		float m_fDuration;
		Vector3 m_vAmplitude;
		Vector3 m_vFrequency;
	};
}

