/* 

Amine Rehioui
Created: August 22nd 2011

*/

#pragma once


#include <wx/menu.h>

namespace shoot
{
	//! MenuManager
	class MenuManager : public Singleton<MenuManager>
	{
		DECLARE_SINGLETON(MenuManager);

	public:

		//! Initializes the menu manager
		void Init();

		//! returns the main menu bar
		wxMenuBar* GetMainMenuBar() { return m_pMainMenu; }

		//! returns the type index for a class
		int GetTypeIndex(const char* strClassName);

	private:

		//! ClassInfo
		struct ClassInfo
		{
			std::string className;
			int typeIndex;
		};

		std::map< std::string, std::vector<ClassInfo> > m_aCategories;

		wxMenuBar* m_pMainMenu;
	};
}



