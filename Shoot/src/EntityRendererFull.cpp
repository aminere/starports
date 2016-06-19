/* 

Amine Rehioui
Created: May 8th 2010

*/

#include "Shoot.h"

#include "EntityRendererFull.h"

#include "GeometryProvider.h"

namespace shoot
{
	//! constructor
	EntityRendererFull::EntityRendererFull()
		: super(false)
	{
		m_Default2DCamera = snew Camera();
		m_Default2DCamera->SetType(Projection::Ortho2D);
	}

	//! Initializes the renderer
	void EntityRendererFull::Init()
	{
		super::Init();
	}

	//! called when the screen has been resized
	void EntityRendererFull::OnResize()
	{
		super::OnResize();

		auto screenSize = GraphicsDriver::Instance()->GetViewPort().Size();
		auto effectRTSize = m_EffectRT->GetSize();
		if (!m_FinalRT.IsValid())
		{
			m_BlurShader = static_cast<Shader*>(ObjectManager::Instance()->FindOrLoad("common/shaders/blur.xml"));
			m_BloomStep1 = static_cast<Shader*>(ObjectManager::Instance()->FindOrLoad("common/shaders/bloom_step1.xml"));
			m_FinalRT = GraphicsDriver::Instance()->CreateRenderTarget(screenSize, GraphicsDriver::BF_ColorBuffer);
			m_EffectRT2 = GraphicsDriver::Instance()->CreateRenderTarget(effectRTSize, GraphicsDriver::BF_ColorBuffer);
		}
		else
		{
			m_FinalRT->Resize(screenSize);
		}
	}

	//! registers the entity for rendering
	bool EntityRendererFull::RegisterEntity(RenderableEntity* pEntity)
	{
		if(m_EnableTextureRT)
			return m_TextureRenderer->RegisterEntity(pEntity);
		else if(super::RegisterEntity(pEntity))
			return true;

		std::vector<GraphicComponent*> graphicComponents;
		pEntity->GetComponents<GraphicComponent>(graphicComponents);
		for(auto gc : graphicComponents)
		{
			switch(gc->GetRenderingPass())
			{
			case GraphicComponent::RP_2D:
				AddToRenderMap(m_Solid2DRenderMap, gc);
				break;

			case GraphicComponent::RP_2DTransparent:
				AddToRenderMap(m_Transparent2DRenderMap, gc);
				break;

			case GraphicComponent::RP_2DPostFade:
				AddToRenderMap(m_Solid2DPostFadeRenderMap, gc);
				break;

			case GraphicComponent::RP_2DLayout:
				AddToRenderMap(m_2DLayoutRenderMap, gc);
				break;

			case GraphicComponent::RP_PostEffect:
				AddToRenderMap(m_PostEffectMap, gc);
				break;

			case GraphicComponent::RP_2DLayoutPostTransparent:
				AddToRenderMap(m_2DLayoutPostTransRenderMap, gc);
				break;
			}
		}

		return true;
	}

	//! registers the entity for rendering
	void EntityRendererFull::RegisterDebugEntity(RenderableEntity* pEntity)
	{
		if(GraphicComponent* pGraphic = pEntity->GetComponent<GraphicComponent>())
			AddToRenderMap(m_Debug2DMap, pGraphic);
	}

	//! clears the rendering lists
	void EntityRendererFull::Clear()
	{
		super::Clear();

		m_Solid2DRenderMap.clear();
		m_Transparent2DRenderMap.clear();
		m_Solid2DPostFadeRenderMap.clear();
		m_Debug2DMap.clear();
		m_2DLayoutRenderMap.clear();
		m_PostEffectMap.clear();
		m_2DLayoutPostTransRenderMap.clear();
	}
	
	//! renders the entities
	void EntityRendererFull::Render()
	{
		// texture render targets first
		if(m_TextureRenderer.IsValid())
			m_TextureRenderer->Render();

		super::Render(); // 3D rendering

		if (!m_FinalRT.IsValid())
			OnResize();

		GraphicsDriver::Instance()->SetRenderState(GraphicsDriver::RS_AlphaBlending, false);
		auto fullScreenQuad = GeometryProvider::Instance()->GetFullScreenQuad();
		auto fullScreenShader = Engine::Instance()->GetFullScreenShader();

		auto usePostFX = true;
		#if SHOOT_PLATFORM == SHOOT_PLATFORM_ANDROID
			usePostFX = false;
		#endif

		if (usePostFX)
		{
			// mostly post-FX
			// Downsample & extract high intensities
			GraphicsDriver::Instance()->SetRenderTarget(m_EffectRT);
			m_SceneRT->BindTexture(0, GraphicsDriver::BF_ColorBuffer);
			fullScreenQuad->Draw(m_BloomStep1);

			// Horizontal blur
			GraphicsDriver::Instance()->SetRenderTarget(m_EffectRT2);
			m_EffectRT->BindTexture(0, GraphicsDriver::BF_ColorBuffer);
			Vector2 vPixelSize = Vector2::Create(1.0f / m_EffectRT->GetSize().X, 1.0f / m_EffectRT->GetSize().Y);
			m_BlurShader->GetConstant("uPixelSize")->Set(&vPixelSize, 1);
			int horizontalBlur = 1;
			m_BlurShader->GetConstant("uIsHorizontal")->Set(&horizontalBlur, 1);
			fullScreenQuad->Draw(m_BlurShader);

			// Vertical blur
			GraphicsDriver::Instance()->SetRenderTarget(m_EffectRT);
			GraphicsDriver::Instance()->ClearBuffers(GraphicsDriver::BF_ColorBuffer, Color::Black);
			m_EffectRT2->BindTexture(0, GraphicsDriver::BF_ColorBuffer);
			vPixelSize = Vector2::Create(1.0f / m_EffectRT2->GetSize().X, 1.0f / m_EffectRT2->GetSize().Y);
			m_BlurShader->GetConstant("uPixelSize")->Set(&vPixelSize, 1);
			horizontalBlur = 0;
			m_BlurShader->GetConstant("uIsHorizontal")->Set(&horizontalBlur, 1);
			fullScreenQuad->Draw(m_BlurShader);
		}

		// draw scene
		GraphicsDriver::Instance()->SetRenderTarget(m_FinalRT);
		m_SceneRT->BindTexture(0, GraphicsDriver::BF_ColorBuffer);
		fullScreenShader->GetConstant("uAmbientColor")->Set(&Color::White, 1);
		fullScreenQuad->Draw(fullScreenShader);

		// combine with effects
		if (usePostFX)
		{
			m_EffectRT->BindTexture(0, GraphicsDriver::BF_ColorBuffer);
			static const Color combineColor = Color::Create(1.0f, 1.0f, 1.0f, 1.0f);
			fullScreenShader->GetConstant("uAmbientColor")->Set(&combineColor, 1);
			GraphicsDriver::Instance()->SetRenderState(GraphicsDriver::RS_AlphaBlending, true);
			GraphicsDriver::Instance()->SetBlendFunc(GraphicsDriver::BF_SrcAlpha, GraphicsDriver::BF_One);
			fullScreenQuad->Draw(fullScreenShader);
		}

		// custom post effects
		RenderFromMap(m_PostEffectMap);

		auto _2DCamera = m_2DCamera.Get();
		if (!_2DCamera)
			_2DCamera = m_Default2DCamera.Get();

		// set up 2D view
		GraphicsDriver::Instance()->SetTransform(GraphicsDriver::TS_Projection, _2DCamera->GetProjectionMatrix());
		GraphicsDriver::Instance()->SetTransform(GraphicsDriver::TS_View, _2DCamera->GetViewMatrix());

		// render 2D entities
		RenderFromMap(m_Solid2DRenderMap);

		// render 2D layouted entities
		GraphicsDriver::Instance()->SetTransform(GraphicsDriver::TS_View, Matrix44::Identity);
		RenderFromMap(m_2DLayoutRenderMap);
		GraphicsDriver::Instance()->SetTransform(GraphicsDriver::TS_View, _2DCamera->GetViewMatrix());

		// render transparent 2D entities
		RenderFromMap(m_Transparent2DRenderMap);

		// render 2D layouted entities post transparent
		if (m_2DLayoutPostTransRenderMap.size())
		{
			GraphicsDriver::Instance()->SetTransform(GraphicsDriver::TS_View, Matrix44::Identity);
			RenderFromMap(m_2DLayoutPostTransRenderMap);
			GraphicsDriver::Instance()->SetTransform(GraphicsDriver::TS_View, _2DCamera->GetViewMatrix());
		}

		// render the fade effect
		FadeRenderer::Instance()->Render();

		// render 2D entities that are not affected by the fade effect
		RenderFromMap(m_Solid2DPostFadeRenderMap);

#ifndef _RETAIL_
		m_Stats.Enabled = false;
		// render Debug entities
		GraphicsDriver::Instance()->SetTransform(GraphicsDriver::TS_View, Matrix44::Identity);
		RenderFromMap(m_Debug2DMap);
#endif
	}

	//! change the current 3D camera
	void EntityRendererFull::Set3DCamera(Camera* pCamera)
	{
		if(m_EnableTextureRT)
			m_TextureRenderer->Set3DCamera(pCamera);
		else
			m_3DCamera = pCamera;

		if (pCamera)
		{
			pCamera->SetProjectionMatrixDirty();
			pCamera->SetViewMatrixDirty();
		}
	}
}


