/* 

Amine Rehioui
Created: February 5th 2010

*/

//! common shoot engine headers
#pragma once

#define SHOOT_NO_NETWORK
#define SHOOT_NO_GAMEPAD

#define NOMINMAX // needed by cpplinq
#include <list>
#include <vector>
#include <map>
#include <string>
#include <algorithm>
#include <sstream>

// determine shoot platform
#define SHOOT_PLATFORM_WIN32		0
#define SHOOT_PLATFORM_IOS			1
#define SHOOT_PLATFORM_ANDROID		2
#define SHOOT_PLATFORM_WP8			3

#if defined(TARGET_OS_IPHONE)
	#define SHOOT_PLATFORM SHOOT_PLATFORM_IOS
	#ifdef __LP64__
		#define SHOOT_64
	#endif
#elif defined(__ANDROID__)
	#define SHOOT_PLATFORM SHOOT_PLATFORM_ANDROID
	// TODO SHOOT_64
#elif defined(WINAPI_FAMILY_PHONE_APP)
	#define SHOOT_PLATFORM SHOOT_PLATFORM_WP8
	// TODO SHOOT_64
#elif defined(_WIN32) || defined(_WIN64)
	#define SHOOT_PLATFORM SHOOT_PLATFORM_WIN32
	#if defined(_WIN64)
		#define SHOOT_64
	#endif
#else
#error UNSUPPORTED PLATFORM!
#endif

#ifdef SHOOT_64
	#pragma message("using SHOOT_64")
#endif

#ifdef _RETAIL_
	#if SHOOT_PLATFORM != SHOOT_PLATFORM_ANDROID // no pch on android
		#pragma message("Building for _RETAIL_!")
	#endif
#endif

namespace shoot
{
	// types
	typedef unsigned int uint;
	typedef unsigned short ushort;
	typedef unsigned char uchar;

	extern float g_fDeltaTime;
	extern uint g_CurrentFrame;
}

#include "Alloc.h"
#include "Debug.h"
#include "ProfileManager.h"
#include "Reference.h"
#include "Handle.h"
#include "MathUtils.h"
#include "Object.h"
#include "PropertyTypes.h"
#include "PropertyStream.h"
#include "Property.h"
#include "Matrix44.h"
#include "Component.h"
#include "Entity.h"
#include "Entity3D.h"
#include "Entity2D.h"
#include "Singleton.h"
#include "GraphicObject.h"
#include "Texture.h"
#include "GraphicsDriver.h"
#include "GraphicComponent.h"
#include "EntityRenderer.h"
#include "ObjectManager.h"
#include "ObjectFactory.h"
#include "Engine.h"
#include "ResourceManager.h"
#include "Random.h"
#include "Log.h"

#if SHOOT_PLATFORM == SHOOT_PLATFORM_WIN32 
#define _WIN32_WINNT 0x0601 // ref. http://blogs.msdn.com/b/chuckw/archive/2012/04/26/xinput-and-windows-8-consumer-preview.aspx
#endif

#if SHOOT_PLATFORM == SHOOT_PLATFORM_WIN32 || SHOOT_PLATFORM == SHOOT_PLATFORM_WP8
#include <winsock2.h>
#include <windows.h>
#ifdef GetClassName
#undef GetClassName
#endif // GetClassName
#endif

#if defined(DX11)
#include <d3d11_1.h>
#include "COMReference.h"

#define DX_ASSERT(x) SHOOT_VERIFY(SUCCEEDED(x), #x" failed");
#define DX_CHECK(x) SHOOT_WARNING(SUCCEEDED(x), #x" failed");
#else
#include "OpenGL.h"
#endif

