/* 

Amine Rehioui
Created: August 22nd 2011

*/

#include "GameEditorCommon.h"

#include "GameToolbar.h"
#include "GameEditorIDs.h"
#include "GameIcons.h"

namespace shoot
{
	//! Initializes the tool bar
	void GameToolbar::Init()
	{
		super::Init();

		IconManager* pIconMgr = IconManager::Instance();
		m_pMainToolbar->AddTool(ID_Menu_ToggleHexMode, wxT("Hexagon Mode"), pIconMgr->GetBitmap(GameIcons::Icon_HexMode), wxT("Hexagon Mode"), wxITEM_CHECK);
		m_pMainToolbar->ToggleTool(ID_Menu_ToggleHexMode, false);

		m_pMainToolbar->Realize();
	}
}

