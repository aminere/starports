/* 

Amine Rehioui
Created: December 6th 2014
*/

#include "Game.h"

#include "GameContext.h"

#include "GameSettings.h"
#include "Playfield.h"
#include "WaveManager.h"
#include "GameCamera.h"
#include "Defender.h"
#include "FollowCamera.h"
#include "SFXManager.h"
#include "AIManager.h"
#include "DefenderManager.h"

namespace shoot
{
	DEFINE_OBJECT(GameContext);

	//! GameContext
	GameContext::GameContext()
	{
		m_aChildren.reserve(300);
		GameEngine::Instance()->SetGameContext(this);
	}

	//! called during the initialization of the entity
	void GameContext::Init()
	{
		m_GameSettings = GetChildByType<GameSettings>();
		m_WaveManager = GetChildByType<WaveManager>();
		m_Playfield = GetChildByType<Playfield>();
		m_GameCamera = GetChildByType<GameCamera>();
		m_FollowCamera = GetChildByType<FollowCamera>();
		m_SFXManager = GetChildByType<SFXManager>();
		m_AIManager = GetChildByType<AIManager>();
		m_DefenderManager = GetChildByType<DefenderManager>();

		super::Init();
	}
}

