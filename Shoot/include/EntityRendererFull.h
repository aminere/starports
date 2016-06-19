/* 

Amine Rehioui
Created: February 1st 2014

*/

#pragma once


#include "EntityRenderer3D.h"
#include "RenderTarget.h"

namespace shoot
{
	//! EntityRendererFull
	class EntityRendererFull : public EntityRenderer3D
	{
		typedef EntityRenderer3D super;

	public:

		//! constructor
		EntityRendererFull();

		//! Initializes the renderer
		void Init();

		//! called when the screen has been resized
		void OnResize();

		//! registers the entity for rendering
		bool RegisterEntity(RenderableEntity* pEntity);

		//! registers the entity for rendering
		void RegisterDebugEntity(RenderableEntity* pEntity);

		//! clears the rendering lists
		void Clear();

		//! renders the entities
		void Render();

		//! change the current 3D camera
		void Set3DCamera(Camera* pCamera);

		//! GetFinalRenderTarget
		RenderTarget* GetFinalRenderTarget() const { return m_FinalRT; }

	protected:

		Reference<Camera> m_Default2DCamera;

		RenderMap m_Solid2DRenderMap;
		RenderMap m_Transparent2DRenderMap;
		RenderMap m_Solid2DPostFadeRenderMap;
		RenderMap m_Debug2DMap;
		RenderMap m_2DLayoutRenderMap;
		RenderMap m_PostEffectMap;
		RenderMap m_2DLayoutPostTransRenderMap;

		// post processing
		Reference<RenderTarget> m_FinalRT;
		Reference<RenderTarget> m_EffectRT2;
		Reference<Shader> m_BlurShader;
		Reference<Shader> m_BloomStep1;
	};
}




