/* 

Amine Rehioui
Created: April 5th 2011

*/

#include "Shoot.h"	

#include "AnimationVisitor.h"

namespace shoot
{
	DEFINE_OBJECT(AnimationVisitor);

	// playback type literals
	const char* const g_strPlaybackTypeLiterals[] =
	{
		"Once",
		"Toggle",
		"Loop",
		0
	};

	//! constructor
	AnimationVisitor::AnimationVisitor()
		: m_iPlayCount(0)

		// properties
		, m_ePlaybackType(PT_Once)
		, m_iMaxPlayCount(-1)
	{
	}

	//! destructor
	AnimationVisitor::~AnimationVisitor()
	{
	}

	//! Reads/Writes struct properties from/to a stream
	void AnimationVisitor::Serialize(PropertyStream& stream)
	{
		super::Serialize(stream);
		
		stream.Serialize("PlaybackType", &m_ePlaybackType, ENUM_PARAMS1(g_strPlaybackTypeLiterals));
		stream.Serialize("MaxPlayCount", &m_iMaxPlayCount);
	}

	//! visits a particular entity
	void AnimationVisitor::Visit(Entity* pTarget)
	{
		m_iPlayCount = 0;
		super::Visit(pTarget);
	}
}

