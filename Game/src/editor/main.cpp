/* 

Amine Rehioui
Created: April 4th 2010

*/

#include "GameEditorCommon.h"
#include "GameEngine.h"
#include "AppSDL.h"
#include "GameFrame.h"

//! Define this on the game side to register entities
extern void RegisterObjects();

//! The class that implements the program entry point
class Application : public wxApp
{
public:

    //! called during initialization
	/** return: true if succeeded, false to abort the application */
    virtual bool OnInit();

	//! called during un-initialization
    virtual int OnExit();
};

//! Implement the main application methods
IMPLEMENT_APP(Application)

//! called during initialization
/** return: true if succeeded, false to abort the application */
bool Application::OnInit()
{
    if(!wxApp::OnInit())
	{
        return false;
	}
	
	new shoot::GameEngine(&RegisterObjects);
	new shoot::AppSDL(shoot::Size(640, 480), false, false);
    (new shoot::GameFrame())->Show();

    return true;
}

//! called during un-initialization
int Application::OnExit()
{
	delete shoot::Engine::Instance();
	delete shoot::App::Instance();
	return 0;
}

