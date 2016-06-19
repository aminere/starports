/*

Amine Rehioui
Created: May 8th 2015

*/

#include "Game.h"

#include "IAPContext.h"
#include "Button.h"
#include "Text.h"

namespace shoot
{
	DEFINE_OBJECT(IAPContext);

	//! constructor
	IAPContext::IAPContext()
	{
	}

	//! serializes the entity to/from a PropertyStream
	void IAPContext::Serialize(PropertyStream& stream)
	{
		super::Serialize(stream);

		stream.Serialize("FPSContext", &m_FPSContext);
	}

	//! called during the initialization of the entity
	void IAPContext::Init()
	{
		m_TextureRenderer = EntityRenderer::Instance()->GetTextureRenderer();

		if (m_FPSContext.IsValid())
			m_FPSContext->Init();

		super::Init();
	}

	//! called during the update of the entity
	void IAPContext::Update()
	{
		super::Update();

		if (m_FPSContext.IsValid())
			m_FPSContext->Update();
	}

	//! registers the entity and its children for rendering
	void IAPContext::RegisterForRendering()
	{
		super::RegisterForRendering();
		
		if (m_FPSContext.IsValid())
		{
			EntityRenderer::RenderToTexture rt;
			m_FPSContext->RegisterForRendering();
		}
	}

	//! OnEnter
	void IAPContext::OnEnter()
	{
		auto camera = static_cast<Camera*>(m_FPSContext->GetChildByName("Camera"));
		camera->Activate();

		UpdateUI();
	}

	//! OnExit
	void IAPContext::OnExit()
	{
	}

	//! UpdateUI
	void IAPContext::UpdateUI()
	{
		{
			auto type = GameData::PT_FPSMode;
			auto buyFPS = static_cast<Button*>(GetChildByName("BuyFPS"));
			auto price = static_cast<Text*>(buyFPS->GetChildByName("Price"));
			bool available = GAMEMGR->GetGameData()->HasProduct(type);
			buyFPS->SetUIEnabled(!available);
			price->SetVisible(!available);
			buyFPS->GetChildByName("Available")->SetVisible(available);

			GameData::Product product;
			if (GAMEMGR->GetGameData()->GetProductInfo(type, product))
			{
				if (!available)
					price->SetText(product.m_Price);
			}			

			bool restoreVisible = true;
			#if SHOOT_PLATFORM == SHOOT_PLATFORM_ANDROID
				restoreVisible = false;
			#endif
			GetChildByName("Restore")->SetVisible(restoreVisible);
		}
	}
}

