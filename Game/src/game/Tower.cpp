/* 

Amine Rehioui
Created: April 12th 2014

*/

#include "Game.h"

#include "Tower.h"
#include "ProgressBar.h"
#include "WaveManager.h"
#include "GameSettings.h"

namespace shoot
{
	DEFINE_OBJECT(Tower);

	//! constructor
	Tower::Tower()
	{
	}

	//! destructor
	Tower::~Tower()
	{
		if (GameEngine::Instance() && GAMECONTEXT && SFXMGR)
			SFXMGR->GetStencilEffects()->OnTilesChanged();
	}

	//! serializes the entity to/from a PropertyStream
	void Tower::Serialize(PropertyStream& stream)
	{
		super::Serialize(stream);
	}

	//! called during the initialization of the entity
	void Tower::Init()
	{
		super::Init();
	}

	//! called during the update of the entity
	void Tower::Update()
	{
	}

	//! checks for a target
	bool Tower::CheckTarget()
	{
		auto pickNewTarget = false;
		if (!m_Target.IsValid() || m_Target->GetHitPoints() < 0.0f)
		{
			pickNewTarget = true;
		}
		else
		{
			Vector3 vPos = m_Target->GetAbsolutePosition();
			vPos.Z = GetAbsolutePosition().Z;
			float fDistSq = (vPos - GetAbsolutePosition()).GetLengthSquared();
			if (fDistSq > m_Settings->m_Range*m_Settings->m_Range + GSETTINGS->m_RangeCheckTreshold)
				pickNewTarget = true;
		}

		if (pickNewTarget)
			PickTarget();

		return m_Target.IsValid();		
	}

	//! picks a target
	void Tower::PickTarget()
	{
		m_Target = NULL;
		float fMinDistSq = Math::Maxf32;
		auto rangeSQ = m_Settings->m_Range*m_Settings->m_Range;
		std::list< Handle<BaseAttacker> >& attackers = WAVEMGR->GetAttackers();
		for(std::list< Handle<BaseAttacker> >::iterator it = attackers.begin(); it != attackers.end(); ++it)
		{
			BaseAttacker* pAttacker = (*it).Get();
			if(!pAttacker || pAttacker->GetHitPoints() < 0.0f)
				continue;

			Vector3 vPos = pAttacker->GetAbsolutePosition();
			vPos.Z = GetAbsolutePosition().Z;
			float fDistSq = (vPos-GetAbsolutePosition()).GetLengthSquared();
			if (fDistSq < fMinDistSq && fDistSq < rangeSQ)
			{
				m_Target = pAttacker;
				fMinDistSq = fDistSq;
			}
		}
	}	
}

