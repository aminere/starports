/* 

Amine Rehioui
Created: August 9th 2010

*/

#include "ShootEditorCommon.h"

#include "ViewPortContainer.h"

#include "EditorSettings.h"
#include <wx/dcbuffer.h>

namespace shoot
{
	// Define event table
	BEGIN_EVENT_TABLE(ViewPortContainer, wxPanel)		
		EVT_SIZE(ViewPortContainer::OnResized)
		EVT_PAINT(ViewPortContainer::OnPaint)
		EVT_ERASE_BACKGROUND(ViewPortContainer::OnEraseBackground)
	END_EVENT_TABLE()

	//! Constructor
	ViewPortContainer::ViewPortContainer(wxWindow *parent)
		: super(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize)
		, _animTime(0.0f)
	{
		m_pViewPort = ShootEditor::Instance()->CreateViewPort(this);
	}

	// event handlers
	void ViewPortContainer::OnResized(wxSizeEvent& event)
	{
		event.Skip();

		if(EditorSettings::Instance()->WindowSize == EditorSettings::WS_Stretch)
		{
			m_pViewPort->SetPosition(wxPoint(0, 0));
			m_pViewPort->SetSize(event.GetSize());
		}
		else
		{
			Size size = EditorSettings::WindowSizes[EditorSettings::Instance()->WindowSize];
			wxPoint center = wxPoint((GetSize().x - size.Width)/2, (GetSize().y - size.Height)/2);
			m_pViewPort->SetPosition(center);
			m_pViewPort->SetSize(wxSize(size.Width, size.Height));
		}
	}

	void ViewPortContainer::OnPaint(wxPaintEvent& event)
	{
		wxBufferedPaintDC dc(this);

		if(m_pViewPort->IsShown())
			return;

		// Editor loading screen
		dc.SetBackground(*wxBLACK_BRUSH);
		dc.Clear();
		dc.SetFont(wxFont(wxFontInfo(12)));
		auto color = uchar(Math::FAbs(Math::Cos(_animTime*180.0f*Math::DegToRadFactor))*255.0f);
		dc.SetTextForeground(wxColor(color, color, color, 255));
		dc.DrawLabel(ShootEditor::Instance()->GetBackgroundThreadMessage(), wxRect(0, 0, GetSize().x, GetSize().y), wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL);
		auto deltaTime = float(_animWatch.Time())/1000.0f;
		_animTime += deltaTime;
		_animWatch.Start();
	}

	void ViewPortContainer::OnEraseBackground(wxEraseEvent& WXUNUSED(event))
	{
		// Do nothing, to avoid flashing.
	}
}
