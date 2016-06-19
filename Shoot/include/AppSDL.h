/* 

Amine Rehioui
Created: August 4th 2010

*/

#pragma once


#include "App.h"
#include <SDL.h>

namespace shoot
{
	//! AppSDL class
	class AppSDL : public App
	{
		typedef App super;

	public:

		//! constructor
		AppSDL(Size screenSize = Size(800, 600),
			   bool bFullScreen = false,
			   bool bUseSDLWindow = true,
			   uchar _BitsPerPixel = 24, 
			   int SDLFlags = SDL_INIT_VIDEO, 
			   int VideoFlags = SDL_OPENGL);

		//! destructor
		virtual ~AppSDL();

		//! main loop
		void Run();

		//! updates the app
		virtual bool Update(float fDeltaTime);

	protected:

		bool m_bUseSDLWindow;
	};
}



