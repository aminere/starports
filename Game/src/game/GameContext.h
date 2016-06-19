/*

Amine Rehioui
Created: December 6th 2014

*/

#pragma once


#define GAMECONTEXT GameEngine::Instance()->GetGameContext()
#define GSETTINGS GAMECONTEXT->m_GameSettings
#define WAVEMGR GAMECONTEXT->m_WaveManager
#define PLAYFIELD GAMECONTEXT->m_Playfield
#define GAMECAM GAMECONTEXT->m_GameCamera
#define FOLLOWCAM GAMECONTEXT->m_FollowCamera
#define SFXMGR GAMECONTEXT->m_SFXManager
#define AIMGR GAMECONTEXT->m_AIManager
#define DEFENDERMGR GAMECONTEXT->m_DefenderManager

#define AUDIOMGR GameEngine::Instance()->GetAudioMgr()
#define GAMEMGR GameEngine::Instance()->GetGameMgr()

namespace shoot
{
	class GameSettings;
	class WaveManager;
	class Playfield;
	class GameCamera;
	class FollowCamera;
	class SFXManager;
	class AIManager;
	class DefenderManager;

	//! GameContext
	class GameContext : public Context
	{
		//! Macro to provide type information
		DECLARE_OBJECT(GameContext, Context);

	public:	

		//! GameContext
		GameContext();

		//! called during the initialization of the entity
		void Init();

		Handle<GameSettings> m_GameSettings;
		Handle<WaveManager> m_WaveManager;
		Handle<Playfield> m_Playfield;
		Handle<GameCamera> m_GameCamera;
		Handle<FollowCamera> m_FollowCamera;
		Handle<SFXManager> m_SFXManager;
		Handle<AIManager> m_AIManager;
		Handle<DefenderManager> m_DefenderManager;
	};
}



