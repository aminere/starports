/* 

Amine Rehioui
Created: October 15th 2011

*/

#pragma once


#include "EditorRenderer.h"

namespace shoot
{
	class Tower;
	class BulletManager;
	class Playfield;
	class Defender;
	class WheelMenu;

	//! Shoot Test Editor renderer
	class GameEditorRenderer : public EditorRenderer
	{
		typedef EditorRenderer super;

	public:

		//! constructor
		GameEditorRenderer();

		//! destructor
		virtual ~GameEditorRenderer();

		//! Init
		void Init();

		//! sets the playfield dirty flag
		inline void SetPlayfieldDirty(bool bSet) { m_bPlayfieldDirty = bSet; }

	protected:

		Reference<VertexBuffer> m_UnitCircle;
		Reference<VertexBuffer> m_PlayfieldVB;
		Size m_PreviousDimension;
		bool m_bPlayfieldDirty;
		Reference<Material> m_ColorDebugMaterial;

		//! registers an entity for post rendering
		void RegisterEntity(Entity* pEntity);

		//! custom Post 3D rendering
		void RenderPost3D();

		//! custom Post 2D rendering
		void RenderPost2D();

		std::vector<BulletManager*> m_aBulletManagers;
		std::vector<Defender*> m_aDefenders;
		std::vector<WheelMenu*> m_aWheelMenus;
		std::vector<Tower*> m_aTowers;
	};
}



