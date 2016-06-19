/* 

Amine Rehioui
Created: April 11th 2014

*/

#include "Game.h"

#include "WheelMenu.h"
#include "MaterialProvider.h"
#include "WheelMenuButton.h"
#include "GameCamera.h"
#include "AudioManager.h"
#include "Playfield.h"
#include "Text.h"
#include "GameSettings.h"
#include "NuclearTower.h"

namespace shoot
{
	DEFINE_OBJECT(WheelMenu);

	//! constructor
	WheelMenu::WheelMenu()
		: m_eState(S_Closed)
		, m_pTile(NULL)
		// properties
		, m_fRadius(100.0f)
		, m_fAnimDuration(0.25f)
		, m_fAnimAngleOffset(60.0f)
		, m_ScreenBounds(Vector2::Create(40.0f, 40.0f), Vector2::Create(920.0f, 600.0f))
	{
	}

	//! serializes the entity to/from a PropertyStream
	void WheelMenu::Serialize(PropertyStream& stream)
	{
		super::Serialize(stream);

		stream.Serialize("Radius", &m_fRadius);
		stream.Serialize("AnimDuration", &m_fAnimDuration);
		stream.Serialize("AnimAngleOffset", &m_fAnimAngleOffset);
		stream.Serialize("ScreenBounds", &m_ScreenBounds);
		stream.Serialize("SellIcon", &SellIcon);
		stream.Serialize("RepairIcon", &RepairIcon);
		stream.Serialize("NuclearLaunchIcon", &NuclearLaunchIcon);
		stream.Serialize("ButtonRef", &m_ButtonRef);
	}

	//! Initializes the menu
	void WheelMenu::Open(Tile* pTile)
	{
		if(!IsInitialized())
			super::Init();

		m_aAnimInfos.clear();
		m_eState = S_Closed;
		if(Entity* pButtons = GetChildByName("Buttons"))
			RemoveChild(pButtons, true);				
		
		m_ButtonInfos.clear();

		auto actor = static_cast<Actor*>(pTile->m_Actor.Get());
		if(!actor)
		{
			Actor::E_Type types[] = { Actor::T_TowerGun, Actor::T_TowerFlame, Actor::T_TowerElectric, Actor::T_Nuclear };
			for(auto type : types)
			{
				auto info = PLAYFIELD->GetPreloadedActorInfo(type);
				auto action = Utils::StringFormat("SPAWN_%d", (int)type);
				m_ButtonInfos.push_back(ButtonInfo(action.c_str(), info->Icon, info->Levels[Actor::L_1].Diamonds));
			}
		}
		else
		{
			auto info = PLAYFIELD->GetPreloadedActorInfo(actor->GetType());
			auto nextLevel = size_t(actor->GetLevel()) + 1;
			auto canUpgrade = nextLevel < info->Levels.GetSize();

			if (actor->GetHitPoints() < actor->GetMaxHitPoints())
				m_ButtonInfos.push_back(ButtonInfo("REPAIR", RepairIcon, GSETTINGS->m_RepairCost));

			if(canUpgrade)
				m_ButtonInfos.push_back(ButtonInfo("UPGRADE", info->Levels[actor->GetLevel()].UpgradeIcon, info->Levels[nextLevel].Diamonds));

			if (actor->GetType() == Actor::T_Nuclear)
			{
				auto nuclearTower = DYNAMIC_CAST(actor, NuclearTower);
				if (nuclearTower->CanManuallyLaunch())
					m_ButtonInfos.push_back(ButtonInfo("NUKE", NuclearLaunchIcon, GSETTINGS->m_NukeCost));
			}

			m_ButtonInfos.push_back(ButtonInfo("SELL", SellIcon, info->Levels[actor->GetLevel()].Diamonds));
		}

		float fAngleStep = Math::PITimes2 / m_ButtonInfos.size();
		float fAngle = 0.0f;
		auto buttons = snew Entity();
		buttons->SetName("Buttons");
		AddChild(buttons, true);
		for (auto &buttonInfo : m_ButtonInfos)
		{
			auto newButton = static_cast<WheelMenuButton*>(m_ButtonRef->Copy());
			newButton->SetPosition(Vector2::Zero);
			newButton->SetCommand(std::string("WHEELMENU ") + buttonInfo.Action);

			// Set Image
			auto material = snew Material();
			material->SetTexture(0, buttonInfo.m_Texture);
			material->SetFlag(Material::MF_AlphaBlending, true);
			newButton->GetChildByName("Image")->GetComponent<GraphicComponent>()->SetMaterial(material);

			// Set price
			auto priceText = static_cast<Text*>(newButton->GetChildByName("Price"));
			priceText->SetText(Utils::ToWString(buttonInfo.Price));

			buttons->AddChild(newButton, true);
			m_aAnimInfos.push_back(AnimInfo(newButton, fAngle));
			fAngle += fAngleStep;

			// foreground
			auto fgGC = newButton->GetChildByName("FG")->GetComponent<GraphicComponent>();
			buttonInfo.m_Foreground = static_cast<Material*>(fgGC->GetMaterial()->Copy());
			fgGC->SetMaterial(buttonInfo.m_Foreground);

			auto affordable = PLAYFIELD->HasDiamonds(buttonInfo.Price);
			if (!affordable)
			{
				material->SetColor(Color::Create(Color::Red.RGB, 0.5f));
				buttonInfo.m_Foreground->SetColor(Color::Create(Color::White.RGB, 0.5f));
			}
			
			buttonInfo.m_Material = material;
		}
		buttons->Init();

		for (size_t i=0; i<m_aAnimInfos.size(); ++i)
			m_aAnimInfos[i].Button->SetPosition(Vector2::Zero);		

		m_pTile = pTile;
		UpdatePosition();

		m_fInterpolator = 0.0f;
		m_fSrcRadius = 0.0f;
		m_fDestRadius = m_fRadius;
		m_fSrcAngleOffset = -m_fAnimAngleOffset*Math::DegToRadFactor;
		m_fDestAngleOffset = 0.0f;
		m_fSrcAlpha = 0.0f;
		m_fDestAlpha = 1.0f;
		m_eState = S_Opening;
		AUDIOMGR->Play(AudioManager::S_WheelOpen);
	}	

	//! called during the update of the entity
	void WheelMenu::Update()
	{
		Actor* pActor = static_cast<Actor*>(m_pTile->m_Actor.Get());
		if(pActor && pActor->GetHitPoints() < 0.0f)
			Close();

		UpdatePosition();

		switch(m_eState)
		{
		case S_Opening:
		case S_Closing:
			{
				if(m_fInterpolator < 1.0f)
				{
					for (size_t i=0; i<m_aAnimInfos.size(); ++i)
					{
						float fAngleOffset = Math::Lerp(m_fSrcAngleOffset, m_fDestAngleOffset, m_fInterpolator);
						float fAngle = m_aAnimInfos[i].Angle + fAngleOffset;
						float fRadius = Math::Lerp(m_fSrcRadius, m_fDestRadius, m_fInterpolator);
						float fAlpha = Math::Lerp(m_fSrcAlpha, m_fDestAlpha, m_fInterpolator);
						m_aAnimInfos[i].Button->SetPosition(Vector2::Create(Math::Cos(fAngle)*fRadius, Math::Sin(fAngle)*fRadius));
						m_aAnimInfos[i].Button->SetAlpha(fAlpha);
					}
					m_fInterpolator += g_fDeltaTime/m_fAnimDuration;
				}
				else
				{
					for (size_t i=0; i<m_aAnimInfos.size(); ++i)
					{
						float fAngle = m_aAnimInfos[i].Angle + m_fDestAngleOffset;
						float fRadius = m_fDestRadius;
						m_aAnimInfos[i].Button->SetPosition(Vector2::Create(Math::Cos(fAngle)*fRadius, Math::Sin(fAngle)*fRadius));
						m_aAnimInfos[i].Button->SetAlpha(m_fDestAlpha);
					}
					m_eState = (m_eState == S_Opening) ? S_Open : S_Closed;
					if(m_eState == S_Open)
						GetChildByName("Buttons")->SetEnabled(true);
					else
						GetParent()->RemoveChild(this);
				}
			}
			break;		
		}

		if (m_eState == S_Closing || m_eState == S_Closed)
			return;

		// update buttons
		for (auto& button : m_ButtonInfos)
		{
			auto sellButton = (button.m_Texture == SellIcon.Get());
			auto affordable = sellButton || PLAYFIELD->HasDiamonds(button.Price);
			auto affordableAlpha = (m_eState == S_Open) ? 1.0f : button.m_Material->GetColor().A;
			auto alpha = affordable ? affordableAlpha : 0.5f;
			auto color = affordable ? Color::White.RGB : Color::Red.RGB;
			button.m_Material->SetColor(Color::Create(color, alpha));
			button.m_Foreground->SetColor(Color::Create(Color::White.RGB, alpha));
		}
	}	

	//! updates the position
	void WheelMenu::UpdatePosition()
	{
		Camera* pCamera = EntityRenderer::Instance()->Get3DCamera();
		Vector3 vScreenPos;
		if (pCamera->GetScreenPos(m_pTile->m_vPosition, vScreenPos))
		{
			SetPosition(vScreenPos.XY);
			if (!m_ScreenBounds.Contains(vScreenPos.XY))
			{
				if (!InputManager::Instance()->IsTouchPressed() && PLAYFIELD->GetDraggingStarted())
					Close();
			}
		}
		else
			Close();
	}

	//! closes the menu
	void WheelMenu::Close()
	{
		if (m_eState == S_Closing || m_eState == S_Closed)
			return;

		m_fInterpolator = 0.0f;
		m_fSrcRadius = m_fRadius;
		m_fDestRadius = 0.0f;
		m_fSrcAngleOffset = 0.0f;
		m_fDestAngleOffset = -m_fAnimAngleOffset*Math::DegToRadFactor;
		m_fSrcAlpha = 1.0f;
		m_fDestAlpha = 0.0f;
		m_eState = S_Closing;
		GetChildByName("Buttons")->SetEnabled(false);
		AUDIOMGR->Play(AudioManager::S_WheelClose);
		PLAYFIELD->OnWheelMenuClosing();
	}	
}

