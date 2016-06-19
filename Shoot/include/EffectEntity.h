/* 

Amine Rehioui
Created: September 6th 2014

*/

#pragma once


#include "Effect.h"

namespace shoot
{
	//! EffectEntity
	class EffectEntity : public Entity
	{
	public:

		DECLARE_OBJECT(EffectEntity, Entity);

		//! serializes the entity to/from a PropertyStream
		void Serialize(PropertyStream& stream);

		//! called during the initialization of the entity
		void Init();

		//! registers the entity for rendering
		void RegisterForRendering();

		//! called during the update of the entity
		void Update();

		//! returns the effect
		inline Effect* GetEffect() const { return m_Effect.Get(); }

	private:

		// properties
		Reference<Effect> m_Effect;
	};
}



