/* 

Amine Rehioui
Created: August 16th 2014

*/

#include "Game.h"

#include "BaseAttacker.h"
#include "WaveManager.h"
#include "Playfield.h"
#include "ProgressBar.h"
#include "RigidBodyComponent.h"
#include "HQCollisionComponent.h"
#include "GameManager.h"
#include "GameSettings.h"

namespace shoot
{
	DEFINE_OBJECT(BaseAttacker);

	//constructor
	BaseAttacker::BaseAttacker()
		: m_pInfo(NULL)
		, m_CurrentTarget(0)
		, m_vSpeed(Vector3::Zero)
		, m_vAccel(Vector3::Zero)
		, m_vTarget(Vector3::Zero)
		, m_vToTarget(Vector3::Zero)
		, m_bTargetReached(false)
		, m_FireDamageTimer(-1.0f)
		, m_AttackerSettings(NULL)
	{
	}

	//! destructor
	BaseAttacker::~BaseAttacker()
	{
	}

	//! called during the initialization of the entity
	void BaseAttacker::Init()
	{
		super::Init();

		SetUseRotationMatrix(true);
		
		if(GAMECONTEXT && m_pInfo)
		{
			m_AttackerSettings = DYNAMIC_CAST(m_Settings, AttackerSettings);
			m_TargetHQ = m_pInfo->m_Target;

			OnTerrainChanged();
			m_vLookAt = m_vToTarget = (m_vTarget-m_DummySteeringTileStart.m_vPosition).Normalize();
			
			m_vSpeed = (m_vLookAt * m_AttackerSettings->m_fMaxAccel).Limit(m_AttackerSettings->m_fMaxFlySpeed);
		}

		LookAt(m_vLookAt);
		m_HealthBG = DYNAMIC_CAST(GetChildByName("HealthBG"), Entity3D);
		m_HealthBar = DYNAMIC_CAST(GetChildByName("HealthBar"), ProgressBar);
	}

	//! called during the update of the entity
	void BaseAttacker::Update()
	{
		if (m_FireDamageTimer > 0.0f)
		{
			m_FireDamageTimer -= g_fDeltaTime;
			if (m_FireDamageTimer <= 0.0f)			
				m_FireDamage->SetActive(false);			
		}
	}

	void BaseAttacker::LookAt(const Vector3& vDirNorm)
	{
		SetRotationMatrix(Matrix44::MakeLookAtLH(Vector3::Zero, vDirNorm, Vector3::Up).GetTranspose());
	}

	void BaseAttacker::MoveToTarget()
	{
		Vector3 vDirection = m_vSpeed.Normalized();
		Vector3 vPos = GetAbsolutePosition();
		vPos.Z = 0.0f;
		Vector3 vFuturePos = vPos + vDirection* GSETTINGS->m_fSteerRayCastDist;
		Vector3 vSrcPos = static_cast<Tile*>(m_Path[m_CurrentTarget-1])->m_vPosition;
		Vector3 vDestPos = static_cast<Tile*>(m_Path[m_CurrentTarget])->m_vPosition;
		float fProjOnPath = Math::ProjectPointOnLine(vFuturePos, vSrcPos, vDestPos);
		fProjOnPath = Math::Clamp(fProjOnPath, 0.0f, fProjOnPath);
		if(fProjOnPath > (vDestPos-vSrcPos).GetLength())
		{
			if(m_CurrentTarget < m_Path.size()-1)
			{
				++m_CurrentTarget;
				m_vTarget = static_cast<Tile*>(m_Path[m_CurrentTarget])->m_vPosition + Vector3::Create(0.0f, 0.0f, m_pInfo->m_fZOffset);				
			}
		}

		m_vToTarget = (m_vTarget-GetAbsolutePosition()).Normalize();
		Vector3 vSteer = m_vToTarget*m_AttackerSettings->m_fSteerFactor;
		Vector3 vSeparation;
		Separate(vSeparation);
		m_vSpeed += (vSteer + vSeparation*m_AttackerSettings->m_fSeparationFactor).Limit(m_AttackerSettings->m_fMaxAccel) * g_fDeltaTime;
		m_vSpeed.Limit(m_AttackerSettings->m_fMaxFlySpeed);
		Translate(m_vSpeed * g_fDeltaTime);
	}

	void BaseAttacker::SteerToTarget()
	{
		Vector3 vSeparation;
		bool needSeparation = Separate(vSeparation);
		Vector3 vToHQ = m_TargetHQ->GetAbsolutePosition() - GetAbsolutePosition();
		float fDistToHQ = vToHQ.GetLength();
		Vector3 vSteer = Vector3::Zero;
		float fDeltaDist = fDistToHQ - m_AttackerSettings->m_fHQRange;
		if (Math::FAbs(fDeltaDist) > m_AttackerSettings->m_fHQTreshold)
			vSteer = (vToHQ / fDistToHQ)*Math::FSign(fDeltaDist)*m_AttackerSettings->m_fSteerFactor;

		if (!needSeparation)
			m_vSpeed = Math::Damp(m_vSpeed, Vector3::Zero, g_fDeltaTime, m_AttackerSettings->m_fDecelDuration);

		Vector3 vAcceleration = (vSteer + vSeparation*m_AttackerSettings->m_fSeparationFactor)*g_fDeltaTime;
		vAcceleration.Z = 0.0f;
		m_vSpeed += vAcceleration;
		m_vSpeed.Limit(m_AttackerSettings->m_fMaxFlySpeed);
		Translate(m_vSpeed * g_fDeltaTime);
	}

	void BaseAttacker::LookAtTarget()
	{
		float fSpeed = m_vSpeed.GetLength();
		if(fSpeed < GSETTINGS->m_fMinSpeedForLookAtChange)
			return;

		Vector3 vDirection = m_vSpeed / fSpeed;
		m_vLookAt = Math::Damp(m_vLookAt, vDirection, g_fDeltaTime, m_AttackerSettings->m_fLookAtDuration).Normalized();
		LookAt(m_vLookAt);
	}

	bool BaseAttacker::Separate(Vector3& vSeparationOut)
	{
		Vector3 vPos = GetAbsolutePosition();
		vPos.Z = 0.0f;
		Vector3 vDirection = m_vSpeed.Normalized();
		vSeparationOut = Vector3::Zero;
		bool needSeparation = false;

		std::list< Handle<BaseAttacker> >& attackers = WAVEMGR->GetAttackers();
		for(std::list< Handle<BaseAttacker> >::iterator it = attackers.begin(); it != attackers.end(); )
		{
			BaseAttacker* pAttacker = (*it).Get();
			if(!pAttacker)
			{
				it = attackers.erase(it);
				continue;
			}

			if(pAttacker == this || pAttacker->GetHitPoints() < 0.0f)
			{
				++it;
				continue;
			}
			
			Vector3 vOtherPos = pAttacker->GetAbsolutePosition();
			vOtherPos.Z = 0.0f;
			Vector3 vToOther = vOtherPos-vPos;
			float fDist = vToOther.GetLength();
			if (fDist < m_AttackerSettings->m_fSeparationDist && fDist > 0.0f)
			{
				Vector3 vToOtherDir = vToOther/fDist;
				float fFactor = m_AttackerSettings->m_fSeparationDist / fDist;
				vSeparationOut += -vToOtherDir*m_AttackerSettings->m_fMaxAccel*fFactor*fFactor;
				needSeparation = true;
			}
			++it;
		}

		return needSeparation;
	}
	
	//! called when the terran has changed
	void BaseAttacker::OnTerrainChanged()
	{
		if(m_bTargetReached)
			return;

		PLAYFIELD->GetShortestPath(GetAbsolutePosition(), m_TargetHQ->GetAbsolutePosition(), m_Path);

		m_DummySteeringTileStart.m_vPosition = GetAbsolutePosition();
		m_DummySteeringTileStart.m_vPosition.Z = 0.0f;
		m_Path.insert(m_Path.begin(), &m_DummySteeringTileStart);

		m_DummySteeringTileEnd.m_vPosition = m_TargetHQ->GetAbsolutePosition();
		m_DummySteeringTileEnd.m_vPosition.Z = 0.0f;
		m_Path.push_back(&m_DummySteeringTileEnd);

		m_CurrentTarget = 1;
		m_vTarget = static_cast<Tile*>(m_Path[m_CurrentTarget])->m_vPosition + Vector3::Create(0.0f, 0.0f, m_pInfo->m_fZOffset);
	}

	//! called when the actor has been shot
	bool BaseAttacker::OnShot(const Vector3& vPosition, CollisionComponent* pCollisionComponent, float damage)
	{
		m_HealthBG->SetVisible(true);
		float newHP = m_fHitPoints-damage;
		m_HealthBar->SetProgress(newHP / m_Settings->m_fHitPoints);

		if(super::OnShot(vPosition, pCollisionComponent, damage))
		{
			m_HealthBG->SetVisible(false);
			WAVEMGR->DecrAttackers();

			PLAYFIELD->ChangeDiamonds(m_AttackerSettings->m_Reward);
			return true;
		}
		return false;
	}

	//! OnFireDamage
	void BaseAttacker::OnFireDamage()
	{
		if (!m_FireDamage.IsValid())
		{
			m_FireDamage = static_cast<ParticleGenerator*>(SFXMGR->GetFireDamage()->Copy());
			AddChild(m_FireDamage);
		}

		m_FireDamageTimer = GSETTINGS->m_FireDamageDuration;
		m_FireDamage->SetActive(true);
	}
}

