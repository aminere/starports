/* 

Amine Rehioui
Created: March 31st 2014

*/

#include "Game.h"

#include "FollowCamera.h"

#include "GameCamera.h"
#include "Defender.h"
#include "Basis3.h"

#include "DefenderManager.h"
#include "Playfield.h"

namespace shoot
{
	DEFINE_OBJECT(FollowCamera);

	//! constructor
	FollowCamera::FollowCamera()
		: m_MissileView(false)
		// properties
		, m_fFollowDuration(0.5f)
		, m_fPosBackOffset(1.0f)
		, m_fPosHeightOffset(2.5f)
		, m_fLookAtOffset(5.0f)
		, m_fMissileFollowDuration(0.5f)
		, m_fMissilePosBackOffset(1.0f)
		, m_fMissilePosHeightOffset(2.5f)
		, m_fMissileLookAtOffset(5.0f)
		, m_fMissilePosBackOffsetDescent(4.0f)
	{
		m_bSet = true;
	}

	//! destructor
	FollowCamera::~FollowCamera()
	{
	}

	//! serializes the entity to/from a PropertyStream
	void FollowCamera::Serialize(PropertyStream& stream)
	{
		super::Serialize(stream);

		stream.Serialize("FollowDuration", &m_fFollowDuration);
		stream.Serialize("PosBackOffset", &m_fPosBackOffset);
		stream.Serialize("PosHeightOffset", &m_fPosHeightOffset);
		stream.Serialize("LookAtOffset", &m_fLookAtOffset);
		stream.Serialize("MissileFollowDuration", &m_fMissileFollowDuration);
		stream.Serialize("MissilePosBackOffset", &m_fMissilePosBackOffset);
		stream.Serialize("MissilePosHeightOffset", &m_fMissilePosHeightOffset);
		stream.Serialize("MissileLookAtOffset", &m_fMissileLookAtOffset);
		stream.Serialize("MissilePosBackOffsetDescent", &m_fMissilePosBackOffsetDescent);		
	}

	//! called during the initialization of the entity
	void FollowCamera::Init()
	{
		super::Init();

		m_fTargetFOV = m_FOV;
		SetEnabled(false);

		// Init FPS UI
		auto uiContext = Engine::Instance()->GetTopContext();
		auto fpsButton = uiContext->GetChildByName("FPSMode");
		auto normalButton = uiContext->GetChildByName("NormalMode");
		if (fpsButton && normalButton)
		{
			if (GAMEMGR->GetGameData()->HasProduct(GameData::PT_FPSMode))
			{
				fpsButton->SetVisible(true);
				normalButton->SetVisible(false);
			}
			else
			{
				fpsButton->SetVisible(false);
				normalButton->SetVisible(false);
			}
		}
	}

	//! called during the update of the entity
	void FollowCamera::Update()
	{
		if(!m_Target.IsValid() || m_Target->GetHitPoints() < 0.0f)
		{
			m_Target = PickTarget();
			if (!m_Target.IsValid() && !m_MissileView)
			{
				OnFPSModeChanged(false);
				return;
			}
		}

		if (InputManager::Instance()->IsTouchJustReleased()
		&& GAMEMGR->GetState() == GameManager::S_InGame
		&& !m_MissileView)
		{
			// cycle targets
			auto& defenders = DEFENDERMGR->GetDefenders();
			auto& towers = PLAYFIELD->GetTowers();
			if (m_FollowingDefenders)
			{
				m_TargetIndex = (m_TargetIndex + 1) % defenders.size();
				int idx = 0;
				for (auto& defender : defenders)
				{
					if (idx >= m_TargetIndex && defender.Get())
					{
						SetTarget(defender.Get());
						break;
					}
					++idx;
				}
			}			
			else
			{
				m_TargetIndex = (m_TargetIndex + 1) % towers.size();
				int idx = 0;
				for (auto& tower : towers)
				{
					if (idx >= m_TargetIndex && tower.Get())
					{
						SetTarget(tower.Get());
						break;
					}
					++idx;
				}
			}

			if (!m_Target.IsValid())
			{
				OnFPSModeChanged(false);
				return;
			}
		}

		if (m_MissileView)
		{
			if (!m_Missile.IsValid())
			{
				m_Missile = PickMissile();
				if (m_Missile.IsValid())
					m_PreviousMissilePos = m_Missile->GetAbsolutePosition();
				else
				{
					OnFPSModeChanged(false);
					m_MissileView = false;
					return;
				}
			}

			auto pos = m_Missile->GetAbsolutePosition();
			auto ascending = pos.Z > m_PreviousMissilePos.Z;
			auto backOffset = ascending ? m_fMissilePosBackOffset : m_fMissilePosBackOffsetDescent;
			auto dir = (pos - m_PreviousMissilePos).Normalized();
			Basis3 localBasis(Vector3::Zero, dir);
			Vector3 vTargetPos = pos - dir*backOffset + localBasis.Vertical*m_fMissilePosHeightOffset;
			Vector3 vLookAt = pos + dir*m_fMissileLookAtOffset;
			m_vPosition = Math::Damp(m_vPosition, vTargetPos, g_fDeltaTime, m_fMissileFollowDuration);
			m_vLookAt = Math::Damp(m_vLookAt, vLookAt, g_fDeltaTime, m_fMissileFollowDuration);
			m_FOV = Math::Damp(m_FOV, m_fTargetFOV, g_fDeltaTime, m_fMissileFollowDuration);
			m_bViewMatrixDirty = true;
			m_PreviousMissilePos = pos;
			return;
		}

		// regular target following
		Basis3 localBasis(Vector3::Zero, m_Target->GetLookAt());		
		Vector3 vTargetPos = m_Target->GetAbsolutePosition() - m_Target->GetLookAt()*m_fPosBackOffset + localBasis.Vertical*m_fPosHeightOffset; 
		Vector3 vLookAt = m_Target->GetAbsolutePosition() + m_Target->GetLookAt()*m_fLookAtOffset;
		m_vPosition = Math::Damp(m_vPosition, vTargetPos, g_fDeltaTime, m_fFollowDuration);
		m_vLookAt = Math::Damp(m_vLookAt, vLookAt, g_fDeltaTime, m_fFollowDuration);
		m_FOV = Math::Damp(m_FOV, m_fTargetFOV, g_fDeltaTime, m_fFollowDuration);
		m_bViewMatrixDirty = true;
	}

	//! PickTarget
	Actor* FollowCamera::PickTarget()
	{		
		auto& defenders = DEFENDERMGR->GetDefenders();
		if (defenders.size())
			return defenders.front();		

		auto& towers = PLAYFIELD->GetTowers();
		if (towers.size())
			return towers.front();

		return NULL;
	}

	//! PickMissile
	Entity3D* FollowCamera::PickMissile()
	{
		Entity3D* missile = NULL;
		for (auto it = m_Missiles.begin(); it != m_Missiles.end();)
		{
			missile = (*it).Get();
			if (missile)
				break;
			it = m_Missiles.erase(it);
		}
		return missile;
	}

	//! OnFPSModeChanged
	void FollowCamera::OnFPSModeChanged(bool fpsMode)
	{
		auto uiContext = Engine::Instance()->GetTopContext();
		auto fpsButton = uiContext->GetChildByName("FPSMode");
		auto normalButton = uiContext->GetChildByName("NormalMode");
		if (fpsMode)
		{
			bool targetFound = false;
			m_MissileView = false;

			m_Missile = PickMissile();
			if (m_Missile.IsValid())
			{
				targetFound = true;
				m_MissileView = true;
			}
			else
			{
				auto& defenders = DEFENDERMGR->GetDefenders();
				m_FollowingDefenders = !defenders.empty();
				m_TargetIndex = 0;
				m_Target = PickTarget();
				targetFound = m_Target.IsValid();
			}

			if (targetFound)
			{
				CopyFrom(GAMECAM);
				Activate();
				SetEnabled(true);

				normalButton->SetVisible(true);
				fpsButton->SetVisible(false);
				PLAYFIELD->OnFPSModeChanged(true);
			}			
		}
		else
		{
			GAMECAM->TransitionFrom(this);
			SetEnabled(false);
						
			normalButton->SetVisible(false);
			fpsButton->SetVisible(false);
			PLAYFIELD->OnFPSModeChanged(false);
		}		
	}

	//! OnDefendersSpawned
	void FollowCamera::OnDefendersSpawned()
	{
		m_FollowingDefenders = true;
		m_TargetIndex = 0;
		m_Target = PickTarget();
	}

	//! OnGameCameraReady
	void FollowCamera::OnGameCameraReady()
	{
		if (GAMEMGR->GetState() != GameManager::S_InGame
		|| !GAMEMGR->GetGameData()->HasProduct(GameData::PT_FPSMode))
			return;

		auto uiContext = Engine::Instance()->GetTopContext();
		auto fpsButton = uiContext->GetChildByName("FPSMode");
		fpsButton->SetVisible(true);
	}

	//! OnMissileLaunched
	void FollowCamera::OnMissileLaunched(Entity3D* missile)
	{
		if (!GAMEMGR->GetGameData()->HasProduct(GameData::PT_FPSMode))
			return;

		m_Missiles.push_back(Handle<Entity3D>(missile));

		if (!IsEnabled() || m_MissileView)
			return;

		// switch to missile view
		m_MissileView = true;
		m_Missile = missile;
		m_PreviousMissilePos = missile->GetAbsolutePosition();
	}
}

