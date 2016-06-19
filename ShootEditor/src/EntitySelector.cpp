/* 

Amine Rehioui
Created: May 24th 2010

*/

#include "ShootEditorCommon.h"

#include "EntitySelector.h"

#include "ShootEditor.h"

#include "IconManager.h"

#include <wx/sizer.h>
#include <wx/button.h>

namespace shoot
{
	// Define event table
	BEGIN_EVENT_TABLE(EntitySelector, wxDialog)
		EVT_BUTTON(wxID_OK, EntitySelector::OnSelectButtonClicked)
		EVT_BUTTON(wxID_CANCEL, EntitySelector::OnCancelClicked)
		EVT_LIST_ITEM_SELECTED(wxID_ANY, EntitySelector::OnListItemSelected)
	END_EVENT_TABLE()

	//! Constructor
	EntitySelector::EntitySelector(wxWindow* pParent,
								   EntitySelectedCallback* pCallback,
								   const char* strBaseType /*= "Entity"*/,
								   const wxSize& size /*= wxDefaultSize*/,
								   wxWindowID id /*= wxID_ANY*/,
								   const wxString& title /*= "Entity Selector"*/,
								   const wxPoint& pos /*= wxDefaultPosition*/,					    
								   long style /*= wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER*/)
	: super(pParent, id, title, pos, size, style)
	, m_pCallBack(pCallback)
	, m_selectedItemID(0)
	{
		wxBoxSizer* pSizer = new wxBoxSizer(wxVERTICAL);
		
		m_pEntityList = new wxListView(this, wxID_ANY, wxDefaultPosition, wxDefaultSize);
		m_pEntityList->SetImageList(IconManager::Instance()->GetImageList(), wxIMAGE_LIST_SMALL);

		// Fill list with entities
		wxListItem column;
		column.SetText(_T("Name"));
	    column.SetImage(-1);
		column.SetWidth(100);
		m_pEntityList->InsertColumn(0, column);

		column.SetText(_T("ID"));
		m_pEntityList->InsertColumn(1, column);

		column.SetText(_T("Type"));
		m_pEntityList->InsertColumn(2, column);

		// Insert null item
		long item = m_pEntityList->InsertItem(0, "[NONE]", -1);		
		m_pEntityList->SetItemData(item, 0);
		m_pEntityList->SetItem(item, 1, "[NONE]");
		m_pEntityList->SetItem(item, 2, "[NONE]");
	
		// Insert entities		
		m_uiCurrentItem = 1;
		std::vector<std::string> typeFilter;
		if(strBaseType)
		{
			ObjectFactory::Instance()->GetCreatableChildClassNames(strBaseType, typeFilter);
		}		
		Fill(Engine::Instance()->GetContextStack(), typeFilter);

		pSizer->Add(m_pEntityList, 1, wxEXPAND | wxALL,	0);

		wxBoxSizer* pButtonSizer = new wxBoxSizer(wxHORIZONTAL);
		pButtonSizer->Add(new wxButton(this, wxID_CANCEL, _T("Cancel")), wxSizerFlags().Left().Border());		
		pButtonSizer->Add(new wxButton(this, wxID_OK, _T("Select")), wxSizerFlags().Right().Border());		
		pSizer->Add(pButtonSizer, wxSizerFlags().Expand().Center());		

		SetSizer(pSizer); // use the sizer for layout
		pSizer->FitInside(this);
	}	

	//! recursively fill the list with entities
	void EntitySelector::Fill(Entity* pEntity, std::vector<std::string>& typeFilter)
	{
		// check type filter
		std::vector<std::string>::iterator it = std::find(typeFilter.begin(), typeFilter.end(), std::string(pEntity->GetClassName()));
		bool typeAllowed = typeFilter.empty() || (it != typeFilter.end());

		if(typeAllowed)
		{
			// fill name column
			long item = m_pEntityList->InsertItem(m_uiCurrentItem++, pEntity->GetName(), -1);		
			m_pEntityList->SetItemData(item, pEntity->GetID());

			// fill ID column
			std::stringstream ss;
			ss << std::hex << pEntity->GetID();
			m_pEntityList->SetItem(item, 1, ss.str());
			 
			// fill type column
			m_pEntityList->SetItem(item, 2, pEntity->GetClassName(), IconManager::Instance()->GetIconIndex(pEntity));
		}

		for (size_t i = 0; i<pEntity->GetChildCount(); ++i)
		{
			Fill(pEntity->GetChild(i), typeFilter);
		}
	}

	// event handlers
	void EntitySelector::OnSelectButtonClicked(wxCommandEvent& event)
	{
		if(m_pCallBack(m_selectedItemID))
		{
			EndModal(0);
		}
	}

	void EntitySelector::OnCancelClicked(wxCommandEvent& event)
	{
		EndModal(0);
	}

	void EntitySelector::OnListItemSelected(wxListEvent& event)
	{
		m_selectedItemID = event.GetItem().GetData();
	}
}

