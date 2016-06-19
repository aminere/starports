/* 

Amine Rehioui
Created: August 22nd 2011

*/

#include "GameEditorCommon.h"

#include "GameIcons.h"

#include "../../EditorData/hexagon.xpm"

namespace shoot
{
	//! returns an icon by type
	wxBitmap GameIcons::GetBitmap(int type)
	{
		switch(type)
		{
		case Icon_HexMode: return wxBitmap(hexagon_xpm);
		default: return super::GetBitmap(type);
		}
	}
}

