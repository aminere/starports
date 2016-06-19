/* 

Amine Rehioui
Created: July 14th 2010

*/

#include "Shoot.h"

#include "VertexBuffer.h"

#include "GraphicExtensionHandler.h"

#if defined(DX11)
#include "DirectX11Driver.h"
#elif SHOOT_PLATFORM == SHOOT_PLATFORM_IOS || SHOOT_PLATFORM == SHOOT_PLATFORM_ANDROID
#include "OpenGLDriverES2.h"
#else
#include "OpenGLDriver.h"
#endif

namespace shoot
{
	GraphicsDriver* GraphicsDriver::m_spInstance = NULL;

	//! creates a driver - the singleton pattern was not used because GraphicsDriver has pure virtual methods
	void GraphicsDriver::CreateInstance()
	{
		SHOOT_ASSERT(!m_spInstance, "Cannot create multiple GraphicsDriver instances");

#if SHOOT_PLATFORM == SHOOT_PLATFORM_IOS || SHOOT_PLATFORM == SHOOT_PLATFORM_ANDROID
		m_spInstance = snew OpenGLDriverES2();
#elif SHOOT_PLATFORM == SHOOT_PLATFORM_WIN32 
	#ifdef DX11
		m_spInstance = snew DirectX11Driver();
	#else		
		m_spInstance = snew OpenGLDriver();
	#endif
#elif SHOOT_PLATFORM == SHOOT_PLATFORM_WP8
	m_spInstance = snew DirectX11Driver();
#else
	m_spInstance = snew NullGraphicsDriver();
#endif
	}

	//! destroys the driver
	void GraphicsDriver::DestroyInstance()
	{
		SHOOT_ASSERT(m_spInstance, "Cannot destroy NULL GraphicsDriver instance");
		delete m_spInstance;
	}

	//! driver initialization
	void GraphicsDriver::Init()
	{
		GraphicExtensionHandler::CreateInstance();
	}

	//! Constructor
	GraphicsDriver::GraphicsDriver()
	{
		for(int i=0; i<TS_Count; ++i)
		{
			m_StateMatrices[i] = Matrix44::Identity;
		}
	}

	//! Destructor
	GraphicsDriver::~GraphicsDriver()
	{
		GraphicExtensionHandler::DestroyInstance();

		SHOOT_ASSERT(m_GraphicObjectMap.empty(), "GraphicObject Leaks Detected");
		m_spInstance = NULL;
	}

	//! resizes the screen surface
	void GraphicsDriver::ResizeScreen(const Vector2& vSize)
	{
		SetViewPort(AABBox2D(Vector2::Create(0.0f, 0.0f), vSize));
	}

	//! registers a graphic object
	void GraphicsDriver::RegisterObject(GraphicObject* pObject)
	{
		SHOOT_ASSERT(m_GraphicObjectMap.find(pObject) == m_GraphicObjectMap.end(), "Trying to register a GraphicObject twice");
		m_GraphicObjectMap[pObject] = 0;
	}

	//! unregister a graphic object
	void GraphicsDriver::UnregisterObject(GraphicObject* pObject)
	{
		SHOOT_ASSERT(m_GraphicObjectMap.find(pObject) != m_GraphicObjectMap.end(), "Trying to unregister an unexisting GraphicObject");
		m_GraphicObjectMap.erase(pObject);
	}

	//! unloads graphic objects
	void GraphicsDriver::UnloadObjects()
	{
		for(GraphicObjectMap::iterator it = m_GraphicObjectMap.begin(); it != m_GraphicObjectMap.end(); ++it)
		{
			(*it).first->GraphicUnload();
		}
	}

	//! loads graphic objects
	void GraphicsDriver::LoadObjects()
	{
		for(GraphicObjectMap::iterator it = m_GraphicObjectMap.begin(); it != m_GraphicObjectMap.end(); ++it)
		{
			if(!(*it).first->IsLoaded())
			{
				(*it).first->GraphicLoad();
			}
		}
	}

	//! create a vertex buffer
	VertexBuffer* NullGraphicsDriver::CreateVertexBuffer(bool bInstancedRendering /*= false*/)
	{ 
		return snew VertexBuffer();
	}

	//! create a skybox vertex buffer
	VertexBuffer* NullGraphicsDriver::CreateSkyBoxVertexBufferNoCubemap(Texture* pTexture)
	{ 
		return snew VertexBuffer();
	}
}

