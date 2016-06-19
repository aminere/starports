/* 

Amine Rehioui
Created: May 8th 2010

*/

#include "Shoot.h"

#include "EntityRenderer3D.h"

#include "VertexBuffer.h"

#include "Light.h"
#include "MaterialProvider.h"
#include "GeometryProvider.h"
#include "Effect.h"

#include "GraphicExtensionHandler.h"

namespace shoot
{
	//! constructor
	EntityRenderer3D::EntityRenderer3D(bool isTextureRenderer)
		: m_EnableTextureRT(false)
		, m_IsTextureRenderer(isTextureRenderer)
	{
	}

	//! Initializes the renderer
	void EntityRenderer3D::Init()
	{
		super::Init();
	}

	//! called when the screen has been resized
	void EntityRenderer3D::OnResize()
	{
		super::OnResize();

		auto screenSize = GraphicsDriver::Instance()->GetViewPort().Size();
		#if SHOOT_PLATFORM == SHOOT_PLATFORM_ANDROID
			auto sceneRTSize = screenSize / 2.0f;			
			auto maxDimension = (int)Math::Max(sceneRTSize.X, sceneRTSize.Y);
			sceneRTSize = Vector2::One * Math::GetNextPow2(maxDimension);
		#else
			auto sceneRTSize = screenSize;
		#endif
		if (m_IsTextureRenderer)
		{
			sceneRTSize /= 2.0f;
			sceneRTSize.X = (float)Math::GetNextPow2((int)sceneRTSize.X);
			sceneRTSize.Y = sceneRTSize.X;
		}

		Log.Print("sceneRTSize: %f, %f\n", sceneRTSize.X, sceneRTSize.Y);
		auto effectRTSize = sceneRTSize / 2.0f;

		if (!m_SceneRT.IsValid())
		{
			auto flags = GraphicsDriver::BF_ColorBuffer | GraphicsDriver::BF_DepthBuffer | GraphicsDriver::BF_StencilBuffer;
			if (m_IsTextureRenderer)
				flags |= GraphicsDriver::BF_EnableWrap;
			m_SceneRT = GraphicsDriver::Instance()->CreateRenderTarget(sceneRTSize, flags);
			m_EffectRT = GraphicsDriver::Instance()->CreateRenderTarget(effectRTSize, GraphicsDriver::BF_ColorBuffer);
		}
		else
		{
			m_SceneRT->Resize(sceneRTSize);
			m_EffectRT->Resize(effectRTSize);
		}
	}

	//! registers the entity for rendering
	bool EntityRenderer3D::RegisterEntity(RenderableEntity* pEntity)
	{
		if(Light* pLight = DYNAMIC_CAST(pEntity, Light))
		{
			if(m_LightingInfo.m_aLightVPs.size() < m_sMaxLights)
			{
				m_LightingInfo.m_aLightColors.push_back(pLight->GetColor());
				m_LightingInfo.m_aLightPositions.push_back(pLight->GetAbsolutePosition());
				m_LightingInfo.m_aLightVPs.push_back(pLight->GetViewProjectionMatrix());
				m_LightingInfo.m_aLightAttenuationFactors.push_back(pLight->GetAttenuationFactor());
			}
			else
				SHOOT_WARNING(false, "m_sMaxLights reached");
			return true;
		}

		std::vector<GraphicComponent*> graphicComponents;
		pEntity->GetComponents<GraphicComponent>(graphicComponents);
		bool handled = false;
		for(auto gc : graphicComponents)
		{
			switch(gc->GetRenderingPass())
			{
			case GraphicComponent::RP_3D:
				AddToRenderMap(m_Solid3DRenderMap, gc); 
				handled = true;
				break;

			case GraphicComponent::RP_3DParticles:
				AddToRenderMap(m_3DParticlesRenderMap, gc); 
				handled = true;
				break;

			case GraphicComponent::RP_3DTransparent:
				AddToRenderMap(m_Transparent3DRenderMap, gc); 
				handled = true;
				break;

			case GraphicComponent::RP_SkyBox:
				AddToRenderMap(m_SkyBoxMap, gc); 
				handled = true;
				break;

			case GraphicComponent::RP_3DDepthOff:
				AddToRenderMap(m_3DDepthOffMap, gc); 
				handled = true;
				break;
			}

			if(gc->IsShadowCaster())
			{
				AddToRenderMap(m_ShadowCastersMap, gc); 
				handled = true;
			}
		}

		return handled;
	}

	//! registers an effect for rendering
	void EntityRenderer3D::RegisterEffect(Effect* pEffect)
	{
		m_aEffects.push_back(pEffect);
	}	

	//! clears the rendering lists
	void EntityRenderer3D::Clear()
	{
		m_Solid3DRenderMap.clear();
		m_Transparent3DRenderMap.clear();
		m_3DParticlesRenderMap.clear();
		m_SkyBoxMap.clear();
		m_3DDepthOffMap.clear();
		m_ShadowCastersMap.clear();
		m_LightingInfo.Clear();
		m_aEffects.clear();
		m_Stats = Stats();

		if(m_TextureRenderer.IsValid())
			m_TextureRenderer->Clear();
	}
	
	//! renders the entities
	void EntityRenderer3D::Render()
	{
		if (GraphicExtensionHandler::Instance()->HasExtension(GraphicExtensionHandler::E_DepthTexture))
		{
			if (!m_ShadowMapShader.IsValid())
			{
				m_ShadowMapShader = static_cast<Shader*>(ObjectManager::Instance()->FindOrLoad("common/shaders/shadowmap_pass.xml"));

				auto screenSize = GraphicsDriver::Instance()->GetViewPort().Size();
				auto maxDimension = Math::Max(screenSize.X, screenSize.Y);
				auto shadowRTDimension = (int)maxDimension;
				while (Math::GetNextPow2(shadowRTDimension) > 1024)
				{
					shadowRTDimension /= 2;
				}
				auto shadowRTSize = Vector2::One*(float)Math::GetNextPow2(shadowRTDimension);
				if (m_IsTextureRenderer)
					shadowRTSize /= 2.0f;

				for (int i = 0; i < m_sMaxLights; ++i)
					m_ShadowMap.push_back(GraphicsDriver::Instance()->CreateRenderTarget(shadowRTSize, GraphicsDriver::BF_DepthBuffer));
			}
		}
		else
		{
			if (!m_FallBackNormalMappingShader.IsValid())
				m_FallBackNormalMappingShader = static_cast<Shader*>(ObjectManager::Instance()->FindOrLoad("common/shaders/normalmap.xml"));
		}

		if(!m_SceneRT.IsValid())
			OnResize();

		if (!m_3DCamera.IsValid())
			return;

		GraphicsDriver::Instance()->SetRenderState(GraphicsDriver::RS_LineWidth, 2.0f);
		GraphicsDriver::Instance()->SetRenderState(GraphicsDriver::RS_DepthWriting, true); // TODO nothing is rendered if this is removed.. understand why!

		if (GraphicExtensionHandler::Instance()->HasExtension(GraphicExtensionHandler::E_DepthTexture))
		{
			// prepare shadow maps
			bool bShadowMapping = m_LightingInfo.m_aLightVPs.size() && m_ShadowCastersMap.size();
			if (bShadowMapping)
			{
				//GraphicsDriver::Instance()->SetCullMode(GraphicsDriver::CM_ClockWise);
				GraphicsDriver::Instance()->SetRenderState(GraphicsDriver::RS_DepthTesting, true);
				GraphicsDriver::Instance()->SetRenderState(GraphicsDriver::RS_DepthWriting, true);
				for (size_t i = 0; i < m_LightingInfo.m_aLightVPs.size(); ++i)
				{
					GraphicsDriver::Instance()->SetRenderTarget(m_ShadowMap[i]);
					GraphicsDriver::Instance()->SetTransform(GraphicsDriver::TS_ViewProjection, m_LightingInfo.m_aLightVPs[i]);
					m_ShadowMapShader->Begin();
					for (RenderMap::iterator it = m_ShadowCastersMap.begin(); it != m_ShadowCastersMap.end(); ++it)
					{
						for (VertexMap::iterator it2 = (*it).second.m_VertexMap.begin(); it2 != (*it).second.m_VertexMap.end(); ++it2)
						{
							VertexInfo& vertexInfo = (*it2).second;
							vertexInfo.pVB->Begin(m_ShadowMapShader);
							vertexInfo.pVB->Draw(vertexInfo.aWorldTransforms, std::vector<Matrix44>());
						}
					}
				}
				//GraphicsDriver::Instance()->SetCullMode(GraphicsDriver::CM_CounterClockWise);
			}
		}		

		// set up 3D view
		GraphicsDriver::Instance()->SetRenderTarget(m_SceneRT);
		GraphicsDriver::Instance()->SetRenderState(GraphicsDriver::RS_DepthTesting, true);
		GraphicsDriver::Instance()->SetRenderState(GraphicsDriver::RS_DepthWriting, true);

		GraphicsDriver::Instance()->SetTransform(GraphicsDriver::TS_Projection, m_3DCamera->GetProjectionMatrix());
		GraphicsDriver::Instance()->SetTransform(GraphicsDriver::TS_View, m_3DCamera->GetViewMatrix());
		RenderFromMap(m_Solid3DRenderMap);

		if (m_SkyBoxMap.size())
		{
			GraphicsDriver::Instance()->SetRenderState(GraphicsDriver::RS_DepthWriting, false);
			RenderSkyBox(m_3DCamera);
			GraphicsDriver::Instance()->SetRenderState(GraphicsDriver::RS_DepthWriting, true);
		}

		GraphicsDriver::Instance()->SetTransform(GraphicsDriver::TS_View, m_3DCamera->GetViewMatrix());
		GraphicsDriver::Instance()->SetRenderState(GraphicsDriver::RS_DepthWriting, false);

		RenderFromMap(m_Transparent3DRenderMap);

		// render effects
		for (auto effect : m_aEffects)
			effect->Render();

		RenderFromMap(m_3DParticlesRenderMap);

		GraphicsDriver::Instance()->SetRenderState(GraphicsDriver::RS_DepthTesting, false);
		RenderFromMap(m_3DDepthOffMap);

		GraphicsDriver::Instance()->DiscardFrameBuffers(true);
	}	

	//! adds an entity to a render map
	void EntityRenderer3D::AddToRenderMap(RenderMap& renderMap, GraphicComponent* pGraphic)
	{
		for (size_t i = 0; i<pGraphic->GetMaterialCount() && i<pGraphic->GetVertexBufferCount(); ++i)
		{
			Material* pMaterial = pGraphic->GetMaterial(i);
			VertexBuffer* pVB = pGraphic->GetVertexBuffer(i);
			if(!pMaterial)
				continue;

			if(pVB->GetNumVertices() || pVB->GetNumIndices())
			{
				size_t materialID = (size_t)pMaterial;
				#ifdef SHOOT_64
					size_t ID = (((size_t)pGraphic->GetRenderingPriority() & 0xFF) << 56) | (materialID & 0x00FFFFFFFFFFFFFF);
				#else
					size_t ID = ((pGraphic->GetRenderingPriority() & 0xFF) << 24) | (materialID & 0x00FFFFFF);
				#endif

				size_t vbID = (size_t)pVB;
				RenderInfo& renderInfo = renderMap[ID];
				renderInfo.pMaterial = pMaterial;
				VertexInfo& vertexInfo = renderInfo.m_VertexMap[vbID];
				vertexInfo.pVB = pVB;
				vertexInfo.aWorldTransforms.push_back(pGraphic->GetWorldTransform());
				vertexInfo.aTextureTransforms.push_back(pGraphic->GetTextureTransform(i));
			}
		}
	}

	//! renders from a render map
	void EntityRenderer3D::RenderFromMap(RenderMap& renderMap)
	{
		for(RenderMap::iterator it = renderMap.begin(); it != renderMap.end(); ++it)
		{
			Material* pMaterial = (*it).second.pMaterial;
			pMaterial->Begin();

			if(pMaterial->UsesTextureRenderTarget())
				m_TextureRenderer->m_SceneRT->BindTexture(0, GraphicsDriver::BF_ColorBuffer);

			Shader* pShader = pMaterial->GetShader();
			if(pShader->GetType() == Shader::T_Lighting
			|| pShader->GetType() == Shader::T_ShadowMapping
			|| pShader->GetType() == Shader::T_LightAndFog)
			{
				if(m_LightingInfo.m_aLightPositions.size())
				{
					pShader->GetConstant("uLightPositions")->Set(&(m_LightingInfo.m_aLightPositions[0]), m_LightingInfo.m_aLightPositions.size());
					pShader->GetConstant("uLightColors")->Set(&(m_LightingInfo.m_aLightColors[0]), m_LightingInfo.m_aLightColors.size());
					pShader->GetConstant("uLightAttenuation")->Set(&(m_LightingInfo.m_aLightAttenuationFactors[0]), m_LightingInfo.m_aLightAttenuationFactors.size());
					Vector3 vCameraPos = m_3DCamera->GetPosition();
					pShader->GetConstant("uCameraPos")->Set(&vCameraPos, 1);
					//int lightCount = (int)m_LightingInfo.m_aLightPositions.size();
					//pShader->GetConstant("uLightCount")->Set(&lightCount, 1);
				}

				if (auto factor = pShader->GetConstant("uDiffuseFactor"))
				{
					float diffuseFactor = 2.0f;
					pShader->GetConstant("uDiffuseFactor")->Set(&diffuseFactor, 1);
				}
			}

			switch(pShader->GetType())
			{
			case Shader::T_ShadowMapping:
				if (GraphicExtensionHandler::Instance()->HasExtension(GraphicExtensionHandler::E_DepthTexture))
				{
					if (m_LightingInfo.m_aLightVPs.size())
					{
						pShader->GetConstant("uLightVPs")->Set(&(m_LightingInfo.m_aLightVPs[0]), m_LightingInfo.m_aLightVPs.size());

						int shadowMapSlots[m_sMaxLights];
						auto textureCount = pMaterial->GetTextures().GetSize();
						for (size_t i = 0; i < m_LightingInfo.m_aLightVPs.size(); ++i)
						{
							shadowMapSlots[i] = (int)(textureCount + i);
							m_ShadowMap[i]->BindTexture(shadowMapSlots[i], GraphicsDriver::BF_DepthBuffer);
						}

						pShader->GetConstant("uShadowMaps")->Set(shadowMapSlots, m_LightingInfo.m_aLightVPs.size());
					}
				}
				else
				{
					pShader = m_FallBackNormalMappingShader.Get();
					float diffuseFactor = 1.0f;
					pShader->GetConstant("uDiffuseFactor")->Set(&diffuseFactor, 1);
				}
				break;

			case Shader::T_Fog:
				{
					Vector3 vCameraPos = m_3DCamera->GetPosition();
					pShader->GetConstant("uCameraPos")->Set(&vCameraPos, 1);
				}
				break;

			case Shader::T_Fire:
				{
					auto screenSize = GraphicsDriver::Instance()->GetViewPort().Size();
					pShader->GetConstant("uScreenSize")->Set(&screenSize, 1);

					if(auto texture = pMaterial->GetTexture(1))
					{
						auto texSize = texture->GetSize();
						pShader->GetConstant("uTexSize")->Set(&texSize, 1);
					}
				}
				break;

			case Shader::T_PostEffect:
				{
					auto screenSize = GraphicsDriver::Instance()->GetViewPort().Size();
					auto pixelSize = Vector2::Create(1.0f / screenSize.X, 1.0f / screenSize.Y);
					pShader->GetConstant("uPixelSize")->Set(&pixelSize, 1);

					// bind scene & effect render targets
					int sceneRT = (int)pMaterial->GetTextures().GetSize();
					int effectRT = sceneRT + 1;
					pShader->GetConstant("uSceneRT")->Set(&sceneRT, 1);
					pShader->GetConstant("uEffectRT")->Set(&effectRT, 1);
					m_SceneRT->BindTexture(sceneRT, GraphicsDriver::BF_ColorBuffer);
					m_EffectRT->BindTexture(effectRT, GraphicsDriver::BF_ColorBuffer);
				}
				break;
			}
			
			if (auto ambient = pShader->GetConstant("uAmbientColor"))
				ambient->Set(&pMaterial->GetColor(), 1);

			pShader->Begin();
			for(VertexMap::iterator it2 = (*it).second.m_VertexMap.begin(); it2 != (*it).second.m_VertexMap.end(); ++it2)
			{
				VertexInfo& vertexInfo = (*it2).second;

				if(pMaterial->UsesTextureRenderTarget()) // flip Y when using a texture render target
				{
					for(auto& textureMatrix : vertexInfo.aTextureTransforms)
						textureMatrix.Scale(Vector3::Create(1.0f, -1.0f, 1.0f));
				}

				vertexInfo.pVB->Begin(pShader);
				vertexInfo.pVB->Draw(vertexInfo.aWorldTransforms, vertexInfo.aTextureTransforms);
			}
			m_Stats.Increment(pMaterial);
		}
	}

	//! renders the skybox
	void EntityRenderer3D::RenderSkyBox(Camera* pCamera)
	{
		Matrix44 viewMatrix;
		viewMatrix = Matrix44::MakeLookAtLH(Vector3::Zero, pCamera->GetLookAtDirection(), Vector3::Up);
		GraphicsDriver::Instance()->SetTransform(GraphicsDriver::TS_View, viewMatrix);

		Material* pMaterial = m_SkyBoxMap.begin()->second.pMaterial;
		pMaterial->Begin();
		pMaterial->GetShader()->GetConstant("uAmbientColor")->Set(&pMaterial->GetColor(), 1);
		pMaterial->GetShader()->Begin();
		VertexInfo& vertexInfo = m_SkyBoxMap.begin()->second.m_VertexMap.begin()->second;
		vertexInfo.pVB->Begin(pMaterial->GetShader());
		vertexInfo.pVB->Draw(vertexInfo.aWorldTransforms, vertexInfo.aTextureTransforms);
		m_Stats.Increment(pMaterial);
	}

	//! EnableRenderToTexture
	void EntityRenderer3D::EnableRenderToTexture(bool enable)
	{
		m_EnableTextureRT = enable;
	}

	EntityRenderer* EntityRenderer3D::GetTextureRenderer()
	{
		if(!m_TextureRenderer.IsValid())
			m_TextureRenderer = snew EntityRenderer3D(true);
		return m_TextureRenderer;
	}
}

