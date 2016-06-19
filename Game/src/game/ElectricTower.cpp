/* 

Amine Rehioui
Created: April 24th 2014

*/

#include "Game.h"

#include "ElectricTower.h"
#include "WaveManager.h"
#include "ElectricityComponent.h"
#include "GameSettings.h"

namespace shoot
{
	DEFINE_OBJECT(ElectricTower);

	//! constructor
	ElectricTower::ElectricTower()
		// properties
		: m_DampingRange(0.3f, 1.5f)
	{
	}

	//! serializes the entity to/from a PropertyStream
	void ElectricTower::Serialize(PropertyStream& stream)
	{
		super::Serialize(stream);

		stream.Serialize("DampingRange", &m_DampingRange);
	}

	//! called during the initialization of the entity
	void ElectricTower::Init()
	{
		super::Init();

		m_Coils = static_cast<Entity3D*>(GetChildByName("Coils"));
		if(m_Coils.IsValid())
		{
			for (auto& coil : m_Coils->GetChildren())
			{
				SHOOT_ASSERT(coil->GetChildCount() > 1, "Coild paths should have more than 1 elements");
				TargetInfo info;
				info.m_Coil = coil.Get();
				info.m_Coil->SetVisibleAndEnabled(false);
				m_TargetInfos.push_back(info);
			}
		}

		m_GroundFlash = static_cast<Entity3D*>(GetChildByName("GroundFlash"));
		if(m_GroundFlash.IsValid())
			m_GroundFlash->SetVisibleAndEnabled(false);

		m_BeamHalo = static_cast<Entity3D*>(GetChildByName("BeamHalo"));
		if(m_BeamHalo.IsValid())
			m_BeamHalo->SetVisibleAndEnabled(false);

		for (auto& e : GetChildren())
			if (e->GetName() == "TeslaOverlay")
			{
				e->SetVisibleAndEnabled(false);
				m_TeslaOverlays.push_back(Handle<Entity>(e.Get()));
			}
	}

	//! called during the update of the entity
	void ElectricTower::Update()
	{
		super::Update();

		bool bActive = true;
		if(m_fHitPoints < 0.0f || !WAVEMGR)		
			bActive = false;

		auto settings = (ElectricTowerSettings*)m_Settings;
		int targetCount = 0;
		bool lookAtSet = false;

		for (auto& info : m_TargetInfos)
		{
			if (!info.m_Target.IsValid())
			{
				// pick target
				if (auto target = PickElectricTarget())
				{
					info.m_Target = target;

					// init coil
					Vector3 vToTarget = (target->GetAbsolutePosition() - m_Coils->GetAbsolutePosition());
					float fDistToTarget = vToTarget.GetLength();
					float fDistStep = fDistToTarget / (info.m_Coil->GetChildCount() - 1);
					vToTarget /= fDistToTarget;
					Vector3 vElemPos = m_Coils->GetAbsolutePosition();
					for (size_t i = 0; i < info.m_Coil->GetChildCount(); ++i)
					{
						Entity3D* pCoilElem = static_cast<Entity3D*>(info.m_Coil->GetChild(i));
						pCoilElem->SetAbsolutePosition(vElemPos);
						vElemPos += vToTarget*fDistStep;
					}

					info.m_Coil->SetVisibleAndEnabled(true);
					info.m_Coil->GetComponent<ElectricityComponent>()->Activate(true);
					++targetCount;
				}				
			}
			else
			{
				// check target
				Vector3 vPos = info.m_Target->GetAbsolutePosition();
				vPos.Z = GetAbsolutePosition().Z;
				float fDistSq = (vPos - GetAbsolutePosition()).GetLengthSquared();
				auto tooFar = (fDistSq > settings->m_Range*settings->m_Range + GSETTINGS->m_RangeCheckTreshold);
				if (tooFar || info.m_Target->GetHitPoints() < 0.0f)
				{
					// clear coil
					info.m_Target.Clear();
					info.m_Coil->SetVisibleAndEnabled(false);
					info.m_Coil->GetComponent<ElectricityComponent>()->Activate(false);
				}
				else
				{
					// update coil
					auto pCoil = info.m_Coil.Get();
					m_GroundFlash->SetVisibleAndEnabled(true);
					m_BeamHalo->SetVisibleAndEnabled(true);
					for (auto tesla : m_TeslaOverlays)
						tesla->SetVisibleAndEnabled(true);

					float fDampingFactor = m_DampingRange.Max;
					float fDampingStep = (m_DampingRange.Max - m_DampingRange.Min) / (pCoil->GetChildCount() - 1);
					Vector3 vToTarget = (info.m_Target->GetAbsolutePosition() - m_Coils->GetAbsolutePosition());
					float fDistToTarget = vToTarget.GetLength();
					float fDistStep = fDistToTarget / (pCoil->GetChildCount() - 1);
					vToTarget /= fDistToTarget;
					Vector3 vElemPos = m_Coils->GetAbsolutePosition();
					for (size_t i = 0; i < pCoil->GetChildCount(); ++i)
					{
						Entity3D* pCoilElem = static_cast<Entity3D*>(pCoil->GetChild(i));
						Vector3 vPosition = Math::Damp(pCoilElem->GetAbsolutePosition(), vElemPos, g_fDeltaTime, fDampingFactor);
						pCoilElem->SetAbsolutePosition(vPosition);
						fDampingFactor -= fDampingStep;
						vElemPos += vToTarget*fDistStep;
					}

					// apply damage
					Entity3D* pCoilElem = static_cast<Entity3D*>(pCoil->GetChild(pCoil->GetChildCount() - 1));
					Vector3 vCoilEndPos = pCoilElem->GetAbsolutePosition();
					if (info.m_Target->OnShot(vCoilEndPos, NULL, g_fDeltaTime*settings->m_fDamagePerSecond))
						info.m_Target->GetMeshEntity()->GetComponent<CollisionComponent>()->SetEnabled(false);

					AUDIOMGR->Play(AudioManager::S_Electricity, false, true);
					++targetCount;

					if (!lookAtSet)
					{
						m_vLookAt = vToTarget;
						lookAtSet = true;
					}
				}
			}
		}

		if (targetCount == 0)
		{
			m_GroundFlash->SetVisibleAndEnabled(false);
			m_BeamHalo->SetVisibleAndEnabled(false);
			for (auto tesla : m_TeslaOverlays)
				tesla->SetVisibleAndEnabled(false);
			for (auto& info : m_TargetInfos)
				info.m_Coil->SetVisibleAndEnabled(false);
		}				
	}

	//! picks a target
	Actor* ElectricTower::PickElectricTarget()
	{
		Actor* target = NULL;
		auto settings = (ElectricTowerSettings*)m_Settings;
		float fMinDistSq = Math::Maxf32;
		auto rangeSQ = settings->m_Range*settings->m_Range;
		for (auto& attacker : WAVEMGR->GetAttackers())
		{
			BaseAttacker* pAttacker = attacker.Get();
			if (!pAttacker || pAttacker->GetHitPoints() < 0.0f || IsAlreadyTargeted(pAttacker))
				continue;

			Vector3 vPos = pAttacker->GetAbsolutePosition();
			vPos.Z = GetAbsolutePosition().Z;
			float fDistSq = (vPos - GetAbsolutePosition()).GetLengthSquared();
			if (fDistSq < fMinDistSq && fDistSq < rangeSQ)
			{
				target = pAttacker;
				fMinDistSq = fDistSq;
			}
		}
		return target;
	}

	//! IsAlreadyTargeted
	bool ElectricTower::IsAlreadyTargeted(Actor* actor) const
	{
		for (auto& info : m_TargetInfos)
		{
			if (info.m_Target.Get() == actor)
				return true;
		}
		return false;
	}
}

