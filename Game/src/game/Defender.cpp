/* 

Amine Rehioui
Created: February 18th 2014

*/

#include "Game.h"

#include "Defender.h"
#include "Quaternion.h"
#include "WaveManager.h"
#include "Tile.h"

#include "HQ.h"
#include "CollisionUtils.h"
#include "Playfield.h"
#include "ProgressBar.h"
#include "Text.h"
#include "Gamepad.h"
#include "GameCamera.h"
#include "FollowCamera.h"
#include "GameSettings.h"
#include "RigidBodyComponent.h"
#include "AudioManager.h"
#include "Basis3.h"
#include "DefenderManager.h"

namespace shoot
{
	DEFINE_OBJECT(Defender);

	//! Type literals
	const char* const g_strTypeLiterals[] =
	{
		"DT_Pulse",
		"DT_Laser",
		"DT_Pellet",
		"DT_Bomber",
		0
	};

	//! constructor
	Defender::Defender()
		: m_eState(S_Idle)
		, m_vCurrentSpeed(Vector3::Zero)
		, m_vSpeed(Vector3::Zero)
		, m_CurrentCannon(0)
		, m_vAccel(Vector3::Zero)
		, m_ImpactTimer(-1.0f)
		// properties
		, m_fShootAngle(30.0f)
		, m_fDamagePerSecond(60.0f)
		, m_eDefenderType(DT_Pulse)
	{
	}

	//! serializes the entity to/from a PropertyStream
	void Defender::Serialize(PropertyStream& stream)
	{
		super::Serialize(stream);

		stream.Serialize("ShootAngle", &m_fShootAngle);
		stream.Serialize("DamagePerSecond", &m_fDamagePerSecond);
		stream.Serialize("DefenderType", &m_eDefenderType, ENUM_PARAMS1(g_strTypeLiterals));
	}

	//! called during the initialization of the entity
	void Defender::Init()
	{
		super::Init();		

		SetUseRotationMatrix(true);
		LookAt(m_vLookAt);

		m_LeftCannon = DYNAMIC_CAST(GetChildByName("LeftCannon"), Entity3D);
		m_RightCannon = DYNAMIC_CAST(GetChildByName("RightCannon"), Entity3D);

		if (m_LeftCannon.IsValid())
		{
			m_MuzzleFlashLeft = m_LeftCannon->GetChildByName("MuzzleFlash");
			m_RecoilLeft = static_cast<ParticleGenerator*>(m_LeftCannon->GetChildByName("Recoil"));
			if (m_MuzzleFlashLeft.IsValid())
				m_MuzzleFlashLeft->SetVisibleAndEnabled(false);
			if (m_RecoilLeft.IsValid())
				m_RecoilLeft->SetActive(false);
		}

		if (m_RightCannon.IsValid())
		{
			m_MuzzleFlashRight = m_RightCannon->GetChildByName("MuzzleFlash");
			m_RecoilRight = static_cast<ParticleGenerator*>(m_RightCannon->GetChildByName("Recoil"));
			if (m_MuzzleFlashRight.IsValid())
				m_MuzzleFlashRight->SetVisibleAndEnabled(false);
			if (m_RecoilRight.IsValid())
				m_RecoilRight->SetActive(false);
		}		
	}

	//! called during the update of the entity
	void Defender::Update()
	{
		if(m_fHitPoints < 0.0f || !GameEngine::Instance()->GetGameContext())
			return;

		switch(m_eState)
		{
		case S_Hover:			
			{
				auto direction = SteerTo(WAVEMGR->GetAverageAttackerPos());
				m_MuzzleFlashLeft->SetVisibleAndEnabled(false);
				m_MuzzleFlashRight->SetVisibleAndEnabled(false);
				m_RecoilLeft->SetActive(false);
				m_RecoilRight->SetActive(false);

				// shoot if attacker in front
				auto& attackers = WAVEMGR->GetAttackers();
				for(auto& attacker : attackers)
				{
					if(!attacker.IsValid() || attacker->GetHitPoints() < 0.0f)
						continue;
					
					auto toTarget = (attacker->GetAbsolutePosition()-GetAbsolutePosition());
					auto toTargetN = toTarget.Normalized();
					if((direction.DotProduct(toTargetN) > Math::Cos(m_fShootAngle*Math::DegToRadFactor))
					&& toTarget.GetLength() < GetSettings()->m_Range)
					{
						Vector3 vBulletPos = m_CurrentCannon ? m_LeftCannon->GetAbsolutePosition() : m_RightCannon->GetAbsolutePosition();
						m_CurrentCannon = 1 - m_CurrentCannon;

						m_MuzzleFlashLeft->SetVisibleAndEnabled(true);
						m_MuzzleFlashRight->SetVisibleAndEnabled(true);

						auto mesh = attacker->GetMeshEntity();
						auto rigidBody = mesh->GetComponent<RigidBodyComponent>();
						auto sphereCol = DYNAMIC_CAST(rigidBody->GetShapes()[0].Get(), CollisionShapeSphere);
						Vector3 vColCenter = sphereCol->GetCenter(mesh->GetWorldTransform());
						float fColRadius = sphereCol->GetRadius() * mesh->GetAbsoluteScale().X;
						float toIntersection = 0.0f;
						if (CollisionUtils::SphereIntersectsWithRay(vColCenter, fColRadius, GetAbsolutePosition(), toTargetN, NULL, &toIntersection))
						{
							auto intersection = GetAbsolutePosition() + toTargetN*toIntersection;

							if (m_ImpactTimer < 0.0f)
								m_ImpactTimer = SFXMGR->OnBulletImpact(intersection, this);
							
							if (attacker->OnShot(intersection, NULL, g_fDeltaTime*m_fDamagePerSecond))
								rigidBody->SetEnabled(false);
						}

						if (toIntersection > GSETTINGS->m_DefenderImpactEffectMinDist)
						{
							m_RecoilLeft->SetActive(true);
							m_RecoilRight->SetActive(true);
						}

						AUDIOMGR->Play(AudioManager::S_MachineGun, false, true);						
						break;
					}
				}
			}
			break;

		case S_PullBack:		
			SteerTo(m_PullBackTarget);		
		break;
		}

		m_ImpactTimer -= g_fDeltaTime;
	}

	//! returns collision status
	bool Defender::CollidesWith(const Vector3& vRayStart, const Vector3& vRayDir)
	{
		auto sphere = static_cast<CollisionShapeSphere*>(GetMeshEntity()->GetComponent<RigidBodyComponent>()->GetShapes().first()->Get());
		Vector3 vColPos = sphere->GetCenter(GetMeshEntity()->GetWorldTransform());
		float fColRadius = (GetMeshEntity()->GetAbsoluteScale()*sphere->GetRadius()).X;
		return CollisionUtils::SphereIntersectsWithRay(vColPos, fColRadius, vRayStart, vRayDir);
	}

	void Defender::SwitchState(E_State eState)
	{
		if(eState == m_eState)
			return;

		m_eState = eState;
		switch (eState)
		{
		case S_Hover:
		{
			m_vAccel = m_vLookAt * GSETTINGS->m_DefenderSettings->m_fMaxAccel;
			m_vSpeed = m_vCurrentSpeed = m_vAccel;
			
		}
		break;

		case S_PullBack:
		{
			auto pullBackDir = Vector3::Create(GetAbsolutePosition().XY, 0.0f).Normalize();
			m_PullBackTarget = GetAbsolutePosition() + pullBackDir*9999.0f;

			m_MuzzleFlashLeft->SetVisibleAndEnabled(false);
			m_MuzzleFlashRight->SetVisibleAndEnabled(false);
			m_RecoilLeft->SetActive(false);
			m_RecoilRight->SetActive(false);
		}
		break;
		}
	}	
	
	bool Defender::PickAttacker()
	{
		m_Target = NULL;
		std::list< Handle<BaseAttacker> >& attackers = WAVEMGR->GetAttackers();
		for(std::list< Handle<BaseAttacker> >::iterator it = attackers.begin(); it != attackers.end(); ++it)
		{
			BaseAttacker* pAttacker = (*it).Get();
			if(pAttacker && !(pAttacker->GetHitPoints() < 0.0f))
			{
				Vector3 vToAttacker = pAttacker->GetAbsolutePosition()-GetAbsolutePosition();
				vToAttacker.Z = 0.0f;
				float fDistSq = vToAttacker.GetLengthSquared();
				if(fDistSq < GetSettings()->m_Range*GetSettings()->m_Range)
				{
					m_Target = pAttacker;
					break;
				}
			}
		}

		return m_Target.IsValid();
	}	

	//! called when the actor has been shot
	bool Defender::OnShot(const Vector3& vPosition, CollisionComponent* pCollisionComponent, float damage)
	{
		return super::OnShot(vPosition, pCollisionComponent, damage);
	}

	void Defender::LookAt(const Vector3& vDirNorm)
	{
		SetRotationMatrix(Matrix44::MakeLookAtLH(Vector3::Zero, vDirNorm, Vector3::Up).GetTranspose());
		m_vLookAt = vDirNorm;
	}

	Vector3 Defender::Separate()
	{
		auto separation = Vector3::Zero;
		auto& defenders = DEFENDERMGR->GetDefenders();
		for(auto& defender : defenders)
		{
			if(defender.Get() == this || !defender.IsValid())
				continue;

			auto sphere = static_cast<CollisionShapeSphere*>(defender->GetMeshEntity()->GetComponent<RigidBodyComponent>()->GetShapes().first()->Get());
			auto colPos = sphere->GetCenter(defender->GetMeshEntity()->GetWorldTransform());
			float colRadius = (defender->GetMeshEntity()->GetAbsoluteScale()*sphere->GetRadius()).X;
			auto lateralAxe = Vector3::Up.CrossProduct(m_vLookAt);
			float toObstacleLeft = 0.0f;
			float toObstacleRight = 0.0f;
			auto colStartLeft = GetAbsolutePosition()-lateralAxe*GSETTINGS->m_fAvoidanceLateralRadius;
			auto colStartRight = GetAbsolutePosition()+lateralAxe*GSETTINGS->m_fAvoidanceLateralRadius;
			auto colLeft = CollisionUtils::SphereIntersectsWithRay(colPos, colRadius, colStartLeft, m_vLookAt, &GSETTINGS->m_fAvoidanceDist, &toObstacleLeft);
			auto colRight = CollisionUtils::SphereIntersectsWithRay(colPos, colRadius, colStartRight, m_vLookAt, &GSETTINGS->m_fAvoidanceDist, &toObstacleRight);			
			//if(CollisionUtils::SphereIntersectsWithRay(colPos, colRadius, GetAbsolutePosition(), m_vLookAt, &GSETTINGS->fAvoidanceDist, &toObstacle))
			if(colLeft || colRight)
			{
				//auto toObstacle = colLeft ? toObstacleLeft : toObstacleRight;
				//auto colStart = colLeft ? colStartLeft : colStartRight;
				//colPos = colStart + m_vLookAt*toObstacle;
				
				//Vector3 vObjToIntersection = (colPos-defender->GetAbsolutePosition()).Normalize();
				float fDirection = colLeft ? 1.0f : -1.0f; //Math::FSign(vObjToIntersection.DotProduct(lateralAxe));
				separation += lateralAxe*fDirection;
			}

			//Vector3 vOtherPos = defender->GetAbsolutePosition();
			//Vector3 vToOther = vOtherPos-vPos;
			//float fDist = vToOther.GetLength();
			//if(fDist < GSETTINGS->DefenderSettings->m_fSeparationDist && fDist > 0.0f)
			//{
			//	Vector3 vToOtherDir = vToOther/fDist;
			//	float fFactor = GSETTINGS->DefenderSettings->m_fSeparationDist/fDist;
			//	separation += -vToOtherDir*GSETTINGS->DefenderSettings->m_fMaxAccel*fFactor*fFactor;
			//}
		}

		return separation;
	}	

	Vector3 Defender::SteerTo(const Vector3& target)
	{
		auto& settings = GSETTINGS->m_DefenderSettings;
		auto steerFactor = settings->m_fSteerFactor;
		auto maxAccel = settings->m_fMaxAccel;
		auto maxSpeed = settings->m_fMaxFlySpeed;
		if (m_eState == S_PullBack)
		{
			steerFactor = settings->m_fPullbackSteerFactor;
			maxAccel = settings->m_fPullbackAccel;
			maxSpeed = settings->m_fPullbackSpeed;
		}

		auto direction = m_vSpeed.Normalized();
		auto steer = (target - GetAbsolutePosition()).Normalize();
		auto separation = Separate();
		auto accel = direction*settings->m_fMinAccelFactor + steer*steerFactor + separation*settings->m_fSeparationFactor;
		accel.Z *= GSETTINGS->m_fAccelZFactor;
		accel = accel.Limit(maxAccel);
		m_vSpeed += accel * g_fDeltaTime;
		m_vSpeed.Limit(maxSpeed);
		Translate(m_vSpeed * g_fDeltaTime);
		m_vLookAt = Math::Damp(m_vLookAt, direction, g_fDeltaTime, settings->m_fLookAtDuration);
		LookAt(m_vLookAt.Normalize());
		return direction;
	}	
}

