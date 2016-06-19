/* 

Amine Rehioui
Created: August 20th 2011

*/

#pragma once


#include <wx/imaglist.h>
#include <wx/bitmap.h>

namespace shoot
{
	// forwards
	class Object;

	//! IconManager
	class IconManager : public Singleton<IconManager>
	{
		DECLARE_SINGLETON(IconManager);

	public:

		//! icon types
		enum IconType
		{
			Icon_Translate,
			Icon_Rotate,
			Icon_Scale,
			Icon_Pin,
			Icon_PostRendering,
			Icon_Audio,
			Icon_3D,
			Icon_GameMouse,
			Icon_Add,
			Icon_Remove,
			Icon_Play,
			Icon_Record,
			Icon_Replay,
			Icon_Pause,
			Icon_Stop,
			Icon_Undo,
			Icon_Redo,
			Icon_ArrowUp,
			Icon_ArrowDown,
			Icon_Last
		};

		//! gets the wx image list
		wxImageList* GetImageList() { return m_pImageList; }

		//! returns the icon index within the image list for a particular object		
		int GetIconIndex(Object* pObject);

		//! returns the icon corresponding to an entity
		wxBitmap GetBitmap(const std::string& strClassName);

		//! returns an icon by type
		virtual wxBitmap GetBitmap(int type);

	private:

		wxImageList* m_pImageList;

		std::map<std::string, wxBitmap> m_IconMap;
	};
}




