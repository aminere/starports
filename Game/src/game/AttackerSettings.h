/* 

Amine Rehioui
Created: May 27th 2014

*/

#pragma once


namespace shoot
{
	//! Attacker settings
	class AttackerSettings : public ActorSettings
	{
		DECLARE_OBJECT(AttackerSettings, ActorSettings);

	public:

		//! constructor
		AttackerSettings();

		//! serializes the entity to/from a PropertyStream
		void Serialize(PropertyStream& stream);

		float m_fMaxFlySpeed;
		float m_fMaxAccel;
		float m_fSeparationDist;
		float m_fSeparationFactor;
		float m_fSteerFactor;
		float m_fLookAtDuration;
		float m_fDecelDuration;
		float m_fHQRange;
		float m_fHQTreshold;
		int m_Reward;
	};
}



