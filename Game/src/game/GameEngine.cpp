/* 

Amine Rehioui
Created: June 24th 2011

*/

#include "Game.h"
#include "GameEngine.h"

#include "InputManager.h"
#include "EventManager.h"
#include "FileSystem.h"
#include "SequenceVisitor.h"
#include "Playfield.h"

#include "EntityRenderer3D.h"
#include "GeometryProvider.h"

namespace shoot
{
	//! statics
	GameEngine* GameEngine::m_spInstance = NULL;

	//! constructor
	GameEngine::GameEngine(ObjectRegistrationCallback* pRegisterObjects,
						   InputManager::E_Type _eInputManagerType /*= InputManagre::IMT_SDL*/)
	: super(pRegisterObjects, _eInputManagerType)
	, m_LoadingState(LS_None)
	{
		SHOOT_ASSERT(!m_spInstance, "multiple GameEngine instances detected");
		m_spInstance = this;
	}

	//! destructor
	GameEngine::~GameEngine()
	{
		 m_spInstance = NULL;
	}

	//! Engine initialization
	void GameEngine::Init()
	{
		FileSystem::Instance()->LoadPartition("common");

		super::Init();

		m_AudioManager = static_cast<AudioManager*>(ObjectManager::Instance()->Load("common/AudioManager.xml"));
		m_AudioManager->Init();

		m_GameManager = static_cast<GameManager*>(ObjectManager::Instance()->Load("common/GameManager.xml"));
		m_GameManager->Init();

		m_LoadingScreen = static_cast<ContextStack*>(ObjectManager::Instance()->Load("common/Loading.xml"));
		m_LoadingScreen->Init();

		m_TransitionShader = static_cast<Shader*>(ObjectManager::Instance()->FindOrLoad("common/shaders/transition.xml"));

		// TODO: typical implementation for ::RequestResourceReload(callback)
		//{
			//FileSystem::Instance()->UnloadAllPartitions("common");
			//GraphicsDriver::Instance()->LoadObjects();
			// TODO: activate 2D camera from current stack
		//}
	}

	//! called during the update of the engine
	/** \return: false to exit the program */
	bool GameEngine::Update(float fDeltaTime)
	{
		if (IsLoading())
		{
			if (UpdateLoading())
			{
				CalculateFPS(fDeltaTime);
				return true;
			}
		}

		if(m_bRequestResourceReload)
		{
			super::Update(fDeltaTime);
			return true;
		}

#ifndef _RETAIL_
		if(InputManager::Instance()->IsKeyJustReleased(InputManager::KT_Y)) // pause
		{
			m_bPaused = !m_bPaused;
		}
		else if (InputManager::Instance()->IsKeyJustReleased(InputManager::KT_R)) // simulate resource reloading
		{
			OnPause();
			GraphicsDriver::Instance()->UnloadObjects();
			OnResume();
			RequestResourceReload([]()
			{
				GraphicsDriver::Instance()->LoadObjects();
				FileSystem::Instance()->UnloadAllPartitions("common");
			});
		}
#endif

		if(m_ContextStackToLoad.empty())
		{
			m_GameManager->Update();

			if(InputManager::Instance()->IsKeyJustPressed(InputManager::KT_Back))
			{
				shoot::UIEvent* pEvent = snew shoot::UIEvent();
				pEvent->m_Command = "BACK";
				EventManager::Instance()->SendEvent(pEvent);
			}
		}

		m_AudioManager->Update();

#ifndef _RETAIL_
		// god mode
		static float fDeltaTimeAccelTimer = 0.0f;
		if(InputManager::Instance()->IsKeyPressed(InputManager::KT_A)
		&& InputManager::Instance()->IsKeyPressed(InputManager::KT_B))
		{
			fDeltaTimeAccelTimer += fDeltaTime;
			return super::Update(fDeltaTime*3.0f*((int)fDeltaTimeAccelTimer+1));
		}
		else
		{
			fDeltaTimeAccelTimer = 0.0f;
			return super::Update(fDeltaTime);
		}
#else
		return super::Update(fDeltaTime);
#endif
	}

	//! called during the rendering the engine
	void GameEngine::Render()
	{
		if (IsLoading())
		{
			RenderLoading();
			return;
		}

		if(m_bRequestResourceReload)		
			return;

		super::Render();
		#ifdef SHOOT_EDITOR
			EntityRenderer::Instance()->RenderBlackBars();
		#else
			if (m_GameManager->GetState() == GameManager::S_MissionFailed
			|| m_GameManager->GetState() == GameManager::S_MissionSuceeded)
			{
				EntityRenderer::Instance()->RenderBlackBars();
			}
		#endif
		Present();
	}

	//! called when the loading thread has started
	void GameEngine::OnLoadingStarted(bool useLoadingTransition)
	{
		Camera* pCamera = static_cast<Camera*>(m_LoadingScreen->GetChildByName("LoadingCam"));
		pCamera->Activate();
		Entity* pLoadingText = m_LoadingScreen->GetChildByName("LoadingText");
		pLoadingText->GetComponent<GraphicComponent>()->GetMaterial()->GetTexture()->ResourceInit();
		pLoadingText->SetVisible(false);
		pLoadingText->GetComponent<SequenceVisitor>()->Visit(pLoadingText);

		m_LoadingTransitionEnabled = useLoadingTransition;
		if (m_ContextStack->GetChildCount() == 0)
			m_LoadingTransitionEnabled = false;

		if (m_LoadingTransitionEnabled)
		{
			m_LoadingTransitionTimer = 0.0f;
			m_LoadingState = LS_TransitionOut;
		}
		else
			m_LoadingState = LS_Loading;
	}

	//! UpdateLoading
	bool GameEngine::UpdateLoading()
	{
		switch (m_LoadingState)
		{
		case LS_TransitionOut:
			if (m_LoadingTransitionTimer < GAMEMGR->GetContextTransitionDuration())			
				m_LoadingTransitionTimer += g_fDeltaTime;
			else
				m_LoadingState = LS_Loading;
			return true;

		case LS_Loading:
			m_LoadingScreen->Update();
			UpdateLoadingThreadStatus();
			if (m_pLoadingThread != NULL)
				return true;
			else
			{
				if (m_LoadingTransitionEnabled)
				{
					m_LoadingTransitionTimer = 0.0f;
					m_LoadingState = LS_TransitionIn;
					return true;
				}
				else
				{
					m_FadeRenderer->StartFade(Color::Black, Color::Zero, 1.0f, false);
					m_LoadingState = LS_None;
				}
			}
			break;

		case LS_TransitionIn:
			if (m_LoadingTransitionTimer < GAMEMGR->GetContextTransitionDuration())
			{
				m_LoadingTransitionTimer += g_fDeltaTime;
				return true;
			}
			else
				m_LoadingState = LS_None;
			break;
		}

		return false;
	}

	//! RenderLoading
	void GameEngine::RenderLoading()
	{
		auto fullScreenQuad = GeometryProvider::Instance()->GetFullScreenQuad();
		auto finalRT = EntityRenderer::Instance()->GetFinalRenderTarget();
		int finalRTIndex = 0;

		switch (m_LoadingState)
		{
			case LS_TransitionOut:
			{
				auto progress = m_LoadingTransitionTimer / GAMEMGR->GetContextTransitionDuration();
				GraphicsDriver::Instance()->SetRenderTarget(NULL);
				GraphicsDriver::Instance()->SetRenderState(GraphicsDriver::RS_AlphaBlending, true);
				m_TransitionShader->GetConstant("uTexture0")->Set(&finalRTIndex, 1);
				int transitionOut = 1;
				m_TransitionShader->GetConstant("uTransitionOut")->Set(&transitionOut, 1);
				m_TransitionShader->GetConstant("uProgress")->Set(&progress, 1);
				m_TransitionShader->GetConstant("uResolution")->Set(&finalRT->GetSize(), 1);
				m_TransitionShader->GetConstant("uAmbientColor")->Set(&Color::White, 1);
				finalRT->BindTexture(finalRTIndex, GraphicsDriver::BF_ColorBuffer);
				GraphicsDriver::Instance()->SetTransform(GraphicsDriver::TS_ViewProjection, Matrix44::Identity);
				fullScreenQuad->Draw(m_TransitionShader);
			}
			break;

			case LS_Loading:
			{
				EntityRenderer::Instance()->Clear();
				m_LoadingScreen->RegisterForRendering();
				EntityRenderer::Instance()->Render();
				EntityRenderer::Instance()->RenderBlackBars();
				Present();
			}
			break;

			case LS_TransitionIn:
			{
				EntityRenderer::Instance()->Clear();
				m_ContextStack->RegisterForRendering();
				EntityRenderer::Instance()->Render();

				auto progress = m_LoadingTransitionTimer / GAMEMGR->GetContextTransitionDuration();
				GraphicsDriver::Instance()->SetRenderTarget(NULL);
				GraphicsDriver::Instance()->SetRenderState(GraphicsDriver::RS_AlphaBlending, true);
				m_TransitionShader->GetConstant("uTexture0")->Set(&finalRTIndex, 1);
				int transitionOut = 0;
				m_TransitionShader->GetConstant("uTransitionOut")->Set(&transitionOut, 1);
				m_TransitionShader->GetConstant("uProgress")->Set(&progress, 1);
				m_TransitionShader->GetConstant("uResolution")->Set(&finalRT->GetSize(), 1);
				m_TransitionShader->GetConstant("uAmbientColor")->Set(&Color::White, 1);
				finalRT->BindTexture(finalRTIndex, GraphicsDriver::BF_ColorBuffer);
				GraphicsDriver::Instance()->SetTransform(GraphicsDriver::TS_ViewProjection, Matrix44::Identity);
				fullScreenQuad->Draw(m_TransitionShader);
			}
			break;
		}
	}

	//! called when the application is resumed
	void GameEngine::OnResume()
	{
		super::OnResume();
		m_GameManager->Pause();
	}
}

