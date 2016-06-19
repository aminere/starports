
/*

Amine Rehioui
Created: March 22nd 2015

*/

#include "Game.h"

#include "GameApp.h"
#include "InputManagerSDL.h"
#include "InputRecorder.h"
#include "EventManager.h"

namespace shoot
{
	//! updates the app
	bool GameApp::Update(float fDeltaTime)
	{
		InputManager::Instance()->Update();

		if (m_bUseSDLWindow)
		{
			// Poll and handle for events
			SDL_Event event;
			while (SDL_PollEvent(&event))
			{
				if (event.type == SDL_KEYUP
				 || event.type == SDL_KEYDOWN)
				{
					SDLKey sdlKey = event.key.keysym.sym;
					InputManager::E_KeyType shootKey = static_cast<InputManagerSDL*>(InputManager::Instance())->GetShootKeyType(sdlKey);
					InputManager::Instance()->SetKeyPressed(shootKey, (event.type == SDL_KEYDOWN));
				}
				else if (event.type == SDL_MOUSEBUTTONDOWN
					|| event.type == SDL_MOUSEBUTTONUP)
				{
					auto buttonIndex = event.button.button;
					if (buttonIndex == SDL_BUTTON_LEFT)
					{
						InputManager::TouchState state;
						state.bPressed = (event.type == SDL_MOUSEBUTTONDOWN);
						state.vPosition.Set(event.button.x, event.button.y);
						InputManager::Instance()->SetTouchState(state);
						m_MousePressed = state.bPressed;

						// if in replay mode, go back to start screen
						auto gameMgr = GameEngine::Instance()->GetGameMgr();
						if (gameMgr->GetState() == GameManager::S_Replay)
						{
							if (state.bPressed)
							{
								InputRecorder::Instance()->StopReplay();
								gameMgr->ResetMenuTimer();
							}
						}
						else if (gameMgr->GetState() == GameManager::S_Menu)
						{
							gameMgr->ResetMenuTimer();
						}
					}
					else if (buttonIndex == SDL_BUTTON_WHEELUP)
					{
						shoot::WheelEvent* pEvent = snew shoot::WheelEvent();
						pEvent->m_Up = true;
						EventManager::Instance()->SendEvent(pEvent);						
					}
					else if (buttonIndex == SDL_BUTTON_WHEELDOWN)
					{
						shoot::WheelEvent* pEvent = snew shoot::WheelEvent();
						pEvent->m_Up = false;
						EventManager::Instance()->SendEvent(pEvent);						
					}
				}
				else if (event.type == SDL_MOUSEMOTION)
				{
					InputManager::TouchState state;
					state.bPressed = m_MousePressed;
					state.vPosition.Set(event.button.x, event.button.y);
					InputManager::Instance()->SetTouchState(state);
				}
			} // end while (event)

			if (InputManager::Instance()->IsKeyJustPressed(InputManager::KT_Exit))
			{
				return false;
			}
		}

		return Engine::Instance()->Update(fDeltaTime);
	}
}

