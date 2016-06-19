/* 

Amine Rehioui
Created: April 1st 2013

*/

#include "Shoot.h"

namespace shoot
{	
	//! constructor
	IHandle::IHandle()
		: m_pObject(NULL)
	{
	}

	//! Sets the referenced object
	void IHandle::SetObject(Object* pObject)
	{
		if(m_pObject == pObject)
			return;
		
		if(m_pObject)
			ObjectManager::Instance()->UnregisterHandle(m_pObject, this);

		if(pObject)
			ObjectManager::Instance()->RegisterHandle(pObject, this);

		m_pObject = pObject;
	}
}

