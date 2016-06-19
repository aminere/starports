/* 

Amine Rehioui
Created: October 5th 2013

*/

#include "Game.h"

#include "Event.h"
#include "GameManager.h"
#include "UIContext.h"
#include "Utils.h"
#include "AudioDriver.h"
#include "SequenceVisitor.h"
#include "CombineVisitor.h"
#include "FileSystem.h"
#include "AudioManager.h"
#include "Playfield.h"
#include "DefenderManager.h"
#include "LevelSelectContext.h"
#include "FollowCamera.h"

#include "EventManager.h"

extern void iOSBuyProduct(const std::wstring& id);
extern void iOSRestorePurchases();

namespace shoot
{
	//! HandleUIEvent
	void GameManager::HandleUIEvent(Event* pEvent)
	{
		GameManager* pMgr = this;
		UIEvent* pUIEvent = static_cast<UIEvent*>(pEvent);
		Entity* pSender = DYNAMIC_CAST(pUIEvent->m_Sender.Get(), Entity);
		Context* pContext = pSender ? pSender->GetRootContext() : NULL;
		UIContext* pUIContext = DYNAMIC_CAST(pContext, UIContext);
		if(pUIContext && pUIContext->GetState() != UIContext::S_Idle)
		{
			Log.Print("Skipping UI Event '%s' because context is in transition", pUIEvent->m_Command.c_str());
			return;
		}

		Log << "Command: " << pUIEvent->m_Command.c_str() << "\n";

		std::vector<std::string> tokens;
		Utils::Tokenize(pUIEvent->m_Command.c_str(), " ", tokens);
		if(tokens.empty())
		{
			SHOOT_WARNING(false, "Empty UI Command found");
			return;
		}

		std::string command = tokens[0];

		if(command == "WHEELMENU")
		{
			auto gameContext = static_cast<GameContext*>(pSender->GetRootContext());
			gameContext->m_Playfield->OnMenuAction(tokens[1]);
			return;
		}

		if (command == "CALLDOWN")
		{
			auto gameContext = static_cast<GameContext*>(pSender->GetRootContext());
			gameContext->m_DefenderManager->SpawnDefenders();
			return;
		}

		if(command == "POPPED")
		{
			if(!pMgr->m_OnPopUIEvent.empty())
			{
				tokens.clear();
				Utils::Tokenize(pMgr->m_OnPopUIEvent.c_str(), " ", tokens);
				command = tokens[0];
				pMgr->m_OnPopUIEvent.clear();
			}
			else
			{
				return;
			}
		}

		if(command == "SWITCH")
		{
			if (pContext)
			{
				GameManager::NavigationInfo info;
				info.m_eType = GameManager::NT_Switch;
				info.m_Context = pContext->GetTemplatePath();
				pMgr->m_NavigationList.push_back(info);
			}			

			std::string context = tokens[1];
			if (!pContext)
				pContext = Engine::Instance()->GetTopContext();
			pContext->Switch(context.c_str());
			return;
		}

		if(command == "CONTEXT")
		{
			pMgr->m_NavigationList.clear();
			auto contextStack = tokens[1];
			pMgr->SwitchContext(contextStack);
			return;
		}

		if(command == "PUSH")
		{
			GameManager::NavigationInfo info;
			info.m_eType = GameManager::NT_Push;
			pMgr->m_NavigationList.push_back(info);

			std::string context = tokens[1];
			Engine::Instance()->PushContext(context.c_str());
			return;
		}

		if(command == "POP")
		{
			if (pContext)
				pContext->Pop();
			else
				Engine::Instance()->PopContext();

			for (size_t i = 1; i<tokens.size(); ++i)
			{
				pMgr->m_OnPopUIEvent = pMgr->m_OnPopUIEvent + " " + tokens[i];
			}
			return;
		}

		if (command == "BACK")
		{
			if (pMgr->m_NavigationList.empty())
			{
				switch (pMgr->GetState())
				{
				case S_MenuOptions:
					command = "MENUOPTIONSEXIT";
					break;

				case S_InGame:
					command = "PAUSE";
					break;

				case S_InGamePause:
					command = "RESUME";
					break;

				case S_Menu:
					Engine::Instance()->RequestExit();
					return;

				case S_ThankYou:
					Engine::Instance()->PopContext();
					pMgr->SetState(S_Menu);
					return;

				case S_IAP:
					command = "IAPEXIT";
					break;

				default:
					return;
				}
			}
			else
			{
				GameManager::NavigationInfo info = pMgr->m_NavigationList.back();
				pMgr->m_NavigationList.pop_back();

				switch (info.m_eType)
				{
				case GameManager::NT_Switch:
					Engine::Instance()->GetTopContext()->Switch(info.m_Context.c_str());
					return;

				case GameManager::NT_Push:
					Engine::Instance()->PopContext();
					return;
				}		
			}
		}		

		if(command == "SLIDER")
		{
			std::string slider = tokens[1];
			float fRatio = Utils::FromString<float>(tokens[2]);
			if(slider == "MusicSlider")
			{
				AUDIOMGR->SetMusicVolume(fRatio);
				pMgr->m_GameData->SetMusicVolume(fRatio);
				pMgr->m_GameData->Save();
			}
			else if(slider == "SoundSlider")
			{
				AudioDriver::Instance()->SetSoundVolume(fRatio);
				pMgr->m_GameData->SetSoundVolume(fRatio);
				pMgr->m_GameData->Save();
			}
			return;
		}		

		if(command == "PAUSE")
		{
			pMgr->Pause();
			return;
		}

		if(command == "RESUME")
		{
			pMgr->Resume();
			return;
		}

		if (command == "IAPBUY")
		{
			auto id = tokens[1];
			std::wstring wstr(id.begin(), id.end());				

			#if SHOOT_PLATFORM == SHOOT_PLATFORM_IOS
				pMgr->SetState(S_IAPBuy);
				Engine::Instance()->PushContext("menu/iapstatus.xml");
				iOSBuyProduct(wstr);
			#elif SHOOT_PLATFORM == SHOOT_PLATFORM_ANDROID
				pMgr->m_HasBuyFPSModeRequest = true;
			#else
			pMgr->SetState(S_IAPBuy);
			std::function<void()> iapEvent = [&]()
			{
				auto success = Random::GetInt(0, 1) == 0;
				GAMEMGR->OnTransactionCompleted(success);
			};
			Engine::Instance()->PushContext("menu/iapstatus.xml");
			EventManager::Instance()->SendEvent(iapEvent, 2.0f);
			#endif
			return;
		}

		if (command == "IAPINIT")
		{
			pMgr->OnIAPInit();
			return;
		}

		if (command == "IAPRESTORE")
		{
			pMgr->SetState(S_IAPRestore);
			Engine::Instance()->PushContext("menu/iapstatus.xml");
			#if SHOOT_PLATFORM == SHOOT_PLATFORM_IOS				
				iOSRestorePurchases();
			#else
			std::function<void()> iapEvent = [&]()
			{
				auto success = Random::GetInt(0, 1) == 0;
				GAMEMGR->OnTransactionCompleted(success);
			};
			EventManager::Instance()->SendEvent(iapEvent, 2.0f);
			#endif
			return;
		}

		if (command == "IAPEXIT")
		{
			m_NavigationList.clear();
			pMgr->SetState(S_Menu);
			if (!pContext)
				pContext = Engine::Instance()->GetTopContext();
			pContext->Switch("menu/levelselectcontext.xml");
			return;
		}

		if (command == "FPSMODE")
		{
			FOLLOWCAM->OnFPSModeChanged(true);
			return;
		}

		if (command == "NORMALMODE")
		{
			FOLLOWCAM->OnFPSModeChanged(false);
			return;
		}

		if (command == "MENUOPTIONSENTER")
		{
			Engine::Instance()->PushContext("common/ui/menupausebg.xml");
			Engine::Instance()->PushContext("common/ui/menuoptionsoutgame.xml");
			pMgr->SetState(S_MenuOptions);
			return;
		}

		if (command == "MENUOPTIONSEXIT")
		{
			Engine::Instance()->PopContext();
			pMgr->m_OnPopUIEvent = "POP";
			pMgr->SetState(S_Menu);
			return;
		}

		if (command == "HACKWIN")
		{
			pMgr->OnSucceeded();
			return;
		}

		if (command == "HACKLOSE")
		{
			pMgr->OnFailed();
			return;
		}

		if (command == "HACKRESETDATA")
		{
			pMgr->ResetGameData();
			return;
		}

		Log.Print("Unknown UI command '%s'\n", pUIEvent->m_Command.c_str());
	}

	//! shoot event listeners
	void GameManager::HandleUITriggerEvent(Event* pEvent)
	{
		GameManager* pMgr = this;
		TriggerEvent* pTrigger = static_cast<TriggerEvent*>(pEvent);
		if(pTrigger->Name == "MENU_ANIM_DONE")
		{
			// ...
		}		
	}
}

