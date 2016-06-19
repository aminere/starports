/* 

Amine Rehioui
Created: March 8th 2014

*/

#include "Game.h"

#include "KamikazeAttacker.h"
#include "GameSettings.h"
#include "WaveManager.h"

namespace shoot
{
	DEFINE_OBJECT(KamikazeAttacker);

	//! constructor
	KamikazeAttacker::KamikazeAttacker()		
		: m_eState(S_Moving)
		, m_fDecelTimer(0.0f)
	{
	}	

	//! serializes the entity to/from a PropertyStream
	void KamikazeAttacker::Serialize(PropertyStream& stream)
	{
		super::Serialize(stream);		
	}

	//! called during the initialization of the entity
	void KamikazeAttacker::Init()
	{
		super::Init();
	}

	//! called during the update of the entity
	void KamikazeAttacker::Update()
	{
		if(m_fHitPoints < 0.0f || !GSETTINGS)
		{
			return;
		}

		auto settings = DYNAMIC_CAST(m_Settings, KamikazeAttackerSettings);
		switch (m_eState)
		{
		case KamikazeAttacker::S_Moving:
			{		
				Vector3 vToHQ = m_TargetHQ->GetAbsolutePosition() - GetAbsolutePosition();
				bool reachedHQ = false;
				if ((int)m_CurrentTarget > (int)m_Path.size() - 3)
				{
					float fDistToHQSq = vToHQ.GetLengthSquared();
					reachedHQ = fDistToHQSq < settings->m_fHQKamikazeRange*settings->m_fHQKamikazeRange;
				}
				if (reachedHQ)
				{
					m_fDecelTimer = 0.0f;
					m_eState = S_PreparingAttack;
					m_bTargetReached = true;
					break;
				}

				if (!m_bTargetReached)
					MoveToTarget();
				else
					SteerToTarget();

				LookAtTarget();
			}
			break;

		case S_PreparingAttack:
			{
				if (m_TargetHQ->GetHitPoints() < 0.0f)
				{
					if (auto nextTarget = WAVEMGR->GetNextHQ())
					{
						m_TargetHQ = nextTarget;
						m_eState = S_Moving;
					}
					else
						m_eState = S_Idle;
				}
				else
				{
					m_vSpeed = Math::Damp(m_vSpeed, Vector3::Zero, g_fDeltaTime, settings->m_fDecelDuration);
					m_fDecelTimer += g_fDeltaTime;
					if (m_fDecelTimer > settings->m_fDecelDuration)
					{
						m_eState = S_AttackingHQ;
						break;
					}

					Translate(m_vSpeed * g_fDeltaTime);

					auto vToHQ = (m_TargetHQ->GetAbsolutePosition() - GetAbsolutePosition()).Normalized();
					m_vLookAt = Math::Damp(m_vLookAt, vToHQ, g_fDeltaTime, 0.1f).Normalized();
					LookAt(m_vLookAt);
				}
			}
			break;

		case KamikazeAttacker::S_AttackingHQ:
			{
				Vector3 vToHQ = (m_TargetHQ->GetAbsolutePosition() - GetAbsolutePosition()).Normalized();
				Vector3 vAcceleration = vToHQ*settings->m_fKamikazeMaxAccel*g_fDeltaTime;
				m_vSpeed += vAcceleration;
				m_vSpeed.Limit(settings->m_fKamikazeMaxSpeed);
				Translate(m_vSpeed * g_fDeltaTime);
			}
			break;		
		}

		super::Update();
	}	
}

