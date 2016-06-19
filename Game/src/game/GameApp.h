
/*

Amine Rehioui
Created: March 22nd 2015

*/

#include "AppSDL.h"

namespace shoot
{
	//! GameApp
	class GameApp : public AppSDL
	{
	public:

		//! constructor
		GameApp(Size screenSize,
			bool bFullScreen,
			bool bUseSDLWindow)
			: AppSDL(screenSize, bFullScreen, bUseSDLWindow)
			, m_MousePressed(false)
		{
		}

		//! updates the app
		bool Update(float fDeltaTime);

	private:

		bool m_MousePressed;
	};
}

