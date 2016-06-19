/* 

Amine Rehioui
Created: April 11th 2010

*/

#pragma once


using namespace shoot;

#define DECLARE_SHOOT_WX_PROPERTY(x) \
WX_PG_DECLARE_VARIANT_DATA(x) \
class wx##x##Property : public wxPGProperty \
{ \
	WX_PG_DECLARE_PROPERTY_CLASS(wx##x##Property) \
public: \
	wx##x##Property() { } \
    wx##x##Property(const wxString& label, const wxString& name, const x& value); \
	void RefreshChildren(); \
	wxVariant ChildChanged(wxVariant& thisValue, int childIndex, wxVariant& childValue ) const; \
};

DECLARE_SHOOT_WX_PROPERTY(Vector2)
DECLARE_SHOOT_WX_PROPERTY(Vector3)
DECLARE_SHOOT_WX_PROPERTY(AABBox2D)
DECLARE_SHOOT_WX_PROPERTY(AABBox3D)
DECLARE_SHOOT_WX_PROPERTY(Size)
DECLARE_SHOOT_WX_PROPERTY(Range)
DECLARE_SHOOT_WX_PROPERTY(Point)




