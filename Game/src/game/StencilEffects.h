/* 

Amine Rehioui
Created: September 6th 2014

*/

#pragma once


#include "Effect.h"

namespace shoot
{
	class Tile;

	//! StencilEffects
	class StencilEffects : public Effect
	{
	public:

		//! Macro to provide type information
		DECLARE_OBJECT(StencilEffects, Effect);

		//! Constructor
		StencilEffects();

		//! Reads/Writes struct properties from/to a stream
		void Serialize(PropertyStream& stream);

		//! Initiliazes the effect
		void Init();

		//! Renders the effect
		void Render();

		//! Update
		void Update();

		//! AddConstructionEffect
		void AddConstructionEffect(MeshEntity* pStencilMesh);

		//! ShowPlayfieldEffect
		void ShowPlayfieldEffect();

		//! OnActorSpawned
		void OnActorSpawned(Tile*);

		//! OnTilesChanged
		inline void OnTilesChanged() { m_TileVBDirty = true; }

	private:

		Reference<Material> m_DefaultMaterial;
		Reference<Material> m_TilesMaterial;
		std::vector< Handle<MeshEntity> > m_aStencilMeshes;

		Reference<VertexBuffer> m_PlayfieldVB;
		Reference<VertexBuffer> m_TilesVB;
		bool m_bPlayfieldEffectVisible;
		float m_fPlayfieldEffectTimer;
		float m_fPlayfieldEffectDuration;
		std::list<Tile*> m_lActorTiles;
		bool m_TileVBDirty;

		void RenderPlayfieldEffect();
		void RenderConstructionEffect();
		void UpdateTileVB(const std::list<Tile*>&, Vertex3D*&, int&);

		// properties
		Color m_PlayfieldHighlight;
		float m_fPlayfieldHighlightAlpha;
		Color m_BlockedTileColor;
		float m_fHexagonScaleFactor;
		float m_fPlayfieldEffectMaxDuration;
		float m_fPlayfieldEffectFadeDuration;
		Reference<Entity> m_BillBoardEntity;
	};
}



