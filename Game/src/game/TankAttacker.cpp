/* 

Amine Rehioui
Created: March 8th 2014

*/

#include "Game.h"

#include "TankAttacker.h"
#include "GameSettings.h"

#include "LaserComponent.h"
#include "CollisionUtils.h"
#include "Visitor.h"
#include "WaveManager.h"

namespace shoot
{
	DEFINE_OBJECT(TankAttacker);

	//! constructor
	TankAttacker::TankAttacker()
		: m_eState(S_Moving)
	{
	}

	//! destructor
	TankAttacker::~TankAttacker()
	{
	}

	//! serializes the entity to/from a PropertyStream
	void TankAttacker::Serialize(PropertyStream& stream)
	{
		super::Serialize(stream);
	}

	//! called during the initialization of the entity
	void TankAttacker::Init()
	{
		super::Init();

		m_Cannon = static_cast<Entity3D*>(GetChildByName("LaserCannon"));
		m_HitEntity = static_cast<Entity3D*>(GetChildByName("HitEntity"));
		m_LaserHalo = GetChildByName("LaserHalo");

		if (m_HitEntity.IsValid())
			m_HitEntity->SetVisible(false);

		if (m_LaserHalo.IsValid())
			m_LaserHalo->SetVisible(false);
	}

	//! called during the update of the entity
	void TankAttacker::Update()
	{
		if(m_fHitPoints < 0.0f)
		{
			if (m_Laser.IsValid())
				m_Laser->Deactivate();

			m_HitEntity->SetVisible(false);
			m_LaserHalo->SetVisible(false);
			return;
		}

		auto pSettings = DYNAMIC_CAST(m_Settings, TankAttackerSettings);
		Vector3 vHQPos = m_TargetHQ->GetAbsolutePosition();
		Vector3 vToHQ = vHQPos - m_Cannon->GetAbsolutePosition();
		float fToHQ = vToHQ.GetLength();

		switch (m_eState)
		{
		case S_Moving:
			{
				bool reachedHQ = false;
				if ((int)m_CurrentTarget > (int)m_Path.size() - 3)
				{
					reachedHQ = fToHQ < pSettings->m_fHQRange;
				}
				if (reachedHQ)
				{
					m_eState = S_AttackingHQ;

					if (m_TargetHQ->GetHitPoints() >= 0.0f)
					{
						// activate laser
						m_Laser = m_Cannon->GetComponent<LaserComponent>();
						m_Laser->Activate(vHQPos);
						m_LaserHalo->SetVisible(true);						
					}

					m_bTargetReached = true;
					auto hoverAnim = static_cast<Visitor*>(GetMeshEntity()->GetComponent("HoverAnim"));
					hoverAnim->Visit(GetMeshEntity());
					break;
				}

				if (!m_bTargetReached)
					MoveToTarget();
				else
					SteerToTarget();

				LookAtTarget();
			}
			break;

		case S_AttackingHQ:
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

					// deactivate laser
					if (m_Laser.IsValid())
						m_Laser->Deactivate();
					m_HitEntity->SetVisible(false);
					m_LaserHalo->SetVisible(false);
				}
				else
				{
					Vector3 vSeparation;
					bool needSeparation = Separate(vSeparation);
					Vector3 vSteer = Vector3::Zero;
					float fDeltaDist = fToHQ - pSettings->m_fHQRange;
					Vector3 vDir = vToHQ / fToHQ;
					if (Math::FAbs(fDeltaDist) > pSettings->m_fHQTreshold)
						vSteer = vDir*Math::FSign(fDeltaDist)*pSettings->m_fSteerFactor;

					if (!needSeparation)
						m_vSpeed = Math::Damp(m_vSpeed, Vector3::Zero, g_fDeltaTime, pSettings->m_fDecelDuration);

					Vector3 vAcceleration = (vSteer + vSeparation*pSettings->m_fSeparationFactor)*g_fDeltaTime;
					vAcceleration.Z = 0.0f;
					m_vSpeed += vAcceleration;
					m_vSpeed.Limit(pSettings->m_fMaxFlySpeed);
					Translate(m_vSpeed * g_fDeltaTime);

					// aim
					m_vLookAt = Math::Damp(m_vLookAt, vDir, g_fDeltaTime, pSettings->m_fLookAtDuration).Normalized();
					LookAt(m_vLookAt);

					// check laser collision
					RigidBodyComponent* pCollision = m_TargetHQ->GetCollisionComponent();
					float fClosestIntersection = Math::Maxf32;
					bool bCollision = false;
					for (auto &shape : pCollision->GetShapes())
					{
						SHOOT_ASSERT(shape->Get()->IsA(CollisionShapeSphere::TypeID), "Invalid HQ CollisionShape");
						Entity3D* pColOwner = static_cast<Entity3D*>(pCollision->GetOwner());
						CollisionShapeSphere* pCol = static_cast<CollisionShapeSphere*>(shape->Get());
						Vector3 vColCenter = pCol->GetCenter(pColOwner->GetWorldTransform());
						float fColRadius = pCol->GetRadius() * pColOwner->GetAbsoluteScale().X;
						float fToIntersection;
						if (CollisionUtils::SphereIntersectsWithRay(vColCenter, fColRadius, m_Cannon->GetAbsolutePosition(), vDir, NULL, &fToIntersection))
						{
							bCollision = true;
							if (fToIntersection < fClosestIntersection)
								fClosestIntersection = fToIntersection;
						}
					}

					if (bCollision)
					{
						if (m_Laser->GetLength() > fClosestIntersection - .1f)
						{
							Vector3 vEndPos = m_Cannon->GetAbsolutePosition() + vDir*fClosestIntersection;
							if (m_TargetHQ->OnDamage(pSettings->m_fLaserDamagePerSec * g_fDeltaTime))
								m_TargetHQ->GetChildByName("HQMesh")->GetComponent<RigidBodyComponent>()->SetEnabled(false);
							m_HitEntity->SetAbsolutePosition(vEndPos);
							m_HitEntity->SetVisible(true);
						}

						m_Laser->SetMaxLength(fClosestIntersection);
					}
				}	
			}
			break;
		}

		super::Update();
	}	
}

