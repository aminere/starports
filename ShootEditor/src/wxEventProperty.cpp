/* 

Amine Rehioui
Created: August 7th 2011

*/

#include "ShootEditorCommon.h"

#include "wxEventProperty.h"

#include "Event.h"
#include "EventManager.h"

WX_PG_IMPLEMENT_PROPERTY_CLASS(wxEventProperty, wxPGProperty, wxString, const wxString&, TextCtrlAndButton)

//! button click callback
bool wxEventProperty::OnButtonClick(wxPropertyGrid* propgrid, wxString& value)
{
	auto event = shoot::ObjectFactory::Instance()->Create(m_strEventName);
	SHOOT_ASSERT(event->IsA(shoot::Event::TypeID), "Object is not of type Event");
	shoot::EventManager::Instance()->SendEvent(static_cast<shoot::Event*>(event));
    return true;
}

