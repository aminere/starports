/* 

Amine Rehioui
Created: April 4th 2011

*/

#pragma once


namespace shoot
{
	//! Shows a dialog to create objects of compatible types
	class ObjectSelector
	{
	public:
		
		//! constructor
		ObjectSelector(wxWindow* pParent) : m_pParent(pParent)
		{
		}

		//! Shows a dialog to select types compatible with a given class name
		bool Select(std::string strClassName, std::string& strSelectedClassName);

	private:

		wxWindow* m_pParent;
	};
}




