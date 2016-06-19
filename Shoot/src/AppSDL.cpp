/* 

Amine Rehioui
Created: August 4th 2010

*/

#include "Shoot.h"

#include "AppSDL.h"

#include "InputManagerSDL.h"

#ifdef DX11
#include "DirectX11Driver.h"
#endif // DX11

namespace shoot
{
	//! constructor
	AppSDL::AppSDL(Size screenSize /*= Size(640, 480)*/,
				   bool bFullScreen /*= false*/,
				   bool bUseSDLWindow /*= true*/, 
				   uchar BitsPerPixel /*= 24*/,
				   int SDLFlags /*= SDL_INIT_VIDEO*/,
				   int VideoFlags /*= SDL_OPENGL*/)
		: super(bFullScreen)
		, m_bUseSDLWindow(bUseSDLWindow)
	{
		int SDLStatus =  SDL_Init(SDLFlags);
		SHOOT_ASSERT(SDLStatus >= 0, "Could not initialize SDL");

		if(m_bUseSDLWindow)
		{
#ifdef DX11
			VideoFlags = 0;
#endif // DX11

			if(bFullScreen)
			{
				VideoFlags |= SDL_FULLSCREEN;
			}
			SDL_SetVideoMode(screenSize.Width, screenSize.Height, BitsPerPixel, VideoFlags);
			SDL_ShowCursor(SDL_DISABLE);

#ifdef DX11
			DirectX11Driver::Instance()->SetWindow(GetActiveWindow());
#endif // DX11
		}

		Engine::Instance()->ResizeScreen(Vector2::Create(float(screenSize.Width), float(screenSize.Height)));

		m_spInstance = this;
	}

	//! destructor
	AppSDL::~AppSDL()
	{
		Engine* pEngine = shoot::Engine::Instance();
		sdelete(pEngine);
		SDL_Quit();
	}	

	//! Engine's main loop
	void AppSDL::Run()
	{
		int previousTime = SDL_GetTicks();

		while(true)
		{			
			// calc delta time
			const int currentTime = SDL_GetTicks();			
			const float deltaTimeSecs = float(currentTime - previousTime) / 1000.0f; 
			previousTime = currentTime;

			if(Update(deltaTimeSecs))
			{
				Engine::Instance()->Render();

				if(GraphicsDriver::Instance()->GetType() == GraphicsDriver::DT_OpenGL)
				{
					SDL_GL_SwapBuffers();
				}
				else
				{
					GraphicsDriver::Instance()->Present();
				}
			}
			else
			{
				break;
			}
		}
	}

	//! updates the app
	bool AppSDL::Update(float fDeltaTime)
	{
		InputManager::Instance()->Update();

		if(m_bUseSDLWindow)
		{
			// Poll and handle for events
			SDL_Event event;
			while(SDL_PollEvent(&event)) 
			{
				if(event.type == SDL_KEYUP
				|| event.type == SDL_KEYDOWN) 
				{
					SDLKey sdlKey = event.key.keysym.sym;
					InputManager::E_KeyType shootKey = static_cast<InputManagerSDL*>(InputManager::Instance())->GetShootKeyType(sdlKey);
					InputManager::Instance()->SetKeyPressed(shootKey, (event.type == SDL_KEYDOWN));
				}
				else if(event.type == SDL_MOUSEBUTTONDOWN
					 || event.type == SDL_MOUSEBUTTONUP)
				{
					InputManager::TouchState state;
					state.bPressed = (event.type == SDL_MOUSEBUTTONDOWN);
					state.vPosition.Set(event.button.x, event.button.y);
					InputManager::Instance()->SetTouchState(state);
				}
				else if(event.type == SDL_MOUSEMOTION)
				{
					InputManager::TouchState state;
					state.bPressed = InputManager::Instance()->IsTouchPressed();
					state.vPosition.Set(event.button.x, event.button.y);
					InputManager::Instance()->SetTouchState(state);
				}
			} // end while (event)

			if(InputManager::Instance()->IsKeyJustPressed(InputManager::KT_Exit))
			{
				return false;
			}
		}

		return Engine::Instance()->Update(fDeltaTime);
	}
}

