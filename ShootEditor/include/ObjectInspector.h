/* 

Amine Rehioui
Created: January 16th 2012

*/

#pragma once


#include "wx/treectrl.h"

#include "PropertyList.h"

namespace shoot
{
	// forwards
	class Entity;

	//! Inspects entities
	class ObjectInspector : public wxPanel
						  , public PropertyListListener
	{
		typedef wxPanel super;

	public:

		//! Constructor
		ObjectInspector(wxWindow* pParent,	
					    wxWindowID id = wxID_ANY, 
					    const wxPoint& pos = wxDefaultPosition, 
					    const wxSize& size = wxSize(300, 300), 
					    long style = 0);

		//! Destructor
		virtual ~ObjectInspector()
		{
		}

		//! Fills the object inspector
		void Fill(Object* pObject);

		//! loads an object from a path
		void Load(Object* pObject, const std::string& path);

		//! updates a property
		void UpdateProperty(const char* strPropertyName, wxVariant& value);

		//! returns the property list
		inline PropertyList* GetPropertyList() const { return m_pPropertyList; }

		// PropertyListListener interface //

		//! call on property change
		void OnPropertyChanged(wxPropertyGridEvent &event);

		//! called while a property is changing
		void OnPropertyChanging(wxPropertyGridEvent &event);

		//! called while a property is selected
		void OnPropertySelected(wxPropertyGridEvent &event);

		//! called while a property is right clicked
		void OnPropertyRightClick(wxPropertyGridEvent &event);

	private:

		//! adds a component to an entity
		void AddComponent(Entity* pEntity, Component* pComponent);

		//! GetObjectRoot
		wxTreeItemId GetObjectRoot() const;

		wxTreeCtrl* m_pTreeControl;
		PropertyList* m_pPropertyList;
		wxButton* m_pReloadButton;
		wxButton* m_pComponentCopyButton;
		wxButton* m_pComponentPasteButton;
		Reference<Component> m_ComponentClipboard;		

		// event handlers
		void OnLoad(wxCommandEvent& event);
		void OnSave(wxCommandEvent& event);
		void OnSaveAs(wxCommandEvent& event);
		void OnReload(wxCommandEvent& event);		
		void OnAddComponent(wxCommandEvent& event);
		void OnRemoveComponent(wxCommandEvent& event);
		void OnCopyComponent(wxCommandEvent& event);
		void OnPasteComponent(wxCommandEvent& event);
		void OnSelectItem(wxTreeEvent& event);
		void OnTreeItemMenu(wxTreeEvent& event);
		void OnTreeItemMenuClicked(wxEvent& event);
		void OnPropertyMenuClicked(wxEvent& event);

		//!PropertyMenuData
		struct PropertyMenuData
		{
			Object* pObject;
			Object* pPropertyObject;
			wxPGProperty* pWxProperty;
			int propertyIndex;
		};

		PropertyMenuData m_PropertyMenuData;

		DECLARE_EVENT_TABLE();
	};
}




