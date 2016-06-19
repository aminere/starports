/* 

Amine Rehioui
Created: June 15th 2013

*/

#include "Shoot.h"

#include "FadeRenderer.h"
#include "GeometryProvider.h"
#include "MaterialProvider.h"

namespace shoot
{
	DEFINE_OBJECT(FadeRenderer);

	//! static vars
	FadeRenderer* FadeRenderer::ms_pInstance = NULL;

	//! constructor
	FadeRenderer::FadeRenderer()
		: m_Color(Color::Zero)
		, m_FadeDestColor(m_Color)
		, m_FadeSrcColor(m_Color)
		, m_Additive(false)
		, m_bFadeInProgress(false)
	{
		SHOOT_ASSERT(!ms_pInstance, "Multiple FadeRenderer instances detected");
		ms_pInstance = this;

		m_WhiteBG = ResourceManager::Instance()->GetResource<Texture>("common/tex/WhiteBG.xml");		
	}

	//! destructor
	FadeRenderer::~FadeRenderer()
	{
		ms_pInstance = NULL;
	}

	//! update the fade effect
	void FadeRenderer::Update()
	{
		if(!m_bFadeInProgress)
			return;

		m_Color = Math::Lerp(m_FadeSrcColor, m_FadeDestColor, m_fFadeTimer/m_fFadeDuration);
		m_fFadeTimer += g_fDeltaTime;
	}

	//! renders the fade effect
	void FadeRenderer::Render()
	{
		if(!m_bFadeInProgress)
			return;

		if(m_fFadeTimer > m_fFadeDuration)
		{
			m_Color = m_FadeDestColor;
			m_bFadeInProgress = m_StayFadedUntilFurtherNotice;
		}

		GraphicsDriver::Instance()->SetRenderState(GraphicsDriver::RS_AlphaBlending, true);
		GraphicsDriver::Instance()->SetBlendFunc(GraphicsDriver::BF_SrcAlpha, m_Additive ? GraphicsDriver::BF_One : GraphicsDriver::BF_InvSrcAlpha);
		auto fullScreenQuad = GeometryProvider::Instance()->GetFullScreenQuad();
		auto fullScreenShader = Engine::Instance()->GetFullScreenShader();
		int texIndex = 0;
		m_WhiteBG->Begin(texIndex);
		fullScreenShader->GetConstant("uTexture0")->Set(&texIndex, 1);
		fullScreenShader->GetConstant("uAmbientColor")->Set(&m_Color, 1);
		fullScreenQuad->Draw(fullScreenShader);
	}

	//! sets the fade color
	void FadeRenderer::SetColor(const Color& color)
	{
		m_Color = color;
		m_bFadeInProgress = false;
	}

	//! starts a fade effect
	void FadeRenderer::StartFade(const Color& srcColor, const Color& destColor, float fDuration, bool additive, bool stayFadedUntilFurtherNotice /*= false*/)
	{
		m_Color = srcColor;
		m_FadeSrcColor = srcColor;
		m_FadeDestColor = destColor;		
		m_fFadeDuration = fDuration;
		m_Additive = additive;
		m_fFadeTimer = 0.0f;
		m_bFadeInProgress = true;
		m_StayFadedUntilFurtherNotice = stayFadedUntilFurtherNotice;
	}
}

