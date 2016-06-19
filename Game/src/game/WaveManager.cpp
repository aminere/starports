/* 

Amine Rehioui
Created: March 9th 2014
*/

#include "Game.h"

#include "WaveManager.h"

#include "Playfield.h"
#include "Random.h"
#include "HQ.h"
#include "AIManager.h"
#include "UIMarker.h"
#include "GameManager.h"
#include "GameSettings.h"
#include "SpawnPoint.h"
#include "DefenderManager.h"
#include "PathwayGraphicComponent.h"
#include "Pathway.h"
#include "Image.h"
#include "MaterialProvider.h"
#include "Text.h"

namespace shoot
{
	DEFINE_OBJECT(WaveManager);

	//! Reads/Writes struct properties from/to a stream
	void WaveManager::Wave::Serialize(PropertyStream& stream)
	{
		stream.Serialize("Duration", &m_fDuration);
		stream.Serialize("Frequencey", &m_fFrequency);
		stream.Serialize("SpawnIndex", &m_SpawnIndex);
		stream.Serialize("TargetIndex", &m_TargetIndex);
		stream.Serialize("ZOffset", &m_fZOffset);
		stream.Serialize("AttackerSettings", &m_AttackerSettings);
		stream.Serialize("Attacker", &m_Attacker);
	}

	//! constructor
	WaveManager::WaveManager()
		: m_CurrentWave(0)
		, m_AttackerCount(0)
		, m_fWaveTimer(-1.0f)
		, m_fSpawnTimer(-1.0f)
		, m_SpawnIndex(0)
		, m_vAverageAttackerPos(Vector3::Zero)
		, m_UITransitionTimer(-1.0f)
		, m_Succeeded(false)
		// properties
		, m_vSpawnRange(Vector3::Create(1.0f, 0.0f, 0.2f))
		, m_fUITransitionDuration(3.0f)
		, m_WaveIcon0Pos(Vector2::Create(5.0f, 25.0f))
		, m_WaveIcon1Pos(Vector2::Create(55.0f, 25.0f))
		, m_fUIOffset(35.0f)
	{
	}

	//! destructor
	WaveManager::~WaveManager()
	{
	}

	//! serializes the entity to/from a PropertyStream
	void WaveManager::Serialize(PropertyStream& stream)
	{
		super::Serialize(stream);

		stream.Serialize("SpawnRange", &m_vSpawnRange);
		stream.Serialize("UITransitionDuration", &m_fUITransitionDuration);
		stream.Serialize("UIOffset", &m_fUIOffset);
		stream.Serialize("WaveIcon0Pos", &m_WaveIcon0Pos);
		stream.Serialize("WaveIcon1Pos", &m_WaveIcon1Pos);
		stream.Serialize("IconInfos", &m_IconInfos);
		stream.Serialize("Waves", &m_aWaves);
	}

	//! called during the initialization of the entity
	void WaveManager::Init()
	{
		super::Init();

		m_aSpawnPoints.clear();
		GetChildrenByType<SpawnPoint>(m_aSpawnPoints);
		GetRoot()->GetChildrenByType<HQ>(m_aObjectives);
		m_Marker = static_cast<UIMarker*>(GetChildByType("UIMarker"));
		m_MarkerTarget = static_cast<Entity3D*>(GetChildByName("MarkerTarget"));
		m_WaveText = static_cast<Text*>(GetChildByName("WaveText"));
		m_Pathway = GetChildByType(Pathway::GetStaticClassName());

		// Init attack info
		for (size_t i=0; i<m_aSpawnPoints.size(); ++i)
		{
			AttackerInfo info;
			m_aAttackerInfos.push_back(info);
		}

		if(m_aWaves.GetSize())
		{
			m_fWaveTimer = m_aWaves[0].m_fDuration;
			m_UITransitionTimer = m_fUITransitionDuration;
			InitWave(0);

			// Init ui
			m_WavePanel = static_cast<Entity2D*>(GetChildByName("Waves"));
			if (m_WavePanel.IsValid())
			{
				m_WaveIconShader = ResourceManager::Instance()->GetResource<Shader>("common/shaders/waveicon.xml");
				auto position = m_WaveIcon1Pos;
				for(auto& wave : m_aWaves)
				{
					std::string iconPath;
					for (auto& info : m_IconInfos)
						if (info->m_Actor == wave->m_Attacker->GetTemplatePath())
						{
							iconPath = info->m_Icon.GetString();
							break;
						}

					Image* icon = snew Image();
					Material::CreationInfo matInfo;
					matInfo.AddTexture(iconPath);
					matInfo.SetShader(m_WaveIconShader->GetTemplatePath());
					matInfo.SetFlag(Material::MF_AlphaBlending, true);
					auto material = MaterialProvider::Instance()->GetMaterial(matInfo);
					auto gc = snew GraphicComponent();
					gc->SetMaterial(material);
					gc->SetRenderingPass(GraphicComponent::RP_2DLayout);
					gc->SetRenderingPriority(1);
					icon->AddComponent(gc, true);

					m_WavePanel->AddChild(icon, true);
					icon->SetPosition(position);
					icon->Init();
					position.X += m_fUIOffset;
					m_WaveIcons.push_back(Handle<Entity2D>(icon));
				}

				m_CutOffLine = static_cast<Entity2D*>(m_WavePanel->GetChildByName("CutOff"));
			}

			#ifdef SHOOT_EDITOR
				UpdateUI();
			#endif
		}
	}

	//! called during the update of the entity	
	void WaveManager::Update()
	{
		if(m_fWaveTimer > 0.0f)
		{
			if(m_fSpawnTimer < 0.0f)
			{
				Wave& wave = m_aWaves[m_CurrentWave];
				BaseAttacker* pAttacker = static_cast<BaseAttacker*>(wave.m_Attacker.Get()->Copy());
				GetRoot()->AddChild(pAttacker);
				auto pSpawnPoint = m_aSpawnPoints[m_SpawnIndex];
				float fXOffset = Random::GetFloat(-m_vSpawnRange.X, m_vSpawnRange.X);
				float fZOffset = Random::GetFloat(-m_vSpawnRange.Z, m_vSpawnRange.Z);				
				pAttacker->SetPosition(pSpawnPoint->GetAbsolutePosition() + Vector3::Create(fXOffset, 0.0f, fZOffset));				
				pAttacker->SetInfo(&m_aAttackerInfos[m_SpawnIndex]);
				pAttacker->InitActor(wave.m_AttackerSettings);
				m_lAttackers.push_back(pAttacker);
				m_fSpawnTimer = wave.m_fFrequency;
				++m_AttackerCount;
				AIMGR->OnActorSpawned(pAttacker);
			}
			m_fSpawnTimer -= g_fDeltaTime;
			m_fWaveTimer -= g_fDeltaTime;
		}
		else if(m_AttackerCount == 0 || AttackersReachedTarget())
		{
			if(m_CurrentWave+1 < m_aWaves.GetSize())
			{
				++m_CurrentWave;
				m_fWaveTimer = m_aWaves[m_CurrentWave].m_fDuration;
				m_UITransitionTimer = m_fUITransitionDuration;
				m_fSpawnTimer = -1.0f;
				InitWave(m_CurrentWave);
			}
			else
			{
				if (m_AttackerCount == 0)
				{
					if (!m_Succeeded)
					{
						m_fSpawnTimer = 3.0f;
						m_Succeeded = true;
					}
					else
					{
						if (m_fSpawnTimer < 0.0f)
							GAMEMGR->OnSucceeded();
						m_fSpawnTimer -= g_fDeltaTime;
					}
					return;
				}
			}
		}

		// Update markers
		if(m_lAttackers.size())
		{
			m_vAverageAttackerPos = Vector3::Zero;
			for(auto it = m_lAttackers.begin(); it != m_lAttackers.end(); )
			{
				if(BaseAttacker* pAttacker = (*it).Get())
				{
					//m_vAverageAttackerPos += pAttacker->GetAbsolutePosition();
					//++it;
					// HACK: just take the closest attacker to HQ
					m_vAverageAttackerPos = pAttacker->GetAbsolutePosition();
					break;
				}
				else
				{
					it = m_lAttackers.erase(it);
				}
			}

			//m_vAverageAttackerPos /= float(m_lAttackers.size());
			m_Marker->SetTarget(m_MarkerTarget);
			m_MarkerTarget->SetAbsolutePosition(m_vAverageAttackerPos);
		}
		else
		{
			m_vAverageAttackerPos = DEFENDERMGR->GetAbsolutePosition();
			m_Marker->SetTarget(NULL);
		}

		UpdateUI();
	}	

	//! called when the terran has changed
	void WaveManager::OnTerrainChanged()
	{
		//for(auto it = m_lAttackers.begin(); it != m_lAttackers.end();)
		//{
		//	if((*it).IsValid())
		//	{
		//		(*it)->OnTerrainChanged();
		//		++it;
		//	}
		//	else
		//	{
		//		it = m_lAttackers.erase(it);
		//	}
		//}

		UpdateCurrentPath();
	}

	//! called to test if the path is blocked
	bool WaveManager::IsPathBlocked() const
	{
		for (size_t i=0; i<m_aSpawnPoints.size(); ++i)
		{
			Entity3D* pSpawnPoint = m_aSpawnPoints[i];
			std::vector<void*> path;
			for (auto& objective : m_aObjectives)
			{
				if (objective->GetHitPoints() < 0.0f)
					continue;

				Vector3 vTargetPos = objective->GetAbsolutePosition();
				PLAYFIELD->GetShortestPath(pSpawnPoint->GetAbsolutePosition(), vTargetPos, path);
				if (path.empty())
					return true;
			}			
		}
		return false;
	}

	//! OnHQDestroyed
	void WaveManager::OnHQDestroyed() const
	{
		if (GetNextHQ())
			return;

		GAMEMGR->OnFailed();
	}

	//! GetNextHQ
	HQ* WaveManager::GetNextHQ() const
	{
		for (auto& objective : m_aObjectives)
			if (objective->GetHitPoints() >= 0.0f)
				return objective.Get();

		return NULL;
	}

	void WaveManager::InitWave(size_t waveIndex)
	{
		// determine index
		auto spawnIndexIn = m_aWaves[waveIndex].m_SpawnIndex;
		switch(spawnIndexIn)
		{
		case -1:
			m_SpawnIndex = GetRandomSpawnIndex();
			break;

		default:
			{
				m_SpawnIndex = int(spawnIndexIn);
				std::vector<int>::iterator it = std::find(m_aSpawnIndices.begin(), m_aSpawnIndices.end(), m_SpawnIndex);
				if(it != m_aSpawnIndices.end())
					m_aSpawnIndices.erase(it);
			}
		}

		if (m_aSpawnPoints.size())
		{
			SHOOT_ASSERT(m_SpawnIndex < m_aSpawnPoints.size(), "Invalid m_SpawnIndex");
		}

		// Update target info
		m_aAttackerInfos[m_SpawnIndex].m_fZOffset = m_aWaves[waveIndex].m_fZOffset;
		auto targetIndex = m_aWaves[waveIndex].m_TargetIndex;
		if (targetIndex < 0)
			targetIndex = Random::GetInt(0, (int)m_aObjectives.size()-1);
		m_aAttackerInfos[m_SpawnIndex].m_Target = m_aObjectives[targetIndex];

		// if target has been destroyed, pick the first valid one
		if (m_aAttackerInfos[m_SpawnIndex].m_Target->GetHitPoints() < 0.0f)
		{
			for (auto& objective : m_aObjectives)
			{
				if (!(objective->GetHitPoints() < 0.0f))
				{
					m_aAttackerInfos[m_SpawnIndex].m_Target = objective;
					break;
				}
			}
		}

		SHOOT_ASSERT(m_aAttackerInfos[m_SpawnIndex].m_Target.IsValid(), "Could not determine a valid target");
		UpdateCurrentPath();
		UpdateWaveText();
	}
	
	int WaveManager::GetRandomSpawnIndex()
	{
		if(m_aSpawnIndices.empty())
		{
			for (size_t i=0; i<m_aSpawnPoints.size(); ++i)			
				m_aSpawnIndices.push_back((int)i);
		}

		if (m_aSpawnIndices.empty())
			return 0;

		int index = Random::GetInt(0, (int)m_aSpawnIndices.size()-1);
		int spawnIndex = m_aSpawnIndices[index];
		m_aSpawnIndices.erase(m_aSpawnIndices.begin()+index);
		return spawnIndex;
	}	

	bool WaveManager::AttackersReachedTarget() const
	{
		for(auto &attacker : m_lAttackers)
		{
			if(!attacker.IsValid())
				continue;

			if(!attacker->IsTargetReached())
				return false;
		}

		return true;
	}

	void WaveManager::UpdateCurrentPath()
	{
		if(!PLAYFIELD)
			return;

		auto pSpawnPoint = m_aSpawnPoints[m_SpawnIndex];
		Vector3 vTargetPos = m_aAttackerInfos[m_SpawnIndex].m_Target->GetAbsolutePosition();
		std::vector<void*> path;
		PLAYFIELD->GetShortestPath(pSpawnPoint->GetAbsolutePosition(), vTargetPos, path);
		SHOOT_ASSERT(!path.empty(), "Path can't be empty at this point");
		SHOOT_ASSERT(path.size() > 2, "Invalid path detected");

		m_DummyTargetTile.m_vPosition = vTargetPos;
		m_DummyTargetTile.m_vPosition.Z = 0.0f;
		path.push_back(&m_DummyTargetTile);
		
		size_t currentElem = 0;
		for(; currentElem < path.size(); ++currentElem)
		{
			Entity3D* elem = NULL;
			if(currentElem < m_Pathway->GetChildCount())
				elem = static_cast<Entity3D*>(m_Pathway->GetChild(currentElem));
			else
			{
				elem = snew PathwayElement();
				m_Pathway->AddChild(elem, true);
			}

			auto tile = static_cast<Tile*>(path[currentElem]);
			auto tilePos = tile->m_vPosition + PLAYFIELD->GetPosition();
			elem->SetAbsolutePosition(tilePos);
		}

		// remove unused elemnents
		while (m_Pathway->GetChildCount() > currentElem)
			m_Pathway->RemoveChild(m_Pathway->GetChild(currentElem), true);

		auto gc = m_Pathway->GetComponent<PathwayGraphicComponent>();
		auto numSegments = path.size() * GSETTINGS->m_PathwaySegmentsPerTile;
		gc->SetNumSegments((int)numSegments);
		gc->UpdatePath();
	}

	void WaveManager::UpdateUI()
	{
		if (!m_WavePanel.IsValid())
			return;

		const auto& screenSize = GraphicsDriver::Instance()->GetViewPort().Size();
		auto panelPos = m_WavePanel->GetWorldTransform().GetTranslation().X;
		auto cutOffPos = m_CutOffLine->GetWorldTransform().GetTranslation().X;
		// convert [0, screenW] => [0, 1] => [-1, 1]
		auto uLeftCutOffX = -1.0f + ((panelPos+2.0f) / screenSize.X)*2.0f;
		auto uMidCutOffX = -1.0f + (cutOffPos / screenSize.X)*2.0f;
		auto panelScale = m_WavePanel->GetAbsoluteScale().X;
		auto uRightCutOffX = -1.0f + ((panelPos + m_WavePanel->GetBoundingBox().Size().X*panelScale - 2.0f) / screenSize.X)*2.0f;
		m_WaveIconShader->GetConstant("uLeftCutOffX")->Set(&uLeftCutOffX, 1);
		m_WaveIconShader->GetConstant("uMidCutOffX")->Set(&uMidCutOffX, 1);
		m_WaveIconShader->GetConstant("uRightCutOffX")->Set(&uRightCutOffX, 1);

		if(m_UITransitionTimer < 0.0f)
			return;

		m_UITransitionTimer -= g_fDeltaTime;
		auto factor = 1.0f - (m_UITransitionTimer / m_fUITransitionDuration);
		factor = Math::Clamp(factor, 0.0f, 1.0f);
		for (size_t i = 0; i<m_WaveIcons.size(); ++i)
		{
			auto icon = m_WaveIcons[i].Get();

			// TODO do this once at transition start
			if(m_CurrentWave > 0 && i < m_CurrentWave-1)
			{
				icon->SetVisible(false);
				continue;
			}

			Vector2 srcPos, destPos;
			if(i < m_CurrentWave)
			{
				srcPos = m_WaveIcon0Pos;
				destPos = srcPos - Vector2::Create(m_fUIOffset, 0.0f);
			}
			else if(i == m_CurrentWave)
			{
				srcPos = m_WaveIcon1Pos;
				destPos = m_WaveIcon0Pos;
			}
			else // i > m_CurrentWave
			{
				auto index = i-m_CurrentWave;
				srcPos = m_WaveIcon1Pos + Vector2::Create(m_fUIOffset*index, 0.0f);
				destPos = srcPos - Vector2::Create(m_fUIOffset, 0.0f);
			}

			icon->SetPosition(Math::Lerp(srcPos, destPos, factor));
		}
	}

	void WaveManager::UpdateWaveText()
	{
		if (!m_WaveText.IsValid())
			return;

		auto numWaves = m_aWaves.GetSize();
		auto waveText = std::wstring(L"Wave ") + Utils::ToWString(m_CurrentWave + 1) + L" of " + Utils::ToWString(numWaves);
		m_WaveText->SetText(waveText);
	}
}



