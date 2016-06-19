/* 

Amine Rehioui
Created: May the 4th be with you 2014

*/

#include "Game.h"

#include "NuclearTower.h"
#include "Path.h"
#include "FollowPathVisitor.h"
#include "Rotation3DVisitor.h"
#include "GameContext.h"
#include "WaveManager.h"
#include "Playfield.h"
#include "EventManager.h"
#include "SequenceVisitor.h"
#include "ProgressIndicator.h"
#include "FollowCamera.h"

namespace shoot
{
	DEFINE_OBJECT(NuclearTower);

	//! constructor
	NuclearTower::NuclearTower()
		: m_bActive(false)
		, m_fTimer(0.0f)
		, m_eState(S_PreLaunch)
		, m_CanManuallyLaunch(true)
	{
	}

	//! destructor
	NuclearTower::~NuclearTower()
	{
		if (IsInitialized())
			EventManager::Instance()->UnregisterListener<TriggerEvent>(this);
	}

	//! serializes the entity to/from a PropertyStream
	void NuclearTower::Serialize(PropertyStream& stream)
	{
		super::Serialize(stream);

		stream.Serialize("Missile", &m_Missile);
		stream.Serialize("Explosion", &m_Explosion);
	}

	//! called during the initialization of the entity
	void NuclearTower::Init()
	{
		super::Init();		

		if (auto settings = (NuclearTowerSettings*)m_Settings)
			m_LaunchDelay = settings->m_fLaunchDelay;

		m_ProgressIndicator = static_cast<ProgressIndicator*>(GetChildByName("ProgressIndicator"));
		if (m_ProgressIndicator.IsValid())
		{
			m_ProgressIndicator->SetRemoveOnCompletion(false);
			m_ProgressIndicator->SetDuration(m_LaunchDelay);
		}

		EventManager::Instance()->RegisterListener<TriggerEvent>(this, (EventManager::EventHandler)&NuclearTower::HandleTriggerEvent);
	}

	//! called during the update of the entity
	void NuclearTower::Update()
	{
		auto settings = (NuclearTowerSettings*)m_Settings;

		switch (m_eState)
		{
		case S_PreLaunch:
			m_fTimer += g_fDeltaTime;
			if (m_fTimer > m_LaunchDelay)
			{
				// open the hatch
				auto leftLid = GetChildByName("LidLeft");
				auto rightLid = GetChildByName("LidRight");
				auto leftRotator = static_cast<Rotation3DVisitor*>(leftLid->GetComponent("Open"));
				auto rightRotator = static_cast<Rotation3DVisitor*>(rightLid->GetComponent("Open"));
				leftRotator->Visit(leftLid);
				rightRotator->Visit(rightLid);

				// launch missile
				m_CurrentMissile = static_cast<Entity3D*>(m_Missile->Copy());
				auto path = static_cast<Path*>(GetChildByType("Path"));
				auto sequencer = m_CurrentMissile->GetComponent<SequenceVisitor>();
				auto follower = DYNAMIC_CAST(sequencer->GetVisitor(0), FollowPathVisitor);
				follower->SetPath(path);
				m_CurrentMissile->GetComponent<CollisionComponent>()->SetEnabled(false);
				AddChild(m_CurrentMissile);
				m_CurrentMissile->Init();
				m_eState = S_Ascending;

				m_MissileDirection = Vector3::Up.DifferentiateFromUp();
				AUDIOMGR->Play(AudioManager::S_MissileLaunch);

				FOLLOWCAM->OnMissileLaunched(m_CurrentMissile);
			}
			break;

		case S_Striking:
			{
				auto missilePos = Math::Lerp(m_StrikeSrcPos, m_StrikeTargetPos, m_Interpolator);				
				m_CurrentMissile->SetAbsolutePosition(missilePos);
				m_Interpolator += g_fDeltaTime / settings->m_StrikeDuration;

				m_MissileDirection = Math::Damp(m_MissileDirection, m_MissileTargetDirection, g_fDeltaTime, settings->m_LookAtDampDuration);
				m_CurrentMissile->SetRotationMatrix(Matrix44::MakeLookAtLH(Vector3::Zero, m_MissileDirection, Vector3::Up).GetTranspose());
			}
			break;
		}
	}

	//! called when a missile has landed
	void NuclearTower::OnMissileLanded(const Vector3& vPosition)
	{
		auto explosion = static_cast<Entity3D*>(m_Explosion->Copy());
		explosion->SetPosition(vPosition);
		GetRoot()->AddChild(explosion);
		explosion->Init();
		RemoveChild(GetChildByName("Missile"));

		auto settings = (NuclearTowerSettings*)m_Settings;

		// blow up shit
		auto radius = settings->m_ExplosionRadius;
		auto radiusSq = radius*radius;
		auto attackers = WAVEMGR->GetAttackers();
		SFXMGR->SetActorExplosionsEnabled(false);
		int index = 0;
		for (auto attacker : attackers)
		{
			if (!attacker.IsValid())
				continue;

			if ((attacker->GetAbsolutePosition() - vPosition).GetLengthSquared() < radiusSq)
			{
				if (!(attacker->GetHitPoints() < 0.0f))
				{
					if (index % 2 == 0)
						attacker->OnShot(attacker->GetAbsolutePosition(), NULL, 9999.0f);
					else
					{
						attacker->GetParent()->RemoveChild(attacker);
						WAVEMGR->DecrAttackers();
					}

					attacker->GetMeshEntity()->GetComponent<CollisionComponent>()->SetEnabled(false);
					++index;
				}
			}
		}
		SFXMGR->SetActorExplosionsEnabled(true);
		AUDIOMGR->Play(AudioManager::S_ExplosionNuclear);

		m_LaunchDelay = settings->m_fLaunchDelay;
		m_fTimer = 0.0f;
		m_ProgressIndicator->StartProgress(m_LaunchDelay);
		m_eState = S_PreLaunch;
		m_CanManuallyLaunch = true;
	}

	//! Nuke
	void NuclearTower::Nuke()
	{
		m_MissileDirection = Vector3::Up.DifferentiateFromUp();		
		m_eState = S_PreLaunch;

		auto settings = (NuclearTowerSettings*)m_Settings;
		auto factor = m_fTimer / m_LaunchDelay;
		m_LaunchDelay = settings->m_fAcceleratedLaunchDelay;
		m_fTimer = m_LaunchDelay * factor;
		m_ProgressIndicator->UpdateDuration(m_LaunchDelay);
		m_CanManuallyLaunch = false;
	}

	//! HandleTriggerEvent
	void NuclearTower::HandleTriggerEvent(Event* pEvent)
	{
		auto trigger = static_cast<TriggerEvent*>(pEvent);
		if (trigger->Name == "NukeReachedTop")
		{
			auto missile = GetChildByName("Missile");
			if (!missile || trigger->Target.Get() != missile)
				return;

			missile->GetComponent<CollisionComponent>()->SetEnabled(true);

			// close the hatch
			auto leftLid = GetChildByName("LidLeft");
			auto rightLid = GetChildByName("LidRight");
			auto leftRotator = static_cast<Rotation3DVisitor*>(leftLid->GetComponent("Close"));
			auto rightRotator = static_cast<Rotation3DVisitor*>(rightLid->GetComponent("Close"));
			leftRotator->Visit(leftLid);
			rightRotator->Visit(rightLid);

			// determine target
			m_StrikeTargetPos = WAVEMGR->GetAverageAttackerPos();
			auto collision = PLAYFIELD->GetCollisionBox() * 0.9f;
			m_StrikeTargetPos.X = Math::Clamp(m_StrikeTargetPos.X, collision.Min().X, collision.Max().X);
			m_StrikeTargetPos.Y = Math::Clamp(m_StrikeTargetPos.Y, collision.Min().Y, collision.Max().Y);
			m_StrikeTargetPos.Z = 0.0f;

			m_StrikeSrcPos = m_CurrentMissile->GetAbsolutePosition();
			m_MissileTargetDirection = (m_StrikeTargetPos - m_StrikeSrcPos).Normalized();
			m_Interpolator = 0.0f;
			m_eState = S_Striking;
		}
	}
}

