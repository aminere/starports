/* 

Amine Rehioui
Created: March 8th 2014

*/

#include "Game.h"

#include "ShootingAttacker.h"
#include "Playfield.h"
#include "AudioManager.h"
#include "GameSettings.h"
#include "Visitor.h"
#include "WaveManager.h"

namespace shoot
{
	DEFINE_OBJECT(ShootingAttacker);

	//! constructor
	ShootingAttacker::ShootingAttacker()
		: m_fShootTimer(-1.0f)
		, m_eState(S_Moving)
		// properties
		, m_fBulletSpeed(15.0f)
		, m_fBulletLife(4.0f)
		, m_vBulletOffset(Vector3::Create(0.0f, 0.85f, -0.1f))
		, m_fBulletSize(1.0f)
		, m_fBulletFrequency(0.3f)
	{
	}

	//! destructor
	ShootingAttacker::~ShootingAttacker()
	{
	}

	//! serializes the entity to/from a PropertyStream
	void ShootingAttacker::Serialize(PropertyStream& stream)
	{
		super::Serialize(stream);

		stream.Serialize("BulletSpeed", &m_fBulletSpeed);
		stream.Serialize("BulletLife", &m_fBulletLife);
		stream.Serialize("BulletOffset", &m_vBulletOffset);
		stream.Serialize("BulletSize", &m_fBulletSize);
		stream.Serialize("BulletFrequency", &m_fBulletFrequency);
	}

	//! called during the initialization of the entity
	void ShootingAttacker::Init()
	{
		super::Init();
	}

	//! called during the update of the entity
	void ShootingAttacker::Update()
	{
		if(m_fHitPoints < 0.0f)
			return;

		switch (m_eState)
		{
		case shoot::ShootingAttacker::S_Moving:
			{
				bool reachedHQ = false;
				if ((int)m_CurrentTarget > (int)m_Path.size() - 3)
				{
					float fDistToHQSq = (m_TargetHQ->GetAbsolutePosition() - GetAbsolutePosition()).GetLengthSquared();
					reachedHQ = fDistToHQSq < m_AttackerSettings->m_fHQRange*m_AttackerSettings->m_fHQRange;
				}				
				if (reachedHQ)
				{
					m_ShootTarget = m_TargetHQ;
					m_eState = S_AttackingHQ;
					m_bTargetReached = true;

					auto hoverAnim = static_cast<Visitor*>(GetMeshEntity()->GetComponent("HoverAnim"));
					hoverAnim->Visit(GetMeshEntity());
					break;
				}

				if (!m_bTargetReached)
					MoveToTarget();
				else
				{
					// this is the second target, just steer to it
					SteerToTarget();
				}
				
				LookAtTarget();
			}
			break;

		case shoot::ShootingAttacker::S_AttackingHQ:
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
					SteerToTarget();
					AimAndShoot();
				}
			}
			break;
		}

		super::Update();
	}

	//! picks a target
	void ShootingAttacker::PickTarget()
	{
		m_ShootTarget = NULL;
		float fMinDistSq = Math::Maxf32;
		auto rangeSQ = m_AttackerSettings->m_Range*m_AttackerSettings->m_Range;
		for(auto &tower : PLAYFIELD->GetTowers())
		{
			if(!tower.IsValid() || !tower->IsEnabled() || tower->GetHitPoints() < 0.0f)			
				continue;

			Vector3 vPos = tower->GetAbsolutePosition();
			vPos.Z = GetAbsolutePosition().Z;
			float fDistSq = (vPos-GetAbsolutePosition()).GetLengthSquared();
			if(fDistSq < fMinDistSq && fDistSq < rangeSQ)
			{
				m_ShootTarget = tower;
				fMinDistSq = fDistSq;
			}
		}
	}

	void ShootingAttacker::AimAndShoot()
	{
		auto settings = DYNAMIC_CAST(m_Settings, ShootingAttackerSettings);

		// aim
		Vector3 vPosition = GetAbsolutePosition();
		Vector3 vAimDirection = (m_ShootTarget->GetAbsolutePosition() - vPosition).Normalize();		
		m_vLookAt = Math::Damp(m_vLookAt, vAimDirection, g_fDeltaTime, settings->m_fLookAtDuration);
		LookAt(m_vLookAt);

		// shoot
		if(m_fShootTimer < 0.0f)
		{
			Bullet::BulletParams params;
			Vector3 vBulletLocalPos = GetRotationMatrix().TransformVect(m_vBulletOffset);

			params.vPosition = vPosition + vBulletLocalPos;
			params.vDirection = vAimDirection;
			params.fSpeed = m_fBulletSpeed;
			params.fLife = m_fBulletLife;
			params.fRadius = m_fBulletSize/2.0f;
			params.damage = settings->m_fBulletDamage;

			SFXMGR->GetEnemyPulseManager()->AddBullet(params);
			AUDIOMGR->Play(AudioManager::S_EnemyPulse);

			m_fShootTimer = m_fBulletFrequency;
		}

		m_fShootTimer -= g_fDeltaTime;
	}	
}

