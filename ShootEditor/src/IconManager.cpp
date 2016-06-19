/* 

Amine Rehioui
Created: August 20th 2011

*/

#include "ShootEditorCommon.h"

#include "IconManager.h"

#include "wx/artprov.h" // wxArtProvider
#include "wx/image.h"

#include "bitmaps/translate.xpm"
#include "bitmaps/rotate.xpm"
#include "bitmaps/scale.xpm"
#include "bitmaps/pin.xpm"
#include "bitmaps/EditorPostRendering.xpm"
#include "bitmaps/audio.xpm"
#include "bitmaps/_3D.xpm"
#include "bitmaps/add.xpm"
#include "bitmaps/remove.xpm"
#include "bitmaps/play.xpm"
#include "bitmaps/record.xpm"
#include "bitmaps/replay.xpm"
#include "bitmaps/pause.xpm"
#include "bitmaps/stop.xpm"
#include "bitmaps/undo.xpm"
#include "bitmaps/redo.xpm"
#include "bitmaps/arrow_up.xpm"
#include "bitmaps/arrow_down.xpm"
#include "bitmaps/game_mouse.xpm"

#include "Camera.h"
#include "Image.h"
#include "Text.h"
#include "ParticleGenerator.h"
#include "MeshEntity.h"

#include "bitmaps/Camera.xpm"
#include "bitmaps/EntityTemplate.xpm"
#include "bitmaps/Image.xpm"
#include "bitmaps/Text.xpm"
#include "bitmaps/ParticleGenerator.xpm"
#include "bitmaps/MeshEntity.xpm"
#include "bitmaps/ContextStack.xpm"
#include "bitmaps/Context.xpm"

namespace shoot
{
	//! constructor
	IconManager::IconManager()
		: m_pImageList(NULL)
	{	
		wxSize size(16, 16);
		m_pImageList = new wxImageList(size.x, size.y, true);
		m_pImageList->Add(wxArtProvider::GetBitmap(wxART_NORMAL_FILE, wxART_OTHER, size));
		m_IconMap[Camera::GetStaticClassName()] = wxBitmap(Camera_xpm);
		m_IconMap["EntityTemplate"] = wxArtProvider::GetBitmap(wxART_FOLDER_OPEN, wxART_OTHER, size);
		m_IconMap[Image::GetStaticClassName()] = wxBitmap(Image_xpm);
		m_IconMap[Text::GetStaticClassName()] = wxBitmap(Text_xpm);
		m_IconMap[ParticleGenerator::GetStaticClassName()] = wxBitmap(ParticleGenerator_xpm);
		m_IconMap[MeshEntity::GetStaticClassName()] = wxBitmap(MeshEntity_xpm);
		m_IconMap[ContextStack::GetStaticClassName()] = wxBitmap(ContextStack_xpm);
		m_IconMap[Context::GetStaticClassName()] = wxBitmap(Context_xpm);
		
		for(std::map<std::string, wxBitmap>::iterator it = m_IconMap.begin(); it != m_IconMap.end(); it++)
		{
			m_pImageList->Add((*it).second);
		}
	}

	//! destructor
	IconManager::~IconManager()
	{
		delete m_pImageList;
	}	

	//! returns the icon index within the image list for a particular object		
	int IconManager::GetIconIndex(Object* pObject)
	{
		bool bFound = false;
		std::map<std::string, wxBitmap>::iterator it;

		if(pObject)
		{
			it = m_IconMap.find(pObject->GetClassName());
			bFound = (it != m_IconMap.end());
		}
		
		if(bFound)
		{
			int index = 1+std::distance(m_IconMap.begin(), it);
			return index;
		}
		else
		{
			return 0;
		}		
	}

	//! returns the icon corresponding to an entity
	wxBitmap IconManager::GetBitmap(const std::string& className)
	{
		std::map<std::string, wxBitmap>::iterator it = m_IconMap.find(className);
		if(it != m_IconMap.end())
		{
			return (*it).second;
		}
		else
		{
			return m_pImageList->GetBitmap(0);
		}
	}

	//! returns an icon by type
	wxBitmap IconManager::GetBitmap(int type)
	{
		wxBitmap bitmap;

		switch(type)
		{
		case Icon_Translate: bitmap = wxBitmap(translate_xpm); break;
		case Icon_Rotate: bitmap = wxBitmap(rotate_xpm); break;
		case Icon_Scale: bitmap = wxBitmap(scale_xpm); break;
		case Icon_Pin: bitmap = wxBitmap(pin_xpm); break;
		case Icon_PostRendering: bitmap = wxBitmap(EditorPostRendering_xpm); break;
		case Icon_Audio: bitmap = wxBitmap(audio_xpm); break;
		case Icon_3D: bitmap = wxBitmap(_3D_xpm); break;
		case Icon_Add: bitmap = wxBitmap(add_xpm); break;
		case Icon_Remove: bitmap = wxBitmap(remove_xpm); break;
		case Icon_Play: bitmap = wxBitmap(play_xpm); break;
		case Icon_Record: bitmap = wxBitmap(record_xpm); break;
		case Icon_Replay: bitmap = wxBitmap(replay_xpm); break;
		case Icon_Pause: bitmap = wxBitmap(pause_xpm); break;
		case Icon_Stop: bitmap = wxBitmap(stop_xpm); break;
		case Icon_Undo: bitmap = wxBitmap(undo_xpm); break;
		case Icon_Redo: bitmap = wxBitmap(redo_xpm); break;
		case Icon_ArrowUp: bitmap = wxBitmap(arrow_up_xpm); break;
		case Icon_ArrowDown: bitmap = wxBitmap(arrow_down_xpm); break;
		case Icon_GameMouse: bitmap = wxBitmap(game_mouse_xpm); break;
		default: 
			SHOOT_ASSERT(false, "Invalid Icon type");
		}

		return bitmap;
	}
}


