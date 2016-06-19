/* 

Amine Rehioui
Created: September 6th 2014

*/

#include "Shoot.h"

#include "EffectEntity.h"

namespace shoot
{
	DEFINE_OBJECT(EffectEntity);

	//! serializes the entity to/from a PropertyStream
	void EffectEntity::Serialize(PropertyStream& stream)
	{
		super::Serialize(stream);

		stream.Serialize("Effect", &m_Effect);
	}

	//! called during the initialization of the entity
	void EffectEntity::Init()
	{
		if(m_Effect.IsValid())		
			m_Effect->Init();		

		super::Init();
	}

	//! registers the entity and its children for rendering
	void EffectEntity::RegisterForRendering()
	{
		if(!m_Effect.IsValid())
			return;

		EntityRenderer::Instance()->RegisterEffect(m_Effect);
	}

	//! called during the update of the entity
	void EffectEntity::Update()
	{
		if(!m_Effect.IsValid())
			return;

		m_Effect->Update();
	}
}

