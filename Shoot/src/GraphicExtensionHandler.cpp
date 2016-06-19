/* 

Amine Rehioui
Created: July 6th 2013

*/

#include "Shoot.h"

#include "GraphicExtensionHandler.h"

#if SHOOT_PLATFORM == SHOOT_PLATFORM_IOS || SHOOT_PLATFORM == SHOOT_PLATFORM_ANDROID
#include "OpenGLExtensionHandlerES2.h"
#elif !defined(DX11)
#include "OpenGLExtensionHandlerWin32.h"
#endif

namespace shoot
{
	//! static variables initialization
	GraphicExtensionHandler* GraphicExtensionHandler::m_spInstance = NULL;

	//! CreateInstance
	void GraphicExtensionHandler::CreateInstance()
	{
		if(m_spInstance)
		{
#ifndef SHOOT_EDITOR
			SHOOT_ASSERT(false, "Multiple GraphicExtensionHandler instances detected");
#endif // SHOOT_EDITOR
			return;
		}

#if defined(DX11)
		m_spInstance = snew GraphicExtensionHandler();
#elif SHOOT_PLATFORM == SHOOT_PLATFORM_IOS || SHOOT_PLATFORM == SHOOT_PLATFORM_ANDROID
		m_spInstance = snew OpenGLExtensionHandlerES2();
#else
		m_spInstance = snew OpenGLExtensionHandlerWin32();
#endif
	}

	//! destroys the driver
	void GraphicExtensionHandler::DestroyInstance()
	{
		sdelete(m_spInstance);
	}

	//! Constructor
	GraphicExtensionHandler::GraphicExtensionHandler()	
	{
		for(int i=0; i<E_Count; ++i)
		{
			m_bHasExtension[i] = false;
		}		
	}
}

