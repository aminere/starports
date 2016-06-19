/* 

Amine Rehioui
Created: April 10th 2010

*/

#pragma once


namespace shoot
{
	//! property list event listener
	class PropertyListListener
	{
	public:

		virtual void OnPropertyChanged(wxPropertyGridEvent &event) = 0;
		virtual void OnPropertyChanging(wxPropertyGridEvent &event) = 0;
		virtual void OnPropertySelected(wxPropertyGridEvent &event) = 0;
		virtual void OnPropertyRightClick(wxPropertyGridEvent &event) { }		
	};

	//! A class that allows the edition of shoot properties through a visual property list
	class PropertyList : public wxPropertyGrid
	{
		typedef wxPropertyGrid super;

	public:
		
		//! Constructor
		PropertyList(wxWindow* pParent,	
					 PropertyListListener* pListener,
					 wxWindowID id = wxID_ANY, 
					 const wxPoint& pos = wxDefaultPosition, 
					 const wxSize& size = wxSize(250, 300), 
					 long style = wxPG_SPLITTER_AUTO_CENTER | wxPG_BOLD_MODIFIED, 
					 const wxString& name = wxPropertyGridNameStr);

		void Fill(Object* pObject);
		void FillArrayProperties(IProperty* pProperty, wxPGProperty* pWxProperty);
		inline Object* GetSelectedObject() { return m_SelectedObject; }

		static wxPGProperty* GetWxProperty(IProperty* pProperty, PropertyList* pInstance);
		static void FillProperty(wxPGProperty* pWxProperty, IProperty* pProperty);

		wxPGEditor* ArrayEditor;
		wxPGEditor* ReferenceEditor;

		// Event handlers
		void OnPropertyGridChange(wxPropertyGridEvent& event); 
		void OnPropertyGridChanging(wxPropertyGridEvent& event);
		void OnPropertySelected(wxPropertyGridEvent& event);
		void OnPropertyRightClick(wxPropertyGridEvent& event);
		void OnMouseMove(wxMouseEvent& event);

	private:

		PropertyListListener* m_pListener;
		Handle<Object> m_SelectedObject;

		DECLARE_EVENT_TABLE();  
	};
}




