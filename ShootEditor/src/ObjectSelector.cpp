/* 

Amine Rehioui
Created: April 4th 2011

*/

#include "ShootEditorCommon.h"

#include "ObjectSelector.h"

#include "wx/choicdlg.h"

namespace shoot
{
	//! Shows a dialog to select types compatible with a given class name
	bool ObjectSelector::Select(std::string strClassName, std::string& strSelectedClassName)
	{		
		std::vector<std::string> types;
		ObjectFactory::Instance()->GetCreatableChildClassNames(strClassName, types);		

		int numTypes = types.size();
		wxString* choices = new wxString[numTypes];
		for(int i=0; i<numTypes; ++i)
		{
			choices[i] = types[i];
		}
		
		wxSingleChoiceDialog dialog(m_pParent, "Select Object Type", "Select Object Type", numTypes, choices);		
		dialog.SetSelection(0);
		bool selected = (dialog.ShowModal() == wxID_OK);
		delete[] choices;

		if(selected)
		{
			strSelectedClassName = types[dialog.GetSelection()];
			return true;
		}
		else
		{
			return false;
		}		
	}
}

