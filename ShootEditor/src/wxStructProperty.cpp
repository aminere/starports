/* 

Amine Rehioui
Created: May 27th 2010

*/

#include "ShootEditorCommon.h"

#include "wxStructProperty.h"

#include "PropertyList.h"

WX_PG_IMPLEMENT_VARIANT_DATA_DUMMY_EQ(wxPGChildChangeInfo)
WX_PG_IMPLEMENT_PROPERTY_CLASS(wxStructProperty, wxPGProperty, wxPGChildChangeInfo, const wxPGChildChangeInfo&, TextCtrl)

wxStructProperty::wxStructProperty(PropertyList* pParent, const wxString& label, const wxString& name, PropertyStream& stream) 
: wxPGProperty(label, name)
, m_pParent(pParent)
{
	wxPGChildChangeInfo childChangeInfo;
	SetValue(WXVARIANT(childChangeInfo));
}

void wxStructProperty::Fill(StructProperty* pProperty)
{
	auto &stream = pProperty->GetStream();
	for (size_t i = 0; i<stream.GetNumProperties(); ++i)
	{
		IProperty* pProperty = stream.GetProperty(i);
		wxPGProperty* pWxProperty = PropertyList::GetWxProperty(pProperty, m_pParent);
		InsertChild(-1, pWxProperty);

		bool bParentDisabled = !GetGrid()->IsPropertyEnabled(this);
		if(bParentDisabled)
			GetGrid()->DisableProperty(pWxProperty);
	}
}

void wxStructProperty::RefreshChildren()
{	
	auto childChangeInfo = wxPGChildChangeInfoRefFromVariant(m_value);
	Item(childChangeInfo.childIndex)->SetValue(childChangeInfo.childValue);
}

wxVariant wxStructProperty::ChildChanged( wxVariant& thisValue, int childIndex, wxVariant& childValue ) const
{	
	auto childChangeInfo = wxPGChildChangeInfoRefFromVariant(thisValue);
	childChangeInfo.childIndex = childIndex;
	childChangeInfo.childValue = childValue;
	wxVariant newVariant;
    newVariant << childChangeInfo;
    return newVariant;
}

