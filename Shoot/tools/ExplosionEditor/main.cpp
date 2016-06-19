/* 

Amine Rehioui
Created: April 4th 2010

*/

#include "ShootEditorCommon.h"
#include "ExplosionEditor.h"
#include "AppSDL.h"

//! The class that implements the program entry point
class ExplosionEditorApp : public wxApp
{
public:

    //! called during initialization
	/** return: true if succeeded, false to abort the application */
    virtual bool OnInit();

	//! called during un-initialization
    virtual int OnExit();

private:

	ExplosionEditor* m_pExplosionEditor;
	shoot::Engine* m_pEngine;
};

//! Implement the main application methods
IMPLEMENT_APP(ExplosionEditorApp)

//! called during initialization
/** return: true if succeeded, false to abort the application */
bool ExplosionEditorApp::OnInit()
{
	if(!wxApp::OnInit())
	{
        return false;
	}
	
	new shoot::Engine();
	new shoot::AppSDL(shoot::Size(640, 480), false, false);

    // create the main application window
	m_pExplosionEditor = new ExplosionEditor(_T("Explosion Editor"), wxSize(640, 480));
    m_pExplosionEditor->Show();

    return true;
}

//! called during un-initialization
int ExplosionEditorApp::OnExit()
{
	delete shoot::Engine::Instance();
	delete shoot::App::Instance();

	return 0;
}

