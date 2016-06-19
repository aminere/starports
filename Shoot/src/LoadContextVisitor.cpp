/* 

Amine Rehioui
Created: June 15th 2013

*/

#include "Shoot.h"	

#include "LoadContextVisitor.h"

namespace shoot
{
	DEFINE_OBJECT(LoadContextVisitor);

	//! Reads/Writes struct properties from/to a stream
	void LoadContextVisitor::Serialize(PropertyStream& stream)
	{
		super::Serialize(stream);

		stream.Serialize("ContextStack", &m_ContextStack);				
	}	

	//! visits a particular entity
	void LoadContextVisitor::Visit(Entity* pTarget)
	{
		if(!m_ContextStack.empty())
			Engine::Instance()->LoadContextStack(m_ContextStack.c_str(), false, [](){});
	}
}

