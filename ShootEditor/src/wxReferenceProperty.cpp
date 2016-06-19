/* 

Amine Rehioui
Created: March 26th 2011

*/

#include "ShootEditorCommon.h"

#include "wxReferenceProperty.h"

#include "PropertyList.h"

#include "EditorUtils.h"

wxReferenceProperty::wxReferenceProperty( const wxString& label, ReferenceProperty* pProperty, PropertyList* pParent )
: wxStringProperty(label, wxPG_LABEL)
, m_eEventType(E_None)
, m_pParent(pParent)
{
	SetClassName(pProperty->GetRefInterface()->GetClassName());
}

void wxReferenceProperty::Fill(ReferenceProperty* pProperty)
{
	PropertyStream& stream = pProperty->GetStream();
	for (size_t i = 0; i<stream.GetNumProperties(); ++i)
	{
		IProperty* pProperty = stream.GetProperty(i);
		wxPGProperty* pWxProperty = PropertyList::GetWxProperty(pProperty, m_pParent);
		InsertChild(-1, pWxProperty);
		bool bParentDisabled = !GetGrid()->IsPropertyEnabled(this);
		if(bParentDisabled)
			GetGrid()->DisableProperty(pWxProperty);		

		if(pProperty->GetType() == PT_Array
			|| pProperty->GetType() == PT_Reference
			|| pProperty->GetType() == PT_Struct)
		{
			GetParent()->FillArrayProperties(pProperty, pWxProperty);	
		}

		if(pProperty->GetName() == "ID")
		{
			pWxProperty->SetAttribute(wxPG_UINT_BASE, wxPG_BASE_HEX);
			pWxProperty->Enable(false);
		}
	}

	// update property name	
	if(!pProperty->GetName().empty())
		SetLabel(pProperty->GetName());

	UpdateValue(pProperty);
	GetParent()->RefreshProperty(this);
}

void wxReferenceProperty::UpdateValue(ReferenceProperty* pProperty)
{
	std::stringstream ss;
	IReference* pReference = pProperty ? pProperty->GetRefInterface() : NULL;
	Object* pObject = pReference ? pReference->Get() : NULL;
	if(pObject)
	{
		if(!pObject->GetTemplatePath().empty())		
			ss << pObject->GetTemplatePath();
		else
			ss << "[" << std::hex << pObject->GetID() << "]";
	}
	else
	{
		ss << "null";
	}	
	SetValue(wxVariant(ss.str()));
}

// wxReferencePropertyEditor
IMPLEMENT_DYNAMIC_CLASS(wxReferencePropertyEditor, wxPGTextCtrlEditor)

wxPGWindowList wxReferencePropertyEditor::CreateControls( wxPropertyGrid* propGrid,
														  wxPGProperty* property,
														  const wxPoint& pos,
														  const wxSize& sz ) const
{
    // Create and populate buttons-subwindow
    wxPGMultiButton* buttons = new wxPGMultiButton( propGrid, sz );
    buttons->Add("+"); // create new
    buttons->Add("-"); // clear
	buttons->Add("b"); // browse existing
	buttons->Add("s"); // save

    // Create the 'primary' editor control (textctrl in this case)
    wxPGWindowList wndList = wxPGTextCtrlEditor::CreateControls( propGrid, property, pos, buttons->GetPrimarySize());

    // Finally, move buttons-subwindow to correct position and make sure
    // returned wxPGWindowList contains our custom button list.
    buttons->Finalize(propGrid, pos);

    wndList.SetSecondary( buttons );
    return wndList;
}

bool wxReferencePropertyEditor::OnEvent( wxPropertyGrid* propGrid,
										 wxPGProperty* property,
										 wxWindow* ctrl,
										 wxEvent& event ) const
{
    if ( event.GetEventType() == wxEVT_COMMAND_BUTTON_CLICKED )
    {
        wxPGMultiButton* buttons = (wxPGMultiButton*) propGrid->GetEditorControlSecondary();
		wxReferenceProperty* pRefProperty = static_cast<wxReferenceProperty*>(property);

		if(event.GetId() == buttons->GetButtonId(0)) // + set reference
		{
			// update the corresponding property in the property stream
			wxPropertyGridEvent PGEvent;
			PGEvent.SetProperty(property);
			pRefProperty->SetEventType(wxReferenceProperty::E_PropertySetNew);
			pRefProperty->GetParent()->OnPropertyGridChange(PGEvent);
		}	
		else if(event.GetId() == buttons->GetButtonId(1)) // - clear reference
		{
			int childCount = property->GetChildCount();
			if(childCount > 0)
			{			
				// remove the wx property
				property->DeleteChildren();			
				pRefProperty->UpdateValue(NULL);
				propGrid->RefreshProperty(property);				

				// update the corresponding property in the property stream
				wxPropertyGridEvent PGEvent;
				PGEvent.SetProperty(property);
				pRefProperty->SetEventType(wxReferenceProperty::E_PropertyCleared);
				pRefProperty->GetParent()->OnPropertyGridChange(PGEvent);
			}
		}
		else if(event.GetId() == buttons->GetButtonId(2)) // b - browse existing
		{
			// helper to browse meshes and textures
			std::string filter = "Object files (*.xml)|*.xml";
			if (pRefProperty->GetClassName() == Texture::GetStaticClassName())
				filter = "Textures (*.png)|*.png";
			else if (pRefProperty->GetClassName() == MeshResource::GetStaticClassName())
				filter = "Meshes (*.fbx)|*.fbx";

			std::string path;
			if (EditorUtils::OpenFileDialog(pRefProperty->GetParent(), path, filter.c_str(), "Select Object"))
			{
				// make sure the path is towards an XML file
				path = Utils::GetFilePathNoExt(path) + ".xml";

				// update the corresponding property in the property stream
				wxPropertyGridEvent PGEvent;
				PGEvent.SetProperty(property);
				pRefProperty->SetEventType(wxReferenceProperty::E_PropertySetFromTemplate);
				pRefProperty->SetTemplatePath(path);
				pRefProperty->GetParent()->OnPropertyGridChange(PGEvent);		
			}
		}	
		else if(event.GetId() == buttons->GetButtonId(3)) // s - save to file
		{
			if(pRefProperty->GetChildCount() != 0)
			{
				// update the corresponding property in the property stream
				wxPropertyGridEvent PGEvent;
				PGEvent.SetProperty(property);
				pRefProperty->SetEventType(wxReferenceProperty::E_PropertySaved);				
				pRefProperty->GetParent()->OnPropertyGridChange(PGEvent);		
			}
		}

		return true;
    }
	else
	{
		return wxPGTextCtrlEditor::OnEvent(propGrid, property, ctrl, event);
	}    
}


