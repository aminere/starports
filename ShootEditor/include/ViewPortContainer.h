/* 

Amine Rehioui
Created: August 9th 2010

*/

#pragma once


#include "ViewPort.h"

namespace shoot
{
	//! Container for the viewport
	class ViewPortContainer : public wxPanel							   
	{
		typedef wxPanel super;

	public:
		
		//! Constructor
		ViewPortContainer(wxWindow *parent);

		//! returns the view port
		ViewPort* GetViewPort() { return m_pViewPort; }

		// event handlers
		void OnResized(wxSizeEvent& event);
		void OnPaint(wxPaintEvent& event);
		void OnEraseBackground(wxEraseEvent& event);

	private:

		ViewPort* m_pViewPort;
		wxStopWatch _animWatch;
		float _animTime;

		DECLARE_EVENT_TABLE();
	};
}



