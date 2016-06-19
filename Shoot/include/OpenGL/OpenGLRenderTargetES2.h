/* 

Amine Rehioui
Created: August 15th 2010

*/

#pragma once

#include "OpenGLRenderTarget.h"

namespace shoot
{
	//! OpenGL render target texture
	class OpenGLRenderTargetES2 : public RenderTarget
	{
		typedef RenderTarget super;

	public:

		//! constructor
		OpenGLRenderTargetES2(const Vector2& vSize, int typeMask);

		//! destructor
		virtual ~OpenGLRenderTargetES2();

		//! resize the texture
		virtual void Resize(const Vector2& vSize);

		//! start rendering to this render target
		virtual void Bind();

		//! binds a texture attached to this render target
		void BindTexture(int slot, int type);

		//! Load into video memory
		virtual void GraphicLoad();

		//! Unload from video memory
		void GraphicUnload();

	protected:

		GLuint m_FrameBufferID;
		GLuint m_ColorTextureID;
		GLuint m_DepthTextureID;
		GLuint m_DepthStencilRenderBufferID;
		GLuint m_StencilRenderBufferID;
	};
}

