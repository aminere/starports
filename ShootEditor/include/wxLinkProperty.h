/* 

Amine Rehioui
Created: May 24th 2010

*/

#pragma once


using namespace shoot;

// wxLinkProperty
class wxLinkProperty : public wxLongStringProperty
{
    WX_PG_DECLARE_PROPERTY_CLASS(wxLinkProperty)

public:
    wxLinkProperty( const wxString& name = wxPG_LABEL, const wxString& label = wxPG_LABEL, const wxString& value = wxEmptyString)
		: wxLongStringProperty(name, label, value)
	{
	}

    bool OnButtonClick( wxPropertyGrid* propgrid, wxString& value);

	void SetLinkID(uint ID) { m_LinkID = ID; }
	uint GetLinkID() const { return m_LinkID; }
	void SetBaseType(const char* strBaseType) { m_strBaseType = strBaseType; }

	static bool OnEntityLinkChanged(uint entityID);
	static wxLinkProperty* GetCurrentProperty() { return ms_pCurrentProperty; }

private:

	static wxLinkProperty* ms_pCurrentProperty;
	uint m_LinkID;
	const char* m_strBaseType;
};



