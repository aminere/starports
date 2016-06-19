/* 

Amine Rehioui
Created: October 15th 2011

*/

#pragma once


#include "ShootEditor.h"

namespace shoot
{
	//! custom Game editor
	class GameEditor : public ShootEditor
	{
		typedef ShootEditor super;

	public:

		//! constructor
		GameEditor();

		//! destructor
		virtual ~GameEditor();

		//! returns a menu category for a class type
		std::string GetMenuCategory(const char* classType);

		//! Selects an entity
		Entity* SelectEntity(Entity* pEntity, const Vector3& vRayStart, const Vector3& vRayDir);

		//! Creates a view port
		ViewPort* CreateViewPort(wxWindow* pParent);

		//! returns the hex mode
		inline bool GetHexMode() const { return m_bHexMode; }

		//! sets the hex mode
		inline void SetHexMode(bool bSet) { m_bHexMode = bSet; }

		//! Return the unique instance of class T
		inline static GameEditor* Instance() { return static_cast<GameEditor*>(m_spInstance); }

	private:

		bool m_bHexMode;
	};
}



