/* 

Amine Rehioui
Created: April 5th 2011

*/

#pragma once


#include "Visitor.h"

namespace shoot
{
	//! AnimationVisitor class
	class AnimationVisitor : public Visitor
	{
	public:

		DECLARE_OBJECT(AnimationVisitor, Visitor);

		//! playback types
		enum E_PlaybackType
		{
			PT_Once,
			PT_Toggle,
			PT_Loop			
		};

		//! constructor
		AnimationVisitor();

		//! destructor
		virtual ~AnimationVisitor();

		//! Reads/Writes struct properties from/to a stream
		virtual void Serialize(PropertyStream& stream);		

		//! visits a particular entity
		virtual void Visit(Entity* pTarget);

	protected:

		int m_iPlayCount;

		// properties		
		E_PlaybackType m_ePlaybackType;
		int m_iMaxPlayCount;
	};
}



