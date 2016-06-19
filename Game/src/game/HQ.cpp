/* 

Amine Rehioui
Created: March 8th 2014

*/

#include "Game.h"

#include "HQ.h"
#include "RigidBodyComponent.h"
#include "ProgressBar.h"
#include "GameManager.h"

#include "ExplosionVisitor.h"
#include "WaveManager.h"
#include "GameCamera.h"

namespace shoot
{
	DEFINE_OBJECT(HQ);

	//! constructor
	HQ::HQ()
		: m_fExplosionTimer(-1.0f)
		// properties
		, m_fMaxHitPoints(150.0f)
		, m_fExplosionIntensity(8.0f)
		, m_fExplosionDuration(1.5f)
	{
	}

	//! serializes the entity to/from a PropertyStream
	void HQ::Serialize(PropertyStream& stream)
	{
		super::Serialize(stream);
		
		stream.Serialize("MaxHitPoints", &m_fMaxHitPoints);
		stream.Serialize("ExplosionIntensity", &m_fExplosionIntensity);
		stream.Serialize("ExplosionDuration", &m_fExplosionDuration);
		stream.Serialize("ExplosionVisitor", &m_ExplosionVisitor);
		stream.Serialize("ExplosionEntity", &m_ExplosionEntity);
	}

	//! called during the initialization of the entity
	void HQ::Init()
	{
		super::Init();
				
		m_HealthBG = DYNAMIC_CAST(GetChildByName("HealthBG"), Entity3D);
		m_HealthBar = DYNAMIC_CAST(GetChildByName("HealthBar"), ProgressBar);
		m_fHitPoints = m_fMaxHitPoints;

		if(Entity* pMesh = GetChildByName("HQMesh"))
			m_Collision = pMesh->GetComponent<RigidBodyComponent>();
	}
	
	//! called during the update of the entity
	void HQ::Update()
	{
		if (m_fHitPoints < 0.0f)
		{
			m_fExplosionTimer -= g_fDeltaTime;
			if (m_fExplosionTimer < 0.0f)
			{
				WAVEMGR->OnHQDestroyed();
				SetVisibleAndEnabled(false);
			}
		}
	}

	//! called when the HQ has taken damage
	bool HQ::OnDamage(float fDamage)
	{
		if (m_fExplosionTimer > 0.0f)
			return true;

		m_fHitPoints -= fDamage;

		m_HealthBG->SetVisible(true);
		m_HealthBar->SetProgress(m_fHitPoints / m_fMaxHitPoints);

		if(m_fHitPoints < 0.0f)
		{
			auto explosionVisitor = static_cast<ExplosionVisitor*>(m_ExplosionVisitor->Copy());
			auto explosionEntity = static_cast<Entity3D*>(m_ExplosionEntity->Copy());
			explosionEntity->SetScale(Vector3::One * 2.0f);

			// add explosion visitor
			explosionVisitor->SetDefaultIntensity(m_fExplosionIntensity);
			explosionVisitor->SetDefaultDuration(m_fExplosionDuration);
			auto mesh = static_cast<Entity3D*>(GetChildByName("HQMesh"));
			mesh->AddComponent(explosionVisitor);

			// add explosion entity
			AddChild(explosionEntity);
			explosionEntity->SetPosition(mesh->GetPosition());

			AUDIOMGR->Play(AudioManager::S_ExplosionNuclear);
			m_fExplosionTimer = m_fExplosionDuration;
			m_HealthBG->SetVisible(false);

			GAMECAM->Shake(.5f, .02f, m_fExplosionDuration / 2.0f);
			return true;
		}

		return false;
	}
}

