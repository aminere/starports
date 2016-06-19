/* 

Amine Rehioui
Created: July 28th 2013

*/

#include "Shoot.h"

#include "DirectX11Driver.h"

namespace shoot
{
	//! Sets the referenced object
	void ICOMReference::SetObject(IUnknown* pObject)
	{
		if((m_pObject != pObject) && DirectX11Driver::Instance())
		{
			if(m_pObject)
			{
				DirectX11Driver::Instance()->UnregisterReference(m_pObject);
			}
			if(pObject)
			{
				DirectX11Driver::Instance()->RegisterReference(pObject);
			}
			m_pObject = pObject;
		}
	}
}

