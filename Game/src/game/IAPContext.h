/*

Amine Rehioui
Created: May 8th 2015

*/

#pragma once

#include "MenuContext.h"

namespace shoot
{
	//! IAPContext
	class IAPContext : public MenuContext
	{
	public:

		//! Macro to provide type information
		DECLARE_OBJECT(IAPContext, MenuContext);

		//! constructor
		IAPContext();

		//! serializes the entity to/from a PropertyStream
		void Serialize(PropertyStream& stream) override;

		//! called during the initialization of the entity
		void Init() override;

		//! called during the update of the entity
		void Update() override;

		//! registers the entity and its children for rendering
		void RegisterForRendering() override;

		//! OnEnter
		void OnEnter() override;

		//! OnExit
		void OnExit() override;

		//! UpdateUI
		void UpdateUI();

	private:

		Reference<EntityRenderer> m_TextureRenderer;

		// properties
		Reference<ContextStack> m_FPSContext;
	};
}



