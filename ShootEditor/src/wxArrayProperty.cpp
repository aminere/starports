/* 

Amine Rehioui
Created: May 28th 2010

*/

#include "ShootEditorCommon.h"

#include "wxArrayProperty.h"

#include "PropertyList.h"

wxArrayProperty::wxArrayProperty( const wxString& label, ArrayProperty* pProperty, PropertyList* pParent )
: wxStringProperty(label, wxPG_LABEL)
, m_eEventType(E_None)
, m_pSubPropertyTemplate(NULL)
, m_pStructTemplate(NULL)
, m_pRefTemplate(NULL)
, m_pLinkTemplate(NULL)
, m_pParent(pParent)
{
	m_pSubPropertyTemplate = IProperty::CreateFromType(pProperty->GetSubType());

	switch(m_pSubPropertyTemplate->GetType())
	{
	case PT_Struct:
		{
			StructProperty* pStructProperty = static_cast<StructProperty*>(m_pSubPropertyTemplate);
			pStructProperty->GetStream().SetMode(SM_Write);
			m_pStructTemplate = static_cast<ISerializableStruct*>(pProperty->GetArray()->Alloc());		
			m_pStructTemplate->Serialize(pStructProperty->GetStream());
		}
		break;

	case PT_Reference:
		{
			m_pRefTemplate = static_cast<IReference*>(pProperty->GetArray()->Alloc());		
			ReferenceProperty* pRefProperty = static_cast<ReferenceProperty*>(m_pSubPropertyTemplate);			
			pRefProperty->SetRefInterface(m_pRefTemplate);
		}
		break;

	case PT_Link:
		{
			m_pLinkTemplate = static_cast<ILink*>(pProperty->GetArray()->Alloc());		
			LinkProperty* pLinkProperty = static_cast<LinkProperty*>(m_pSubPropertyTemplate);			
			pLinkProperty->SetData(m_pLinkTemplate);
		}
	}

	UpdateValue();
}

wxArrayProperty::~wxArrayProperty()
{
	sdelete(m_pSubPropertyTemplate);
	sdelete(m_pStructTemplate);
	sdelete(m_pRefTemplate);
	sdelete(m_pLinkTemplate);
}

void wxArrayProperty::Fill(ArrayProperty* pProperty)
{
	int numProperties = pProperty->GetProperties().size();	
	for(int i=0; i<numProperties; ++i)
	{
		IProperty* pSubProperty = pProperty->GetProperties()[i];
		InitPropertyName(pSubProperty, i);

		wxPGProperty* pWxProperty = PropertyList::GetWxProperty(pSubProperty, m_pParent);
		InsertChild(-1, pWxProperty);
		bool bParentDisabled = !GetGrid()->IsPropertyEnabled(this);
		if(bParentDisabled)
			GetGrid()->DisableProperty(pWxProperty);
		pWxProperty->SetExpanded(false);
	}

	UpdateValue();
}

//! updates the array property value
void wxArrayProperty::UpdateValue()
{
	// display number of elements
	int numElements = this->GetChildCount();
	std::stringstream ss;
	ss << numElements << " elems";
	SetValue(wxVariant(ss.str()));
}

void wxArrayProperty::InitPropertyName(IProperty* property, int index)
{
	std::stringstream ss;
	ss << "[" << index << "]";
	if(property->GetType() == PT_Reference)
	{
		auto refProperty = static_cast<ReferenceProperty*>(property);
		auto refObject = refProperty->GetRefInterface() ? refProperty->GetRefInterface()->Get() : NULL;
		if(refObject)
			ss << " " << refObject->GetClassName();
	}
	property->SetName(ss.str());
}

//! implement the array property editor
IMPLEMENT_DYNAMIC_CLASS(wxArrayPropertyEditor, wxPGTextCtrlEditor)

wxPGWindowList wxArrayPropertyEditor::CreateControls( wxPropertyGrid* propGrid,
                                                      wxPGProperty* property,
                                                      const wxPoint& pos,
                                                      const wxSize& sz ) const
{
    // Create and populate buttons-subwindow
    wxPGMultiButton* buttons = new wxPGMultiButton( propGrid, sz );	
    buttons->Add("+");
    buttons->Add("-");
	buttons->Add("c");

    // Create the 'primary' editor control (textctrl in this case)
    wxPGWindowList wndList = wxPGTextCtrlEditor::CreateControls( propGrid, property, pos, buttons->GetPrimarySize());

    // Finally, move buttons-subwindow to correct position and make sure
    // returned wxPGWindowList contains our custom button list.
    buttons->Finalize(propGrid, pos);

    wndList.SetSecondary( buttons );
    return wndList;
}

bool wxArrayPropertyEditor::OnEvent( wxPropertyGrid* propGrid,
                                     wxPGProperty* property,
                                     wxWindow* ctrl,
                                     wxEvent& event ) const
{
    if ( event.GetEventType() == wxEVT_COMMAND_BUTTON_CLICKED )
    {
        wxPGMultiButton* buttons = (wxPGMultiButton*) propGrid->GetEditorControlSecondary();
		wxArrayProperty* pArrayProperty = static_cast<wxArrayProperty*>(property);

		if(event.GetId() == buttons->GetButtonId(0)) // + - add element
		{
			// determine new property name
			IProperty* pNewProperty = pArrayProperty->GetSubPropertyTemplate();
			pArrayProperty->InitPropertyName(pNewProperty, property->GetChildCount());

			// create a new wxPGProperty out of it and insert it
			wxPGProperty* pWxProperty = PropertyList::GetWxProperty(pNewProperty, pArrayProperty->GetPropertyList());
			property->InsertChild(-1, pWxProperty);

			if(pNewProperty->GetType() == PT_Reference
			|| pNewProperty->GetType() == PT_Struct)
				pArrayProperty->GetPropertyList()->FillArrayProperties(pNewProperty, pWxProperty);

			pArrayProperty->UpdateValue();
			propGrid->RefreshProperty(property);

			// update the corresponding property in the property stream
			wxPropertyGridEvent PGEvent;
			PGEvent.SetProperty(property);
			pArrayProperty->SetEventType(wxArrayProperty::E_PropertyAdded);
			pArrayProperty->GetPropertyList()->OnPropertyGridChange(PGEvent);
		}	
		else if(event.GetId() == buttons->GetButtonId(1)) // - remove element
		{
			int childCount = property->GetChildCount();
			if(childCount > 0)
			{
				// remove the wx property
				wxPGProperty* pChild = property->Item(childCount-1);
				propGrid->DeleteProperty(pChild);	
				pArrayProperty->UpdateValue();
				propGrid->RefreshProperty(property);

				// update the corresponding property in the property stream
				wxPropertyGridEvent PGEvent;
				PGEvent.SetProperty(property);
				pArrayProperty->SetEventType(wxArrayProperty::E_PropertyRemoved);
				pArrayProperty->GetPropertyList()->OnPropertyGridChange(PGEvent);
			}
		}
		else if(event.GetId() == buttons->GetButtonId(2)) // clear all
		{
			// remove all wx properties
			pArrayProperty->DeleteChildren();
			pArrayProperty->UpdateValue();
			propGrid->RefreshProperty(pArrayProperty);

			// update the corresponding property in the property stream
			wxPropertyGridEvent PGEvent;
			PGEvent.SetProperty(property);
			pArrayProperty->SetEventType(wxArrayProperty::E_PropertiesCleared);
			pArrayProperty->GetPropertyList()->OnPropertyGridChange(PGEvent);
		}
		return true;
    }
	else
	{
		return wxPGTextCtrlEditor::OnEvent(propGrid, property, ctrl, event);
	}    
}

