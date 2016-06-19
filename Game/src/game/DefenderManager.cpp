/* 

Amine Rehioui
Created: December 4th 2014
*/

#include "Game.h"

#include "DefenderManager.h"
#include "GameSettings.h"
#include "Defender.h"
#include "Button.h"

#include "FollowCamera.h"

namespace shoot
{
	DEFINE_OBJECT(DefenderManager);

	//! constructor
	DefenderManager::DefenderManager()
		: m_State(S_Ready)
		, m_SpawnTimer(-1.0f)
		, m_RandomSpots(0)
		, m_DiseappearTimer(4.0f)
	{
	}

	//! serializes the entity to/from a PropertyStream
	void DefenderManager::Serialize(PropertyStream& stream)
	{
		super::Serialize(stream);

		stream.Serialize("PreloadedActor", &m_PreloadedActor);
	}

	//! called during the initialization of the entity
	void DefenderManager::Init()
	{
		super::Init();
		
		m_CooldownTimer = GSETTINGS->m_DefenderManagerSettings->m_Cooldown;
		m_RandomSpots = RandomPool<int>(GSETTINGS->m_DefenderManagerSettings->m_SpotsPerQuarter);

		m_CallDownButton = static_cast<Button*>(GetParent()->GetChildByName("CallDown"));
		if (m_CallDownButton.IsValid())
		{
			m_CallDownButton->SetUIEnabled(false);
			m_CallDownButton->GetChildByName("Halo")->SetVisibleAndEnabled(false);
		}

		if(auto cooldownMask = GetParent()->GetChildByName("CallMask"))
		{
			m_CooldownShader = cooldownMask->GetComponent<GraphicComponent>()->GetMaterial()->GetShader();
			float angle = 0.0f;
			m_CooldownShader->GetConstant("uCurrentAngle")->Set(&angle, 1);
		}
	}

	//! called during the update of the entity	
	void DefenderManager::Update()
	{
		auto coolDown = GSETTINGS->m_DefenderManagerSettings->m_Cooldown;

		// update cool down
		if (m_CooldownTimer > 0.0f)
		{			
			float angle = ((coolDown - m_CooldownTimer) / coolDown) * Math::PITimes2;
			m_CooldownTimer -= g_fDeltaTime;
			if (m_CooldownTimer <= 0.0f)			
			{
				angle = Math::PITimes2 + .1f;
				m_CooldownTimer = -1.0f;
				m_CallDownButton->SetUIEnabled(true);
				m_CallDownButton->GetChildByName("Halo")->SetVisibleAndEnabled(true);
			}
			m_CooldownShader->GetConstant("uCurrentAngle")->Set(&angle, 1);
		}

		switch(m_State)
		{
		case S_Spawning:
		UpdateSpawning();
		break;

		case S_Defending:		
		if (m_CooldownTimer < (coolDown - GSETTINGS->m_DefenderManagerSettings->m_PullBackTreshold))
			SwitchState(S_PullBack);		
		break;			

		case S_PullBack:
			m_DiseappearTimer -= g_fDeltaTime;
			if (m_CooldownTimer < 0.0f || m_DiseappearTimer < 0.0f)
			{
				for (auto defender : m_lDefenders)
				{
					if (defender.IsValid())
						defender->GetParent()->RemoveChild(defender);
				}

				m_lDefenders.clear();
				SwitchState(S_Ready);
			}
		break;
		}
	}

	void DefenderManager::SpawnDefenders()
	{
		SHOOT_ASSERT(m_CooldownTimer < 0.0f, "Trying to spawn defenders twice");
		SwitchState(S_Spawning);
		AUDIOMGR->Play(AudioManager::S_MetalRiff);
	}

	void DefenderManager::UpdateSpawning()
	{
		if(m_WaveTimer < 0.0f)
		{
			auto angleStep = 90.0f / (GSETTINGS->m_DefenderManagerSettings->m_SpotsPerQuarter + 1);
			for(int i=0; i<4; ++i)
			{
				auto angleStart = 90.0f*i;
				auto angleEnd = 90.0f*(i+1);
				for (int j = 0; j < GSETTINGS->m_DefenderManagerSettings->m_CountPerWavePerQuarter; ++j)
				{
					auto spawnAngle = angleStart + angleStep*(m_RandomSpots.GetNext()+1);
					auto spawnDir = Vector3::Create(Math::Cos(spawnAngle*Math::DegToRadFactor), Math::Sin(spawnAngle*Math::DegToRadFactor), 0.0f);
					auto pos = spawnDir * GSETTINGS->m_DefenderManagerSettings->m_Radius;
					auto range = GSETTINGS->m_DefenderManagerSettings->m_AngleRange;
					auto angle = Random::GetFloat(-range, range);
					auto dir = (-spawnDir).Rotate(Vector3::Up, angle * Math::DegToRadFactor);

					// Spawn actor
					auto newActor = static_cast<Defender*>(m_PreloadedActor->Copy());
					AddChild(newActor);
					newActor->SetPosition(pos);
					newActor->LookAt(dir);
					newActor->SwitchState(Defender::S_Hover);
					newActor->InitActor(GSETTINGS->m_DefenderSettings);
					m_lDefenders.push_back(newActor);
				}
			}

			m_WaveTimer = GSETTINGS->m_DefenderManagerSettings->m_WaveFrequency;
		}
		else
			m_WaveTimer -= g_fDeltaTime;

		if (m_SpawnTimer < 0.0f)
		{
			SwitchState(S_Defending);

			auto camera = EntityRenderer::Instance()->Get3DCamera();
			if (camera == FOLLOWCAM)
				FOLLOWCAM->OnDefendersSpawned();
		}
		else
			m_SpawnTimer -= g_fDeltaTime;
	}

	void DefenderManager::SwitchState(E_State state)
	{
		switch(state)
		{
		case S_Spawning:
		m_WaveTimer = -1.0f;
		m_SpawnTimer = GSETTINGS->m_DefenderManagerSettings->m_SpawnDuration;
		m_CooldownTimer = GSETTINGS->m_DefenderManagerSettings->m_Cooldown;
		m_CallDownButton->SetUIEnabled(false);
		m_CallDownButton->GetChildByName("Halo")->SetVisibleAndEnabled(false);
			break;

		case S_PullBack:
			for (auto defender : m_lDefenders)
			{
				if (defender.IsValid())
					defender->SwitchState(Defender::S_PullBack);
			}
			m_DiseappearTimer = GSETTINGS->m_DefenderManagerSettings->m_DiseappearTime;
			break;
		}

		m_State = state;
	}
}

