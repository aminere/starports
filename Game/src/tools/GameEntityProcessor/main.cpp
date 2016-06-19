/* 

Amine Rehioui
Created: April 26th 2014

*/

#include "Game.h"
#include "GameEngine.h"

#include "AppSDL.h"
#include "GameEntityProcessor.h"

//! Entity registration function
extern void RegisterObjects();

int main(int argc, char** argv)
{
	{
		shoot::GameEngine* pEngine = new shoot::GameEngine(&RegisterObjects);
		shoot::AppSDL app(shoot::Size(800, 600), false, true);
		pEngine->Init();

		shoot::GameEntityProcessor().Process();
	}	
	return 0;
}

