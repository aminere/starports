/* 

Amine Rehioui
Created: June 24th 2011

*/

#pragma once


#include "Texture.h"

#include "GameManager.h"
#include "AudioManager.h"

namespace shoot
{
	class GameContext;

	//! Game Engine
	class GameEngine : public Engine				    			   
	{
		typedef Engine super;

	public:

		//! constructor
		GameEngine(ObjectRegistrationCallback* pRegisterObjects,
				   InputManager::E_Type _eInputManagerType = InputManager::IMT_SDL);

		//! destructor
		virtual ~GameEngine();

		//! Engine initialization
		virtual void Init();

		//! called during the update of the engine
		/** \return: false to exit the program */
		virtual bool Update(float fDeltaTime);

		//! called during the rendering the engine
		virtual void Render();

		//! called when the application is resumed
		virtual void OnResume();

		//! returns the loading status
		virtual bool IsLoading() const override { return m_LoadingState != LS_None; }

		//! getters
		inline GameManager* GetGameMgr() const { return m_GameManager; }
		inline AudioManager* GetAudioMgr() const { return m_AudioManager; }
		inline GameContext* GetGameContext() const { return m_GameContext; }
		inline Shader* GetTransitionShader() const { return m_TransitionShader; }

		//! setters
		inline void SetGameContext(GameContext* context) { m_GameContext = context; }

		//! returns the instance
		inline static GameEngine* Instance() { return m_spInstance; }

	protected:

		//! Processes resource reload request
		void OnResourceReloadRequest();

		//! called when the loading thread has started
		void OnLoadingStarted(bool useLoadingTransition) override;

		//! UpdateLoading
		bool UpdateLoading();

		//! RenderLoading
		void RenderLoading();

		//! LoadingState
		enum E_LoadingState
		{
			LS_None,
			LS_TransitionOut,
			LS_Loading,
			LS_TransitionIn
		};

		E_LoadingState m_LoadingState;
		float m_LoadingTransitionTimer;
		bool m_LoadingTransitionEnabled;
		Reference<Shader> m_TransitionShader;

		Reference<GameManager> m_GameManager;
		Reference<AudioManager> m_AudioManager;
		Handle<GameContext> m_GameContext;
		Reference<ContextStack> m_LoadingScreen;

		static GameEngine* m_spInstance;
	};
}



