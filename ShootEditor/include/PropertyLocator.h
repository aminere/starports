/* 

Amine Rehioui
Created: May 29th 2010

*/

#pragma once


namespace shoot
{
	//!PropertyLocator - Locates an IProperty given a wxProperty
	class PropertyLocator
	{
	public:

		//! finds a Property in a stream based on a wxProperty
		IProperty* GetProperty(wxPGProperty* pWxProperty, PropertyStream& stream);	

		//! finds a root Property in a stream based on a wxProperty
		IProperty* GetRootProperty(wxPGProperty* pWxProperty, PropertyStream& stream);	

	private:

		//! recursively fill wxProperty child traversal info (keeps track of indexes from property to root)
		void FillWxTraversalInfo(wxPGProperty* pWxProperty);

		//! based on the traversal info, locates a Property in the root stream
		IProperty* GetProperty(IProperty* pParent);

		std::vector<int> m_aTraversalInfo;
	};
}



