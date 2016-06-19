/*

Amine Rehioui
Created: January 3rd 2014

*/

#pragma once

#include "MenuContext.h"

namespace shoot
{
	class LevelSelectCamera;

	//! LevelSelectContext
	class LevelSelectContext : public MenuContext
	{
	public:

		//! Macro to provide type information
		DECLARE_OBJECT(LevelSelectContext, MenuContext);

		//! constructor
		LevelSelectContext();

		//! serializes the entity to/from a PropertyStream
		virtual void Serialize(PropertyStream& stream);

		//! called during the initialization of the entity
		virtual void Init();

		//! called during the update of the entity
		virtual void Update();

		//! registers the entity and its children for rendering
		virtual void RegisterForRendering();

		//! StartSubLevel
		void PlayLevel(int level);

		//! GetCurrentLevel
		inline uint GetCurrentLevel() const { return m_CurrentLevel; }

		//! UpdateCurrentLevel
		void UpdateCurrentLevel(uint level);

		//! OnEnter
		void OnEnter() override;

	private:

		//! LevelInfo
		struct LevelInfo : public ISerializableStruct
		{
			std::string folder;
			Reference<ContextStack> previewContext;

			void Serialize(PropertyStream& stream)
			{
				stream.Serialize("Folder", &folder);
				stream.Serialize("PreviewContext", &previewContext);
			}
		};

		Reference<EntityRenderer> m_TextureRenderer;
		Handle<Material> m_LevelMaterial;
		Handle<Entity2D> m_LevelPanel;
		Handle<Entity> m_Buttons;
		Handle<Entity2D> m_Root;
		Handle<LevelSelectCamera> m_Camera;
		uint m_CurrentLevel;

		// properties
		float m_LineThickness;
		Array<LevelInfo> m_Levels;
	};
}



