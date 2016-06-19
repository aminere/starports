/* 

Amine Rehioui
Created: April 16th 2011

*/

#include "Shoot.h"

namespace shoot
{	
	//! constructor
	IReference::IReference()
		: m_pObject(NULL)
		, m_pOwner(NULL)
	{
	}

	//! Sets the referenced object
	void IReference::SetObject(Object* pObject)
	{
		if (m_pObject == pObject)
			return;
				
		if (m_pObject)
			ObjectManager::Instance()->UnregisterReference(this, m_pObject);
		
		if (pObject)
			ObjectManager::Instance()->RegisterReference(this, pObject);
		
		m_pObject = pObject;
	}

	//! Copy
	Object* IReference::CopyObject() const
	{
		return m_pObject ? m_pObject->Copy() : NULL;
	}
}

