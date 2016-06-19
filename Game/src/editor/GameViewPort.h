/* 

Amine Rehioui
Created: January 9th 2014

*/

#pragma once


#include "ViewPort.h"

namespace shoot
{
	class Tile;

	//! GameViewPort
	class GameViewPort : public ViewPort
	{
		typedef ViewPort super;

	public:

		//! Constructor
		GameViewPort(wxWindow *parent);

		//! event handlers
		void OnMouseMove(wxMouseEvent& event);
		void OnMouseLeftDown(wxMouseEvent& event);
		void OnMouseLeftUp(wxMouseEvent& event);

	private:

		//! returns the closest playfield tile
		Tile* GetTile(const Vector3& vIntersection, int& y, int& x) const;

		DECLARE_EVENT_TABLE()
	};
}



