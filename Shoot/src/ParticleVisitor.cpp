/* 

Amine Rehioui
Created: November 3rd 2013

*/

#include "Shoot.h"	

#include "ParticleVisitor.h"
#include "ParticleGenerator.h"

namespace shoot
{
	DEFINE_OBJECT(ParticleVisitor);

	//! constructor
	ParticleVisitor::ParticleVisitor()
		// properties
		: m_bEmitterActive(true)
	{
	}

	//! Reads/Writes struct properties from/to a stream
	void ParticleVisitor::Serialize(PropertyStream& stream)
	{
		super::Serialize(stream);
		
		stream.Serialize("EmitterActive", &m_bEmitterActive);		
	}

	//! visits a particular entity
	void ParticleVisitor::Visit(Entity* pTarget)
	{
		SHOOT_ASSERT(pTarget->IsA(ParticleGenerator::TypeID), "ParticleVisitor target is not of type ParticleGenerator");
		if(ParticleGenerator* pEmitter = DYNAMIC_CAST(pTarget, ParticleGenerator))		
			pEmitter->SetActive(m_bEmitterActive);
		super::Visit(pTarget);		
	}	
}

