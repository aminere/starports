/* 

Amine Rehioui
Created: May 29th 2010

*/

#include "ShootEditorCommon.h"

#include "PropertyLocator.h"

namespace shoot
{
	//! finds a Property in a stream based on a wxProperty
	IProperty* PropertyLocator::GetProperty(wxPGProperty* pWxProperty, PropertyStream& stream)
	{
		m_aTraversalInfo.clear();
		FillWxTraversalInfo(pWxProperty);

		IProperty* pFirstChild = stream.GetProperty(m_aTraversalInfo.back());
		m_aTraversalInfo.erase(m_aTraversalInfo.end()-1);
		IProperty* pProperty = GetProperty(pFirstChild);

		return pProperty;
	}

	//! finds a root Property in a stream based on a wxProperty
	IProperty* PropertyLocator::GetRootProperty(wxPGProperty* pWxProperty, PropertyStream& stream)
	{
		m_aTraversalInfo.clear();
		FillWxTraversalInfo(pWxProperty);

		IProperty* pFirstChild = stream.GetProperty(m_aTraversalInfo.back());
		return pFirstChild;
	}

	//! recursively fill wxProperty child traversal info (keeps track of indexes from property to root)
	void PropertyLocator::FillWxTraversalInfo(wxPGProperty* pWxProperty)
	{	
		if(pWxProperty->IsRoot())
			return;
		
		int indexInParent = pWxProperty->GetIndexInParent();

		// if the parent is the root, remove one to account for the editor "Type" property
		if(pWxProperty->GetParent()->IsRoot())
			indexInParent -= 1;

		m_aTraversalInfo.push_back(indexInParent);

		FillWxTraversalInfo(pWxProperty->GetParent());
	}

	//! based on the traversal info, locates a Property in a property stream
	IProperty* PropertyLocator::GetProperty(IProperty* pParent)
	{	
		if(m_aTraversalInfo.size() == 0)		
			return pParent;
		
		int childIndex = m_aTraversalInfo.back();
		m_aTraversalInfo.erase(m_aTraversalInfo.end()-1);

		switch(pParent->GetType())
		{
		case PT_Array:
			{
				ArrayProperty* pArrayProperty = static_cast<ArrayProperty*>(pParent);
				auto child = pArrayProperty->GetProperties()[childIndex];
				return GetProperty(child);
			}

		case PT_Struct:
		case PT_Reference:
			{
				StructProperty* pStructProperty = static_cast<StructProperty*>(pParent);
				auto child = pStructProperty->GetStream().GetProperty(childIndex);
				return GetProperty(child);
			}

		default:
			SHOOT_ASSERT(0, "PropertyLocator traversal error");			
		}
		
		return NULL;
	}
}

