/* 

Amine Rehioui
Created: July 14th 2010

*/

#pragma once

#ifndef _RETAIL_
	#ifdef SHOOT_EDITOR
		#define GL_CHECK(x) (x); SHOOT_LOG_WARNING(glGetError() == GL_NO_ERROR, #x" failed");
	#else
		#define GL_CHECK(x) \
				{ \
					(x); \
					auto e = glGetError(); \
					SHOOT_WARNING(glGetError() == GL_NO_ERROR, #x" failed with error 0x%x", e); \
				}
	#endif
#else
	#define GL_CHECK(x) (x);
#endif

#if SHOOT_PLATFORM == SHOOT_PLATFORM_IOS
	#include <OpenGLES/ES2/gl.h>
	#include <OpenGLES/ES2/glext.h>
#elif SHOOT_PLATFORM == SHOOT_PLATFORM_ANDROID
	#include <GLES2/gl2.h>
	#include <GLES2/gl2ext.h>
	#include <EGL/egl.h>
#else // desktop OGL
	#include <GL/gl.h>
	#include <GL/glu.h>
	#include "glext.h"
#endif



