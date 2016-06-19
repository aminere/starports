/* 

Amine Rehioui
Created: May 27th 2014

*/

#include "Game.h"

#include "AttackerSettings.h"

namespace shoot
{
	DEFINE_OBJECT(AttackerSettings);

	//! constructor
	AttackerSettings::AttackerSettings()
		: m_fMaxFlySpeed(2.0f)
		, m_fMaxAccel(40.0f)
		, m_fSeparationDist(2.0f)
		, m_fSeparationFactor(0.2f)
		, m_fSteerFactor(6.0f)
		, m_fLookAtDuration(0.5f)
		, m_fDecelDuration(0.5f)
		, m_fHQRange(10.0f)
		, m_fHQTreshold(1.0f)
		, m_Reward(25)
	{
	}

	//! serializes the entity to/from a PropertyStream
	void AttackerSettings::Serialize(PropertyStream& stream)
	{
		super::Serialize(stream);

		stream.Serialize("MaxFlySpeed", &m_fMaxFlySpeed);
		stream.Serialize("MaxAccel", &m_fMaxAccel);
		stream.Serialize("SeparationDist", &m_fSeparationDist);
		stream.Serialize("SeparationFactor", &m_fSeparationFactor);
		stream.Serialize("SteerFactor", &m_fSteerFactor);
		stream.Serialize("LookAtDuration", &m_fLookAtDuration);
		stream.Serialize("DecelDuration", &m_fDecelDuration);
		stream.Serialize("HQRange", &m_fHQRange);
		stream.Serialize("HQTreshold", &m_fHQTreshold);
		stream.Serialize("Reward", &m_Reward);
	}	
}

