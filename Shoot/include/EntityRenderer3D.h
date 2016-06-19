/* 

Amine Rehioui
Created: February 1st 2014

*/

#pragma once

#include "EntityRenderer.h"
#include "RenderTarget.h"

namespace shoot
{	
	// forwards
	class VertexBuffer;
	class Material;
	class Light;

	//! EntityRenderer3D
	class EntityRenderer3D : public EntityRenderer
	{
		typedef EntityRenderer super;

	public:		

		//! constructor
		EntityRenderer3D(bool isTextureRenderer);

		//! Initializes the renderer
		virtual void Init();

		//! called when the screen has been resized
		virtual void OnResize();

		//! registers the entity for rendering
		virtual bool RegisterEntity(RenderableEntity* pEntity);

		//! registers an effect for rendering
		void RegisterEffect(Effect* pEffect);

		//! registers the entity for rendering
		virtual void RegisterDebugEntity(RenderableEntity* pEntity) { }

		//! clears the rendering lists
		virtual void Clear();

		//! renders the entities
		virtual void Render();

		//! Gets a texture renderer
		EntityRenderer* GetTextureRenderer();

		//! GetFinalRenderTarget
		virtual RenderTarget* GetFinalRenderTarget() const { return NULL; }

	protected:	

		//! vertex info
		struct VertexInfo
		{
			VertexBuffer* pVB;
			std::vector<Matrix44> aWorldTransforms;
			std::vector<Matrix44> aTextureTransforms;
		};

		typedef std::map< size_t, VertexInfo > VertexMap;

		//! render info
		struct RenderInfo
		{
			Material* pMaterial;
			VertexMap m_VertexMap;
		};
		
		typedef std::map< size_t, RenderInfo > RenderMap;

		//! lighting info
		struct LightingInfo
		{
			std::vector<Vector3> m_aLightPositions;
			std::vector<Color> m_aLightColors;
			std::vector<Matrix44> m_aLightVPs;
			std::vector<float> m_aLightAttenuationFactors;

			//! clear
			inline void Clear()
			{
				m_aLightPositions.clear();
				m_aLightColors.clear();
				m_aLightVPs.clear();
				m_aLightAttenuationFactors.clear();
			}
		};

		RenderMap m_Solid3DRenderMap;
		RenderMap m_Transparent3DRenderMap;
		RenderMap m_3DParticlesRenderMap;
		RenderMap m_SkyBoxMap;
		RenderMap m_3DDepthOffMap;
		std::vector<Effect*> m_aEffects;

		Reference<RenderTarget> m_SceneRT;
		Reference<RenderTarget> m_EffectRT;

		// shadow mapping
		RenderMap m_ShadowCastersMap;
		LightingInfo m_LightingInfo;
		std::vector< Reference<RenderTarget> > m_ShadowMap;
		Reference<Shader> m_ShadowMapShader;
		Reference<Shader> m_FallBackNormalMappingShader;
		static const int m_sMaxLights = 1;

		//! adds a graphic to a render map
		virtual void AddToRenderMap(RenderMap& renderMap, GraphicComponent* pGraphic);

		//! renders from a render map
		void RenderFromMap(RenderMap& renderMap);

		//! renders the skybox
		void RenderSkyBox(Camera* pCamera);

		//! RenderToTexture
		Handle<EntityRenderer3D> m_TextureRenderer;
		bool m_EnableTextureRT;
		bool m_IsTextureRenderer;

		void EnableRenderToTexture(bool enable);
	};
}

