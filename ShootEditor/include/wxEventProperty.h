/* 

Amine Rehioui
Created: August 7th 2011

*/

#pragma once


// wxEventProperty
class wxEventProperty : public wxLongStringProperty
{
    WX_PG_DECLARE_PROPERTY_CLASS(wxEventProperty)

public:

    wxEventProperty(const wxString& name = wxPG_LABEL, const wxString& label = wxPG_LABEL, const wxString& value = wxEmptyString)
		: wxLongStringProperty(name, label, value)
	{
	}

    bool OnButtonClick(wxPropertyGrid* propgrid, wxString& value );
	void SetEventName(const std::string& strEventName) { m_strEventName = strEventName; }
	std::string GetEventName() const { return m_strEventName; }

private:

	std::string m_strEventName;
};



