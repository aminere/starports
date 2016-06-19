/* 

Amine Rehioui
Created: October 11th 2011

*/

#include "Game.h"

#include "GameManager.h"

#include "File.h"

#include "FileSystem.h"

#include "AudioManager.h"
#include "AudioDriver.h"
#include "Playfield.h"

#include "EventManager.h"
#include "GameEngine.h"
#include "InputRecorder.h"

#include "IAPContext.h"
#include "SequenceVisitor.h"
#include "Text.h"
#include "TextVisitor.h"
#include "UIContext.h"
#include "UISlider.h"

extern void iOSRequestProducts();

namespace shoot
{
	DEFINE_OBJECT(GameManager);

	//! constructor
	GameManager::GameManager()
		: m_eState(S_Menu)
		, m_CurrentContext(NULL)
		, m_MenuTimer(0.0f)
		, m_ProductsRetrieved(false)
		, m_ThankYouPending(false)
		, m_HasBuyFPSModeRequest(false)
		// properties
		, m_fUITransitionDuration(1.0f)
		, m_fUITransitionOriginTop(-100.0f)
		, m_fUITransitionOriginBottom(100.0f)
		, m_ContextTransitionDuration(2.0f)
		, m_MenuTimeOutBig(15.0f)
		, m_MenuTimeOutSmall(3.0f)
	{

		{
			File::UseWritablePath uwp;
			if(File::Exists(GameData::m_sSaveFile))			
				m_GameData = static_cast<GameData*>(ObjectManager::Instance()->Load(GameData::m_sSaveFile));			
			else
			{
				m_GameData = snew GameData();
				m_GameData->Save();
			}
		}

		AudioDriver::Instance()->SetSoundVolume(m_GameData->GetSoundVolume());
		m_MenuTimeOut = m_MenuTimeOutBig;
	}

	//! destructor
	GameManager::~GameManager()
	{
		EventManager::Instance()->UnregisterListener<UIEvent>(this);
	}

	//! serializes the entity to/from a PropertyStream
	void GameManager::Serialize(PropertyStream& stream)
	{
		super::Serialize(stream);

		stream.Serialize("UITransitionDuration", &m_fUITransitionDuration);
		stream.Serialize("UITransitionOriginTop", &m_fUITransitionOriginTop);
		stream.Serialize("UITransitionOriginBottom", &m_fUITransitionOriginBottom);
		stream.Serialize("ContextTransitionDuration", &m_ContextTransitionDuration);	
		stream.Serialize("MenuTimeOut", &m_MenuTimeOut);		
	}

	//! called during the initialization of the entity
	void GameManager::Init()
	{
		super::Init();

		AUDIOMGR->SetMusicVolume(m_GameData->GetMusicVolume());

		SetupContexts();

#ifndef SHOOT_EDITOR
		SwitchContext(C_TouchToStart);
		//Engine::Instance()->LoadContextStack("menu/menuscenelevels.xml", true, [](){});
#endif

		AUDIOMGR->Play(AudioManager::M_Metal);
		EventManager::Instance()->RegisterListener<UIEvent>(this, (EventManager::EventHandler)&GameManager::HandleUIEvent);
	}

	//! called during the update of the entity
	void GameManager::Update()
	{
		switch (m_eState)
		{
		case S_Menu:
//#ifndef SHOOT_EDITOR
//			if (m_MenuTimer > m_MenuTimeOut)
//			{	
//				const char* folders[] = { "tutorial", "level1" };
//				E_Context contexts[] = { C_Tutorial, C_Level1 };
//				int maxReplayLevels = 4;
//				int contextCount = 1;
//				int contextIndex = Random::GetInt(0, contextCount - 1);
//				int levelIndex = Random::GetInt(0, maxReplayLevels - 1);
//
//				DiscardPartitions(contexts[contextIndex]);
//				auto replayFinished = [&]()
//				{
//					m_MenuTimeOut = m_MenuTimeOutSmall;
//					SwitchContext(GameManager::C_TouchToStart);
//				};
//
//				std::function<void()> loadingFinished = [&]()
//				{
//					FileSystem::Instance()->UnloadAllPartitions("common");
//					GraphicsDriver::Instance()->LoadObjects();
//				};
//
//				auto path = std::string(folders[contextIndex]) + "/rec/rec" + Utils::ToString(levelIndex + 1) + ".xml";
//				InputRecorder::Instance()->Replay(path, false, loadingFinished, replayFinished);
//				SetState(S_Replay);
//				m_MenuTimer = 0.0f;
//			}
//			else
//				m_MenuTimer += g_fDeltaTime;
//#endif
			break;

		case S_MissionSuceeded:
			if (InputManager::Instance()->IsTouchJustReleased() || m_MenuSuccessTouched)
			{
				if (m_MenuSuccessTimer < 0.0f)
					SwitchContext(C_MainMenu);
				else
					m_MenuSuccessTouched = true;
			}
			m_MenuSuccessTimer -= g_fDeltaTime;
			break;

		case S_MissionFailed:
			if (InputManager::Instance()->IsTouchJustReleased())
				SwitchContext(C_MainMenu);
			break;
		}
	}

	//! pauses the game
	void GameManager::Pause()
	{
		if (m_eState != S_InGame)
			return;

		Engine::Instance()->PushContext("common/ui/menupausebg.xml");
		Engine::Instance()->PushContext("common/ui/menupause.xml");
		SetState(S_InGamePause);
	}

	//! resumes the game
	void GameManager::Resume()
	{
		Engine::Instance()->PopContext();
		m_OnPopUIEvent = "POP";
		SetState(S_InGame);		
	}

	//! exits the current level
	void GameManager::ExitLevel()
	{
		m_NavigationList.clear();

		// if level completed, update save data & advance to next level

		SwitchContext(C_MainMenu);
		//AUDIOMGR->Play(AudioManager::M_Metal);
	}

	//! sets the current state
	void GameManager::SetState(E_State eNewState)
	{
		m_eState = eNewState;

		switch (eNewState)
		{
		case S_ThankYou:
		case S_Menu:
		case S_MenuOptions:
			m_NavigationList.clear();
			break;
		}
	}	

	//! initializes a UIContext
	void GameManager::InitUIContext(Entity* pContext)
	{
		std::string path = pContext->GetTemplatePath();
		if (m_eState == S_IAPInit 
		|| m_eState == S_IAPRestore 
		|| m_eState == S_IAPBuy)
		{
			SHOOT_ASSERT(path == "menu/iapstatus.xml", "Invalid menu flow, expecting '%s' context, got '%s'", "iapstatus.xml", path.c_str());
			auto status = pContext->GetChildByName("Status");
			auto ok = pContext->GetChildByName("OK");
			auto anim = status->GetComponent<SequenceVisitor>();

			std::wstring statusText;
			switch (m_eState)
			{
			case S_IAPInit:
				statusText = L"Loading...";
				break;

			case S_IAPBuy:
				statusText = L"...";
				break;

			case S_IAPRestore:
				statusText = L"Restoring Purchases...";
				break;
			}
			auto textVisitor = static_cast<TextVisitor*>(anim->GetVisitor(0));
			textVisitor->SetText(statusText);
			anim->Visit(status);
			ok->SetVisible(false);
		}
		else if (path == "common/ui/menuoptions.xml"
			 || path == "common/ui/menuoptionsoutgame.xml")
		{
			if (UISlider* pSlider = DYNAMIC_CAST(pContext->GetChildByName("MusicSlider"), UISlider))
			{
				pSlider->SetRatio(m_GameData->GetMusicVolume());
			}
			if (UISlider* pSlider = DYNAMIC_CAST(pContext->GetChildByName("SoundSlider"), UISlider))
			{
				pSlider->SetRatio(m_GameData->GetSoundVolume());
			}
		}
	}

	//! restarts the level
	void GameManager::Restart()
	{
		// TODO should restart current level not go to main menu
		SwitchContext(C_MainMenu);
	}

	//! OnFailed
	void GameManager::OnFailed()
	{
		if (m_eState == S_MissionFailed)
			return;

		Engine::Instance()->PushContext("common/ui/menupausebg.xml");
		Engine::Instance()->PushContext("common/ui/menumissionfailed.xml");
		SetState(S_MissionFailed);
	}

	//! OnSucceeded
	void GameManager::OnSucceeded()
	{
		if (m_eState == S_MissionSuceeded)
			return;

		Engine::Instance()->PushContext("common/ui/menupausebg.xml");
		Engine::Instance()->PushContext("common/ui/menumissionsucceeded.xml");
		SetState(S_MissionSuceeded);
		m_MenuSuccessTimer = 1.3f;
		m_MenuSuccessTouched = false;
		
		// game progression
		int lastLevelIndex = 17;
		auto newLevel = m_GameData->GetCurrentLevel() + 1;
		if (newLevel > lastLevelIndex)
		{
			// just finished last level, say thank you
			if (!m_GameData->GetSaidThankYou())
			{
				m_ThankYouPending = true;
				m_GameData->SetSaidThankYou(true);
			}
			newLevel = lastLevelIndex;
		}
		m_GameData->SetCurrentLevel(newLevel);
		if (newLevel > m_GameData->GetLastUnlockedLevel())
			m_GameData->SetLastUnlockedLevel(newLevel);
		m_GameData->Save();
	}

	//! SwitchContext
	void GameManager::SwitchContext(E_Context type)
	{
		auto& context = m_ContextInfos[type];
		SHOOT_ASSERT(context.m_Type == type, "Context type mismatch");

		DiscardPartitions(type);

		m_NavigationList.clear();
		GAMEMGR->SetState(context.m_StartingState);
		m_CurrentContext = &context;

		std::function<void()> loadingFinished = [&]()
		{
			GraphicsDriver::Instance()->LoadObjects();
			FileSystem::Instance()->UnloadAllPartitions("common");
		};
		Engine::Instance()->SetPartitionsToLoad(context.m_Partitions);
		Engine::Instance()->LoadContextStack(context.m_ContextStack.c_str(), false, loadingFinished);
	}

	//! SwitchContext
	void GameManager::SwitchContext(const std::string& contextStack)
	{
		for (auto& context : m_ContextInfos)
		{
			if (context.m_ContextStack == contextStack)
			{
				SwitchContext(context.m_Type);
				break;
			}
		}
	}	

	//! DiscardPartitions
	void GameManager::DiscardPartitions(E_Context nextContext)
	{
		auto& newContext = m_ContextInfos[nextContext];

		// determine partitions to discard
		std::vector<std::string> partitionsToDiscard;
		if (m_CurrentContext)
		{
			for (auto& partition : m_CurrentContext->m_Partitions)
			{
				auto it = std::find(newContext.m_Partitions.begin(), newContext.m_Partitions.end(), partition);
				if (it == newContext.m_Partitions.end())
					partitionsToDiscard.push_back(partition);
			}
		}

		for (auto& partition : partitionsToDiscard)
			FileSystem::Instance()->UnloadPartition(partition);
	}

	//! SetupContexts
	void GameManager::SetupContexts()
	{
		std::vector<std::string> menuPartitions;
		menuPartitions.push_back("menu");
		menuPartitions.push_back("envtuto");
		menuPartitions.push_back("env1");
		menuPartitions.push_back("blueskylow");
		menuPartitions.push_back("redsky");

		{
			// C_TouchToStart
			ContextInfo context;
			context.m_Type = C_TouchToStart;
			context.m_ContextStack = "menu/touchtostartscene.xml";
			context.m_StartingState = S_Menu;
			context.m_Partitions = menuPartitions;
			m_ContextInfos.push_back(context);
		}
		{
			// MainMenu
			ContextInfo context;
			context.m_Type = C_MainMenu;
			context.m_ContextStack = "menu/menuscenelevels.xml";
			context.m_StartingState = S_Menu;
			context.m_Partitions = menuPartitions;
			m_ContextInfos.push_back(context);
		}
		{
			// Tutorial
			ContextInfo context;
			context.m_Type = C_Tutorial;
			context.m_ContextStack = "tutorial/tutorial.xml";
			context.m_StartingState = S_InGame;
			context.m_Partitions.push_back("envtuto");
			context.m_Partitions.push_back("ingame");
			context.m_Partitions.push_back("tutorial");
			context.m_Partitions.push_back("bluesky");
			m_ContextInfos.push_back(context);
		}
		{
			// Level1
			ContextInfo context;
			context.m_Type = C_Level1;
			context.m_ContextStack = "1/level1.xml";
			context.m_StartingState = S_InGame;
			context.m_Partitions.push_back("env1");
			context.m_Partitions.push_back("ingame");
			context.m_Partitions.push_back("level1");
			context.m_Partitions.push_back("redsky");
			m_ContextInfos.push_back(context);
		}		
	}

	//! SwitchLevel
	void GameManager::SwitchLevel(const std::string& world, int level)
	{
		for (auto& context : m_ContextInfos)
		{
			auto idx = context.m_ContextStack.find(world);
			if (idx != std::string::npos)
			{
				context.m_ContextStack = world + "/" + world + "_" + Utils::ToString(level+1) + ".xml";
				SwitchContext(context.m_Type);
				break;
			}
		}		
	}	

	//! OnProductsRetrieved
	void GameManager::OnProductsRetrieved(bool success)
	{
		Log.Print("OnProductsRetrieved(%d)\n", success);
		SHOOT_ASSERT(m_eState == S_IAPInit, "Invalid Menu flow, expecting S_IAPInit, got '%d", m_eState);
		if (success)
		{
			Engine::Instance()->PopContext();
			m_OnPopUIEvent = "SWITCH menu/iapcontext.xml";
			m_eState = S_IAP;
			FadeRenderer::Instance()->StartFade(Color::Zero, Color::Black, 0.2f, false, true);
			m_ProductsRetrieved = true;
		}
		else
		{
			auto context = DYNAMIC_CAST(Engine::Instance()->GetTopContext(), UIContext);
			SHOOT_ASSERT(context->GetTemplatePath() == "menu/iapstatus.xml", "Invalid Menu flow, expecting iapstatus, got '%s", context->GetTemplatePath().c_str());
			auto status = static_cast<Text*>(context->GetChildByName("Status"));
			auto anim = status->GetComponent<SequenceVisitor>();
			anim->Leave();
			status->SetText("Couldn't retrieve upgrades");
			status->SetDesiredVertexCount(-1);
			context->GetChildByName("OK")->SetVisible(true);
			context->SetOnTouchEvent("POP");
			m_eState = S_Menu;
		}
	}

	//! OnTransactionFailed
	void GameManager::OnTransactionCompleted(bool success)
	{
		SHOOT_ASSERT(m_eState == S_IAPBuy || m_eState == S_IAPRestore, "Invalid Menu flow, expecting S_IAPBuy|S_IAPRestore, got '%d", m_eState);
		Log.Print("OnTransactionCompleted(%d)\n", success);		

		if (success)
		{
			Engine::Instance()->PopContext();
			if (auto iapContext = Engine::Instance()->GetContextStack()->GetChildByType<IAPContext>())
				iapContext->UpdateUI();
		}
		else
		{
			auto context = static_cast<UIContext*>(Engine::Instance()->GetTopContext());
			SHOOT_ASSERT(context->GetTemplatePath() == "menu/iapstatus.xml", "Invalid Menu flow, expecting iapstatus, got '%s", context->GetTemplatePath().c_str());
			auto status = static_cast<Text*>(context->GetChildByName("Status"));
			auto anim = status->GetComponent<SequenceVisitor>();
			anim->Leave();

			if (m_eState == S_IAPBuy)
				status->SetText("Transaction cancelled");
			else
				status->SetText("Couldn't restore purchases");
			status->SetDesiredVertexCount(-1);

			context->GetChildByName("OK")->SetVisible(true);
			context->SetOnTouchEvent("POP");
		}

		m_eState = S_IAP;
	}

	//! OnSignInCancelled
	void GameManager::OnSignInCancelled()
	{
		Log.Print("OnSignInCancelled()\n");
		Engine::Instance()->PopContext();
		m_eState = S_IAP;
	}

	//! OnSignInCancelled
	void GameManager::OnNothingToRestore()
	{
		SHOOT_ASSERT(m_eState == S_IAPRestore, "Invalid Menu flow, expecting S_IAPRestore, got '%d", m_eState);
		auto context = static_cast<UIContext*>(Engine::Instance()->GetTopContext());
		SHOOT_ASSERT(context->GetTemplatePath() == "menu/iapstatus.xml", "Invalid Menu flow, expecting iapstatus, got '%s", context->GetTemplatePath().c_str());
		auto status = static_cast<Text*>(context->GetChildByName("Status"));
		auto anim = status->GetComponent<SequenceVisitor>();
		anim->Leave();
		status->SetText("Nothing to restore");
		status->SetDesiredVertexCount(-1);
		context->GetChildByName("OK")->SetVisible(true);
		context->SetOnTouchEvent("POP");
		m_eState = S_IAP;
	}

	//! OnIAPInit
	void GameManager::OnIAPInit()
	{
		m_NavigationList.clear();
		if (m_ProductsRetrieved)
		{
			Engine::Instance()->GetTopContext()->Switch("menu/iapcontext.xml");			
			m_eState = S_IAP;
			return;
		}
		
#if SHOOT_PLATFORM == SHOOT_PLATFORM_IOS
		m_eState = S_IAPInit;		
		Engine::Instance()->PushContext("menu/iapstatus.xml");
		iOSRequestProducts();
#elif SHOOT_PLATFORM == SHOOT_PLATFORM_ANDROID
		std::wstring id = L"com.frantic.starports.extramode1";
		std::wstring name = L"FPS Mode";
		std::wstring price = L"0.99$";
		GetGameData()->AddProductInfo(id, name, price);
		Engine::Instance()->GetTopContext()->Switch("menu/iapcontext.xml");
		m_eState = S_IAP;
		m_ProductsRetrieved = true;
#else
		m_eState = S_IAPInit;
		std::function<void()> iapEvent = [&]()
		{
			auto success = Random::GetInt(0, 1) == 0;
			GAMEMGR->OnProductsRetrieved(success);
		};
		Engine::Instance()->PushContext("menu/iapstatus.xml");
		EventManager::Instance()->SendEvent(iapEvent, 2.0f);
#endif
	}

	//! ResetGameData
	void GameManager::ResetGameData()
	{
		m_GameData = snew GameData();
		m_GameData->Save();
	}
}

