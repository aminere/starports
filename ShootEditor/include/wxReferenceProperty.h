/* 

Amine Rehioui
Created: March 26th 2011

*/

#pragma once


using namespace shoot;

// forwards
namespace shoot
{	
	class PropertyList;
}

// wxReferenceProperty
class wxReferenceProperty : public wxStringProperty
{
	typedef wxStringProperty super;

public:

	//! event types
	enum E_Event
	{
		E_PropertySetNew,
		E_PropertySetFromTemplate,
		E_PropertyCleared,
		E_SubPropertyChanged,
		E_PropertySaved,
		E_None
	};

	wxReferenceProperty( const wxString& label,	ReferenceProperty* pProperty, PropertyList* pParent );

	void Fill(ReferenceProperty* pProperty);
	void UpdateValue(ReferenceProperty* pProperty);

	inline void SetEventType(E_Event eEventType) { m_eEventType = eEventType; }
	inline E_Event GetEventType() const { return m_eEventType; }	
	inline PropertyList* GetParent() const { return m_pParent; }
	inline void SetClassName(std::string className) { m_ClassName = className; } 
	inline const std::string& GetClassName() const { return m_ClassName; }
	inline void SetTemplatePath(const std::string& path) { m_strTemplatePath = path; }
	inline const std::string& GetTemplatePath() const { return m_strTemplatePath; }

private:
	
	PropertyList* m_pParent;
	E_Event m_eEventType;
	std::string m_ClassName;
	std::string m_strTemplatePath;
};

//! wxReferencePropertyEditor
class wxReferencePropertyEditor : public wxPGTextCtrlEditor
{
    wxDECLARE_DYNAMIC_CLASS(wxReferencePropertyEditor);

public:

	wxPGWindowList CreateControls( wxPropertyGrid* propGrid,
		wxPGProperty* property,
		const wxPoint& pos,
		const wxSize& sz ) const;

	bool OnEvent( wxPropertyGrid* propGrid,
		wxPGProperty* property,
		wxWindow* ctrl,
		wxEvent& event ) const;
};



