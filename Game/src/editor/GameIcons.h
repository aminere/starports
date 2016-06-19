/* 

Amine Rehioui
Created: January 9th 2014

*/

#pragma once


#include "IconManager.h"

namespace shoot
{
	//! GameIcons
	class GameIcons : public IconManager
	{
		typedef IconManager super;

	public:

		//! icon types
		enum IconType
		{
			Icon_HexMode = Icon_Last+1,			
		};

		//! returns an icon by type
		wxBitmap GetBitmap(int type);
	};
}



