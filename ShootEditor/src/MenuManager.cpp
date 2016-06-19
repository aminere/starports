/* 

Amine Rehioui
Created: August 22nd 2011

*/

#include "ShootEditorCommon.h"

#include "MenuManager.h"

#include "CameraManager.h"

#include "IconManager.h"

namespace shoot
{
	//! constructor
	MenuManager::MenuManager()
	{	
	}

	//! destructor
	MenuManager::~MenuManager()
	{
	}

	//! Initializes the menu manager
	void MenuManager::Init()
	{
		// File menu
		wxMenu* pFileMenu = new wxMenu;
		//pFileMenu->Append(ID_Tab_New, _T("New Tab"), _T("New Tab"));
		//pFileMenu->AppendSeparator();
		pFileMenu->Append(wxID_EXIT, _T("E&xit\tAlt-X"), _T("Quit this program"));

		// Edit menu
		wxMenu* pEditMenu = new wxMenu;
		pEditMenu->Append(ID_Menu_Duplicate, _T("Duplicate\tCtrl-D"), _T("Duplicate an entity"));	
		pEditMenu->Append(ID_Menu_Remove, _T("Remove\tDEL"), _T("Remove an entity"));
		pEditMenu->AppendSeparator();
		pEditMenu->AppendCheckItem(ID_Menu_ToggleGameCameraControl, "Edit Game Camera", "Edit Game Camera");
		pEditMenu->Check(ID_Menu_ToggleGameCameraControl, CameraManager::Instance()->GetGameCameraControl());

		wxMenu* pInsertMenu = new wxMenu;
		int numTypes = ObjectFactory::Instance()->GetNumCreatableTypes();
		SHOOT_ASSERT(numTypes <= MaxEntityTypes, "MaxEntityTypes reached");
		for(int i=0; i<numTypes; ++i)
		{			
			std::string className = ObjectFactory::Instance()->GetCreatableClassName(i);
			if(ObjectFactory::Instance()->IsDerived(className, Entity::GetStaticClassName()))
			{
				std::string category = ShootEditor::Instance()->GetMenuCategory(className.c_str());
				if(category.empty())
					category = "Default";
				
				ClassInfo info = { className, i };
				m_aCategories[category].push_back(info);				
			}		
		}

		// hack to add EntityTemplate to core group
		ClassInfo info = { "EntityTemplate", numTypes+1 };
		m_aCategories["Core"].push_back(info);

		for(std::map< std::string, std::vector<ClassInfo> >::iterator it = m_aCategories.begin(); 
			it != m_aCategories.end(); 
			it++)
		{
			wxMenu* pSubMenu = new wxMenu;
			std::string category = (*it).first;
			const std::vector<ClassInfo>& classInfos = (*it).second;

			for (size_t i=0; i<classInfos.size(); ++i)
			{
				std::string className = classInfos[i].className;
				wxMenuItem *pItem = new wxMenuItem(pSubMenu, ID_Menu_Insert_FirstEntity+classInfos[i].typeIndex, className);
				pItem->SetBitmap(IconManager::Instance()->GetBitmap(className));
				pSubMenu->Append(pItem);
			}

			pInsertMenu->Append(wxID_ANY, category, pSubMenu);
		}

		// Windows menu
		wxMenu* pWindowsMenu = new wxMenu;
		pWindowsMenu->Append(ID_Menu_ShowSettings, "Editor Settings");		
		pWindowsMenu->Append(ID_Menu_ShowConsole, "Console");	

		// Help menu
		wxMenu* pHelpMenu = new wxMenu;
		pHelpMenu->Append(wxID_ABOUT, _T("&About...\tF1"), _T("Show about dialog"));

		// Create menu bar
		m_pMainMenu = new wxMenuBar();
		m_pMainMenu->Append(pFileMenu, _T("&File"));
		m_pMainMenu->Append(pEditMenu, _T("&Edit"));
		m_pMainMenu->Append(pInsertMenu, _T("&Insert"));
		m_pMainMenu->Append(pWindowsMenu, _T("&Windows"));
		m_pMainMenu->Append(pHelpMenu, _T("&Help"));
	}

	//! returns the type index for a class
	int MenuManager::GetTypeIndex(const char* strClassName)
	{
		for(std::map< std::string, std::vector<ClassInfo> >::iterator it = m_aCategories.begin(); 
			it != m_aCategories.end(); 
			it++)
		{
			const std::vector<ClassInfo>& classInfos = (*it).second;
			for (size_t i=0; i<classInfos.size(); ++i)
			{
				if(classInfos[i].className == strClassName)
				{
					return classInfos[i].typeIndex;
				}
			}
		}

		SHOOT_ASSERT(false, "MenuManager::GetTypeIndex() failed");
		return -1;
	}
}

