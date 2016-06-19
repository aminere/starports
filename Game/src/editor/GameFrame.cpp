/* 

Amine Rehioui
Created: January 9th 2014

*/

#include "GameEditorCommon.h"

#include "GameFrame.h"
#include "GameEditor.h"
#include "GameEditorIDs.h"
#include "GameToolbar.h"
#include "GameIcons.h"

namespace shoot
{
	// define the event table
	BEGIN_EVENT_TABLE(GameFrame, MainFrame)
		EVT_MENU(ID_Menu_ToggleHexMode, GameFrame::OnToggleHexMode)
	END_EVENT_TABLE()

	//! constructor
	GameFrame::GameFrame()
		: super(_T("Starports: SpaceDefense"), wxSize(1024, 768), new GameEditor(), new GameToolbar(), new GameIcons())
	{
		#ifdef SHOOT_64
			SetTitle(GetTitle() + " (64 Bit)");
		#endif
	}

	void GameFrame::OnToggleHexMode(wxCommandEvent& event)
	{
		GameEditor::Instance()->SetHexMode(!GameEditor::Instance()->GetHexMode());
	}
}

