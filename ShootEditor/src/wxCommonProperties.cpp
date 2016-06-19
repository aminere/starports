/* 

Amine Rehioui
Created: April 11th 2010

*/

#include "ShootEditorCommon.h"

#include "wxCommonProperties.h"

#include "EntitySelector.h"

WX_PG_IMPLEMENT_VARIANT_DATA_DUMMY_EQ(Vector2)
WX_PG_IMPLEMENT_VARIANT_DATA_DUMMY_EQ(Vector3)
WX_PG_IMPLEMENT_VARIANT_DATA_DUMMY_EQ(AABBox2D)
WX_PG_IMPLEMENT_VARIANT_DATA_DUMMY_EQ(AABBox3D)
WX_PG_IMPLEMENT_VARIANT_DATA_DUMMY_EQ(Size)
WX_PG_IMPLEMENT_VARIANT_DATA_DUMMY_EQ(Range)
WX_PG_IMPLEMENT_VARIANT_DATA_DUMMY_EQ(Point)

// Vector2
WX_PG_IMPLEMENT_PROPERTY_CLASS(wxVector2Property, wxPGProperty, Vector2, const Vector2&, TextCtrl)

wxVector2Property::wxVector2Property(const wxString& label, const wxString& name, const Vector2& value) 
: wxPGProperty(label, name)
{
    SetValue(WXVARIANT(value));
    AddPrivateChild(new wxFloatProperty(wxT("X"), wxPG_LABEL, value.X));
    AddPrivateChild(new wxFloatProperty(wxT("Y"), wxPG_LABEL, value.Y));	
}

void wxVector2Property::RefreshChildren()
{    
	if (!GetChildCount())
		return;
	auto vec = Vector2RefFromVariant(m_value);
	Item(0)->SetValue(vec.X);
	Item(1)->SetValue(vec.Y);
}

wxVariant wxVector2Property::ChildChanged( wxVariant& thisValue, int childIndex, wxVariant& childValue ) const
{	
    auto vec = Vector2RefFromVariant(thisValue);
    switch ( childIndex )
    {
		case 0: vec.X = childValue.GetDouble(); break;
        case 1: vec.Y = childValue.GetDouble(); break;
    }
    wxVariant newVariant;
    newVariant << vec;
    return newVariant;
}

// Vector3
WX_PG_IMPLEMENT_PROPERTY_CLASS(wxVector3Property, wxPGProperty, Vector3, const Vector3&, TextCtrl)

wxVector3Property::wxVector3Property(const wxString& label, const wxString& name, const Vector3& value) 
: wxPGProperty(label, name)
{
    SetValue(WXVARIANT(value));
    AddPrivateChild(new wxFloatProperty(wxT("X"), wxPG_LABEL, value.X));
    AddPrivateChild(new wxFloatProperty(wxT("Y"), wxPG_LABEL, value.Y));	
	AddPrivateChild(new wxFloatProperty(wxT("Z"), wxPG_LABEL, value.Z));	
}

void wxVector3Property::RefreshChildren()
{
	if (!GetChildCount())
		return;
    
	auto vec = Vector3RefFromVariant(m_value);
	Item(0)->SetValue(vec.X);
	Item(1)->SetValue(vec.Y);
	Item(2)->SetValue(vec.Z);	
}

wxVariant wxVector3Property::ChildChanged( wxVariant& thisValue, int childIndex, wxVariant& childValue ) const
{	
    auto vec = Vector3RefFromVariant(thisValue);
    switch ( childIndex )
    {
		case 0: vec.X = childValue.GetDouble(); break;
        case 1: vec.Y = childValue.GetDouble(); break;
		case 2: vec.Z = childValue.GetDouble(); break;
    }
	wxVariant newVariant;
    newVariant << vec;
    return newVariant;
}

// AABBox2D
WX_PG_IMPLEMENT_PROPERTY_CLASS(wxAABBox2DProperty, wxPGProperty, AABBox2D, const AABBox2D&, TextCtrl)

wxAABBox2DProperty::wxAABBox2DProperty(const wxString& label, const wxString& name, const AABBox2D& value) 
: wxPGProperty(label, name)
{
    SetValue(WXVARIANT(value));
    AddPrivateChild(new wxVector2Property(wxT("Min"), wxPG_LABEL, value.Min()));
    AddPrivateChild(new wxVector2Property(wxT("Max"), wxPG_LABEL, value.Max()));
}

void wxAABBox2DProperty::RefreshChildren()
{
	if (!GetChildCount())
		return;

	auto box = AABBox2DRefFromVariant(m_value);
	Item(0)->SetValue(WXVARIANT(box.Min()));
	Item(1)->SetValue(WXVARIANT(box.Max()));
}

wxVariant wxAABBox2DProperty::ChildChanged( wxVariant& thisValue, int childIndex, wxVariant& childValue ) const
{	
   auto box = AABBox2DRefFromVariant(thisValue);
    switch ( childIndex )
    {
		case 0: box.Set(Vector2RefFromVariant(childValue), box.Max()); break;
		case 1: box.Set(box.Min(), Vector2RefFromVariant(childValue)); break;
    }
	wxVariant newVariant;
    newVariant << box;
    return newVariant;
}

// AABBox3D
WX_PG_IMPLEMENT_PROPERTY_CLASS(wxAABBox3DProperty, wxPGProperty, AABBox3D, const AABBox3D&, TextCtrl)

wxAABBox3DProperty::wxAABBox3DProperty(const wxString& label, const wxString& name, const AABBox3D& value) 
: wxPGProperty(label, name)
{
    SetValue(WXVARIANT(value));
    AddPrivateChild(new wxVector3Property(wxT("Min"), wxPG_LABEL, value.Min()));
    AddPrivateChild(new wxVector3Property(wxT("Max"), wxPG_LABEL, value.Max()));
}

void wxAABBox3DProperty::RefreshChildren()
{
	if (!GetChildCount())
		return;

	auto box = AABBox3DRefFromVariant(m_value);
	Item(0)->SetValue(WXVARIANT(box.Min()));
	Item(1)->SetValue(WXVARIANT(box.Max()));
}

wxVariant wxAABBox3DProperty::ChildChanged(wxVariant& thisValue, int childIndex, wxVariant& childValue) const
{	
	auto box = AABBox3DRefFromVariant(thisValue);
	switch ( childIndex )
	{
	case 0: box.Set(Vector3RefFromVariant(childValue), box.Max()); break;
	case 1: box.Set(box.Min(), Vector3RefFromVariant(childValue)); break;
	}
	wxVariant newVariant;
	newVariant << box;
	return newVariant;
}

// Size
WX_PG_IMPLEMENT_PROPERTY_CLASS(wxSizeProperty, wxPGProperty, Size, const Size&, TextCtrl)

wxSizeProperty::wxSizeProperty(const wxString& label, const wxString& name, const Size& value) 
: wxPGProperty(label, name)
{
    SetValue(WXVARIANT(value));
    AddPrivateChild(new wxIntProperty(wxT("Width"), wxPG_LABEL, value.Width));
    AddPrivateChild(new wxIntProperty(wxT("Height"), wxPG_LABEL, value.Height));
}

void wxSizeProperty::RefreshChildren()
{
	if (!GetChildCount())
		return;

	auto size = SizeRefFromVariant(m_value);
	Item(0)->SetValue( (long)size.Width );
	Item(1)->SetValue( (long)size.Height );
}

wxVariant wxSizeProperty::ChildChanged( wxVariant& thisValue, int childIndex, wxVariant& childValue ) const
{	
    auto size = SizeRefFromVariant(thisValue);
    switch ( childIndex )
    {
		case 0: size.Width = childValue.GetInteger(); break;
        case 1: size.Height = childValue.GetInteger(); break;
    }
	wxVariant newVariant;
	newVariant << size;
	return newVariant;
}

// Point
WX_PG_IMPLEMENT_PROPERTY_CLASS(wxPointProperty, wxPGProperty, Point, const Point&, TextCtrl)

wxPointProperty::wxPointProperty(const wxString& label, const wxString& name, const Point& value) 
: wxPGProperty(label, name)
{
    SetValue(WXVARIANT(value));
    AddPrivateChild(new wxIntProperty(wxT("X"), wxPG_LABEL, value.X));
    AddPrivateChild(new wxIntProperty(wxT("Y"), wxPG_LABEL, value.Y));
}

void wxPointProperty::RefreshChildren()
{
	if (!GetChildCount())
		return;

	auto point = PointRefFromVariant(m_value);
	Item(0)->SetValue( (long)point.X );
	Item(1)->SetValue( (long)point.X );
}

wxVariant wxPointProperty::ChildChanged( wxVariant& thisValue, int childIndex, wxVariant& childValue ) const
{	
    auto point = PointRefFromVariant(thisValue);
    switch ( childIndex )
    {
		case 0: point.X = childValue.GetInteger(); break;
        case 1: point.Y = childValue.GetInteger(); break;
    }
	wxVariant newVariant;
	newVariant << point;
	return newVariant;
}

// Range
WX_PG_IMPLEMENT_PROPERTY_CLASS(wxRangeProperty, wxPGProperty, Range, const Range&, TextCtrl)

wxRangeProperty::wxRangeProperty(const wxString& label, const wxString& name, const Range& value) 
: wxPGProperty(label, name)
{
    SetValue(WXVARIANT(value));
    AddPrivateChild( new wxFloatProperty(wxT("Min"), wxPG_LABEL, value.Min) );
    AddPrivateChild( new wxFloatProperty(wxT("Max"), wxPG_LABEL, value.Max) );
}

void wxRangeProperty::RefreshChildren()
{
	if (!GetChildCount())
		return;

	auto range = RangeRefFromVariant(m_value);
	Item(0)->SetValue(range.Min);
	Item(1)->SetValue(range.Max);
}

wxVariant wxRangeProperty::ChildChanged( wxVariant& thisValue, int childIndex, wxVariant& childValue ) const
{	
    auto range = RangeRefFromVariant(thisValue);
    switch ( childIndex )
    {
		case 0: range.Min = childValue.GetDouble(); break;
        case 1: range.Max = childValue.GetDouble(); break;
    }
	wxVariant newVariant;
	newVariant << range;
	return newVariant;
}
