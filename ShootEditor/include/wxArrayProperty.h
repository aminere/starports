/* 

Amine Rehioui
Created: May 28th 2010

*/

#pragma once


using namespace shoot;

// forwards
namespace shoot
{	
	class PropertyList;
}

// -----------------------------------------------------------------------
// wxArrayProperty
// -----------------------------------------------------------------------
class wxArrayProperty : public wxStringProperty
{
	typedef wxStringProperty super;

public:

	//! event types
	enum E_Event
	{
		E_PropertyAdded,
		E_PropertyRemoved,
		E_SubPropertyChanged,
		E_PropertiesCleared,
		E_None
	};

	wxArrayProperty(const wxString& label,	ArrayProperty* pProperty, PropertyList* pParent);
	virtual ~wxArrayProperty();

	inline void SetEventType(E_Event eEventType) { m_eEventType = eEventType; }
	inline E_Event GetEventType() const { return m_eEventType; }
	inline void SetChangedSubPropertyIndex(int index) { m_iChangedSubPropertyIndex = index; }
	inline int GetChangedSubPropertyIndex() const { return m_iChangedSubPropertyIndex; }	
	inline IProperty* GetSubPropertyTemplate() const { return m_pSubPropertyTemplate; }
	inline ISerializableStruct* GetStructTemplate() const { return m_pStructTemplate; }
	inline PropertyList* GetPropertyList() const { return m_pParent; }

	void Fill(ArrayProperty* pProperty);
	void UpdateValue();	
	void InitPropertyName(IProperty*, int);

private:

	IProperty* m_pSubPropertyTemplate;
	ISerializableStruct* m_pStructTemplate;
	IReference* m_pRefTemplate;
	ILink* m_pLinkTemplate;
	E_Event m_eEventType;
	int m_iChangedSubPropertyIndex;	
	PropertyList* m_pParent;
};

//! wxArrayProperty editor
class wxArrayPropertyEditor : public wxPGTextCtrlEditor
{
    wxDECLARE_DYNAMIC_CLASS(wxArrayPropertyEditor);

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



