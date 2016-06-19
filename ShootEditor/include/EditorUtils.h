/* 

Amine Rehioui
Created: May 1st 2011

*/

#pragma once


#include "InputManager.h"

namespace shoot
{
	//! Editor utilities class
	class EditorUtils
	{
	public:

		//! Browse for a file utility
		/** return: true if the user completed the browse operation */
		static bool OpenFileDialog(wxWindow* pParent, std::string& strOut, const char* strFilter = NULL, const char* strTitle = "Open File", long style = wxFD_OPEN);

		//! formats an object path
		static void FormatObjectPath(Object* pObject, std::string& strPath);

		//! converts from a wx key code to a shoot key code
		static InputManager::E_KeyType GetShootKeyType(int wxKeyCode);

		//! returns a relative path into the data folder
		static bool GetRelativePath(const std::string& strPath, std::string& strOut);

		//! GetTemplatePath
		static std::string GetTemplatePath(Object* object);

		//! GetObjectName
		static std::string GetObjectName(Object* object);

		//! struct to map between the tree items and the components
		struct ItemData : public wxTreeItemData
		{
			ItemData(Object* pObject) : m_pObject(pObject)
			{
			}
			Object* m_pObject;
		};
	};
}



