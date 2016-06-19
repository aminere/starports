/* 

Amine Rehioui
Created: August 7th 2010

*/

#include "ShootEditorCommon.h"
#include "EditorSettingsDialog.h"
#include "EditorSettings.h"

#include "PropertyLocator.h"
#include "wxArrayProperty.h"
#include "wxReferenceProperty.h"

namespace shoot
{
	// Define event table
	BEGIN_EVENT_TABLE(EditorSettingsDialog, wxDialog)
	END_EVENT_TABLE()

	//! Constructor
	EditorSettingsDialog::EditorSettingsDialog(wxWindow* pParent) 
	: super(pParent, wxID_ANY, wxString("Editor Settings"), wxDefaultPosition, wxSize(400, 500))
	{
		PropertyList* pPropertyList = new PropertyList(this, this);
		pPropertyList->Fill(EditorSettings::Instance());		

		wxBoxSizer *pSizer = new wxBoxSizer( wxVERTICAL );
		pSizer->Add(pPropertyList, 
					1, // make vertically stretchable
					wxEXPAND | // make horizontally stretchable
					wxALL, //   and make border all around
					0); // set border width to 10		

		SetSizer(pSizer); // use the sizer for layout
		pSizer->FitInside(this);
	}	

	//! call on property change
	void EditorSettingsDialog::OnPropertyChanged(wxPropertyGridEvent &event)
	{
		wxPGProperty* pWxProperty = event.GetProperty();

		PropertyStream stream(shoot::SM_Write);
		EditorSettings::Instance()->Serialize(stream);		

		// check if this is a sub property
		if(!pWxProperty->GetParent()->IsRoot()) 
		{			
			IProperty* pParentProperty = PropertyLocator().GetProperty(pWxProperty->GetParent(), stream);

			// if this was part of an array, mark it
			if(pParentProperty->GetType() == PT_Array)
			{	
				wxArrayProperty* pWxArrayParent = static_cast<wxArrayProperty*>(pWxProperty->GetParent());
				pWxArrayParent->SetEventType(wxArrayProperty::E_SubPropertyChanged);
				pWxArrayParent->SetChangedSubPropertyIndex(pWxProperty->GetIndexInParent());
			}
			else if(pParentProperty->GetType() == PT_Reference)
			{
				wxReferenceProperty* pWxRefParent = static_cast<wxReferenceProperty*>(pWxProperty->GetParent());
				pWxRefParent->SetEventType(wxReferenceProperty::E_SubPropertyChanged);				
			}
			
			PropertyList::FillProperty(pWxProperty->GetParent(), pParentProperty);

			stream.SetMode(shoot::SM_Read);			
			stream.SetPropertyToUpdate(PropertyLocator().GetRootProperty(pWxProperty, stream)->GetName());
			EditorSettings::Instance()->Serialize(stream);
		}
		else
		{
			std::string strPropertyName(pWxProperty->GetName());
			IProperty* pProperty = stream.GetProperty(strPropertyName);
			SHOOT_ASSERT(pProperty, "Unknow property");
			PropertyList::FillProperty(pWxProperty, pProperty);

			stream.SetMode(shoot::SM_Read);
			stream.SetPropertyToUpdate(strPropertyName);
			EditorSettings::Instance()->Serialize(stream);					
		}
	}

	//! called while a property is changing
	void EditorSettingsDialog::OnPropertyChanging(wxPropertyGridEvent &event)
	{
		// TODO
	}
}

