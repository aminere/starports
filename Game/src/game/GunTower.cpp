/* 

Amine Rehioui
Created: April 12th 2014

*/

#include "Game.h"

#include "GunTower.h"
#include "WaveManager.h"
#include "AudioManager.h"
#include "ParticleGenerator.h"
#include "Visitor.h"
#include "CollisionUtils.h"
#include "GameSettings.h"

namespace shoot
{
	DEFINE_OBJECT(GunTower);

	//! constructor
	GunTower::GunTower()		
		: m_ImpactTimer(-1.0f)
		, m_State(S_Locking)
		, m_LockTimer(0.0f)
		, m_CurrentMuzzleFlash(-1)
		, m_MuzzleTimer(-1.0f)
		// properties		
		, m_fShowRecoilThreshold(0.1f)
	{
		m_vLookAt = Vector3::Create(0.0f, 1.0f, 0.0f);
	}

	//! serializes the entity to/from a PropertyStream
	void GunTower::Serialize(PropertyStream& stream)
	{
		super::Serialize(stream);
		
		stream.Serialize("ShowRecoilThreshold", &m_fShowRecoilThreshold);
	}

	//! called during the initialization of the entity
	void GunTower::Init()
	{
		super::Init();

		m_Core = static_cast<Entity3D*>(GetChildByName("Core_orient"));
		if(m_Core.IsValid())
		{
			m_Core->SetUseRotationMatrix(true);
			LookAt(m_vLookAt);

			for (auto& child : m_Core->GetChildren())
			{
				if (child->GetName() == "MuzzleFlash")
				{
					auto muzzleFlash = static_cast<Entity3D*>(child.Get());
					muzzleFlash->SetVisibleAndEnabled(false);
					m_MuzzleFlashes.push_back(muzzleFlash);
				}
			}

			m_Particles = static_cast<ParticleGenerator*>(m_Core->GetChildByName("Recoil"));
			if (m_Particles.IsValid())
				m_Particles->SetActive(false);

			m_RecoilAnim = m_Core->GetComponent<Visitor>();
		}		
		
		m_GroundFlash = static_cast<Entity3D*>(GetChildByName("GroundFlash"));
		if(m_GroundFlash.IsValid())
			m_GroundFlash->SetVisibleAndEnabled(false);
	}

	//! called during the update of the entity
	void GunTower::Update()
	{
		super::Update();
		
		auto previousTarget = m_Target.Get();

		if (m_fHitPoints < 0.0f
		|| !CheckTarget())
		{
			m_GroundFlash->SetVisibleAndEnabled(false);

			for(auto& flash : m_MuzzleFlashes)
				flash->SetVisibleAndEnabled(false);

			m_Particles->SetActive(false);

			if (m_RecoilAnim->IsActive())
				m_RecoilAnim->Leave();

			m_LockTimer = 0.0f;
			m_State = S_Locking;
			return;
		}

		if (previousTarget && m_Target.Get() != previousTarget)
		{
			m_LockTimer = 0.0f;
			m_State = S_Locking;
		}

		auto vPosition = m_Core->GetAbsolutePosition();
		auto toTarget = (m_Target->GetAbsolutePosition() - vPosition);
		auto toTargetLength = toTarget.GetLength();
		auto vDirection = toTarget / toTargetLength;
		auto settings = (TowerSettings*)m_Settings;

		switch (m_State)
		{
		case S_Locking:
			{			
				m_vLookAt = Math::Damp(m_vLookAt, vDirection, g_fDeltaTime, settings->m_fAimDuration).Normalized();
				LookAt(m_vLookAt);

				m_LockTimer += g_fDeltaTime;
				if (m_LockTimer > settings->m_fAimDuration)
					m_State = S_Shooting;
			}
			break;

		case S_Shooting:
			{
				LookAt(vDirection);

				if (m_Target->OnShot(m_Target->GetAbsolutePosition(), NULL, g_fDeltaTime*settings->m_fDamagePerSecond))
					m_Target->GetMeshEntity()->GetComponent<CollisionComponent>()->SetEnabled(false);

				auto mesh = m_Target->GetMeshEntity();
				auto rigidBody = mesh->GetComponent<RigidBodyComponent>();
				auto sphereCol = DYNAMIC_CAST(rigidBody->GetShapes()[0].Get(), CollisionShapeSphere);
				Vector3 vColCenter = sphereCol->GetCenter(mesh->GetWorldTransform());
				float fColRadius = sphereCol->GetRadius() * mesh->GetAbsoluteScale().X;
				float toIntersection = 0.0f;
				if (CollisionUtils::SphereIntersectsWithRay(vColCenter, fColRadius, vPosition, vDirection, NULL, &toIntersection))
				{
					auto intersection = vPosition + vDirection*toIntersection;
					if (m_ImpactTimer < 0.0f)
						m_ImpactTimer = SFXMGR->OnBulletImpact(intersection, this);
				}

				auto particlesPos = m_Particles->Entity3D::GetWorldTransform().GetTranslation();
				m_GroundFlash->SetAbsolutePosition(Vector3::Create(particlesPos.XY, GetPosition().Z));
				m_GroundFlash->SetVisibleAndEnabled(true);

				if (m_CurrentMuzzleFlash < 0)
					m_CurrentMuzzleFlash = (int)Random::GetInt(0, (uint)m_MuzzleFlashes.size() - 1);
				m_MuzzleFlashes[m_CurrentMuzzleFlash]->SetVisibleAndEnabled(true);

				m_Particles->SetActive(toTargetLength < GSETTINGS->m_GunTowerRecoilDist);

				if (!m_RecoilAnim->IsActive())
					m_RecoilAnim->Visit(m_Core);

				m_MuzzleTimer -= g_fDeltaTime;
				if (m_MuzzleTimer < 0.0f)
				{
					m_MuzzleTimer = Random::GetFloat(0.0f, GSETTINGS->m_MuzzleFlashDuration);
					m_MuzzleFlashes[m_CurrentMuzzleFlash]->SetVisibleAndEnabled(false);
					m_CurrentMuzzleFlash = -1;
				}

				AUDIOMGR->Play(AudioManager::S_MachineGun, false, true);
			}
			break;
		}		

		m_ImpactTimer -= g_fDeltaTime;
	}

	//! Looks in a direction
	void GunTower::LookAt(const Vector3& vDirection)
	{
		m_Core->SetRotationMatrix(Matrix44::MakeLookAtLH(Vector3::Zero, vDirection, Vector3::Up).GetTranspose());
		m_vLookAt = vDirection;
	}
}

