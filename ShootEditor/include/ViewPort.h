/* 

Amine Rehioui
Created: April 10th 2010

*/

#pragma once


#include "Camera.h"

#include "wx/glcanvas.h"
#include "wx/stopwatch.h"

namespace shoot
{
	// forwards
	class Entity2D;
		
	//! The rendering area of the editor
	class ViewPort : public wxGLCanvas
	{
		typedef wxGLCanvas super;

	public:

		//! Constructor
		ViewPort(wxWindow *parent);

		//! Destructor
		virtual ~ViewPort();

		//! returns the viewport 3D camera
		Camera* Get3DCamera() { return m_Default3DCamera; }

		//! returns the viewport 2D camera
		Camera* Get2DCamera() { return m_Default2DCamera; }

		//! resets the view
		void ResetView();

		//! SwitchToEditor3DCamera
		void SwitchToEditor3DCamera();

		// event handlers
		void OnMouseMove(wxMouseEvent& event);
		void OnMouseLeftDown(wxMouseEvent& event);
		void OnMouseLeftUp(wxMouseEvent& event);
		void OnMouseMiddleDown(wxMouseEvent& event);
		void OnMouseMiddleUp(wxMouseEvent& event);
		void OnMouseRightDown(wxMouseEvent& event);
		void OnMouseRightUp(wxMouseEvent& event);
		void OnMouseWheelRolled(wxMouseEvent& event);
		void OnKeyDown(wxKeyEvent& event);
		void OnKeyUp(wxKeyEvent& event);
		void OnMouseLeave(wxMouseEvent& event);
		void OnResized(wxSizeEvent& event);
		void OnFocusGained(wxFocusEvent& event);
		void OnFocusLost(wxFocusEvent& event);
		void OnPaint(wxPaintEvent& event);
		void OnEraseBackground(wxEraseEvent& event);

	protected:

		//! returns the 2D entity under a specific point
		Entity2D* GetSelectedEntity(Entity* pParent, const Vector2& clickPosition);

		//! returns the 3D entity selected by a 3D ray
		void GetSelectedEntity(Entity* pParent, const Vector3& vRayStart, const Vector3& vRayDir, Entity3D*& pResult);

		//! checks if camera move is allowed in the current frame
		bool IsCameraMoveAllowed();

		bool m_bDraggingStarted;
		Vector2 m_LastMousePosition;
		float m_fEntity2DStartRotation;
		Vector2 m_vEntity2DStartScale;

		float m_fDistToClosestPickedEntity;

		bool m_bZoomingStarted;
		bool m_bStrafingStarted;
		bool m_bLookingStarted;

		Reference<Camera> m_Default3DCamera;
		Reference<Camera> m_Default2DCamera;

		wxStopWatch m_StopWatch;
		wxGLContext* m_pGLContext;
		
		DECLARE_EVENT_TABLE();
	};
}



