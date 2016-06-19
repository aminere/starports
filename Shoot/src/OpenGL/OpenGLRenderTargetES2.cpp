/* 

Amine Rehioui
Created: August 15th 2010

*/

#include "Shoot.h"

#include "OpenGLRenderTargetES2.h"

#include "OpenGLExtensionHandler.h"

namespace shoot
{
	//! constructor
	OpenGLRenderTargetES2::OpenGLRenderTargetES2(const Vector2& vSize, int typeMask)
		: super(vSize, typeMask)
		, m_FrameBufferID(0)
		, m_ColorTextureID(0)
		, m_DepthTextureID(0)
		, m_DepthStencilRenderBufferID(0)
		, m_StencilRenderBufferID(0)
	{
	}

	//! destructor
	OpenGLRenderTargetES2::~OpenGLRenderTargetES2()
	{
		GraphicUnload();
	}

	//! Load into video memory
	void OpenGLRenderTargetES2::GraphicLoad()
	{
		if (m_FrameBufferID)
		{
			SHOOT_WARNING(false, "OpenGLRenderTarget already created");
			return;
		}

		if (!OpenGLExtensionHandler::Instance()->HasExtension(OpenGLExtensionHandler::E_FBO))
		{
			SHOOT_WARNING(false, "OpenGLRenderTarget not supported");
			return;
		}

		OpenGLExtensionHandler* pExtHandler = OpenGLExtensionHandler::Instance();
		pExtHandler->extGlGenFramebuffers(1, &m_FrameBufferID);
		pExtHandler->extGlBindFramebuffer(GL_FRAMEBUFFER, m_FrameBufferID);
		GL_CHECK(glGenTextures(1, &m_ColorTextureID));
		
		if ((m_TypeMask & GraphicsDriver::BF_ColorBuffer) && (m_TypeMask & GraphicsDriver::BF_DepthBuffer))
		{
			pExtHandler->extGlGenRenderbuffers(1, &m_DepthStencilRenderBufferID);

			if (m_TypeMask & GraphicsDriver::BF_StencilBuffer)
			{
				if (!pExtHandler->HasExtension(GraphicExtensionHandler::E_PackedDepthStencil))
					pExtHandler->extGlGenRenderbuffers(1, &m_StencilRenderBufferID);
			}
		}
		else if (m_TypeMask & GraphicsDriver::BF_DepthBuffer)
		{
			GL_CHECK(glGenTextures(1, &m_DepthTextureID));
		}
		
		Resize(m_vSize);

		pExtHandler->extGlFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ColorTextureID, 0);
		if (m_DepthStencilRenderBufferID)
		{
			pExtHandler->extGlFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_DepthStencilRenderBufferID);
			if (m_TypeMask & GraphicsDriver::BF_StencilBuffer)
			{
				if (m_StencilRenderBufferID)
					pExtHandler->extGlFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_StencilRenderBufferID);
				else
					pExtHandler->extGlFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_DepthStencilRenderBufferID);				
			}
		}
		else if (m_DepthTextureID)
		{
			pExtHandler->extGlFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_DepthTextureID, 0);
		}
		
		GLenum status = pExtHandler->extGlCheckFramebufferStatus(GL_FRAMEBUFFER);
		if (status != GL_FRAMEBUFFER_COMPLETE)
		{
			SHOOT_ASSERT(false, "OpenGLRenderTarget creation error '0x%x'", status);			
		}
	}

	//! Unload from video memory
	void OpenGLRenderTargetES2::GraphicUnload()
	{
		OpenGLExtensionHandler::Instance()->extGlBindFramebuffer(GL_FRAMEBUFFER, 0);
		glBindTexture(GL_TEXTURE_2D, 0);

		if (m_FrameBufferID)
		{
			OpenGLExtensionHandler::Instance()->extGlDeleteFramebuffers(1, &m_FrameBufferID);
			m_FrameBufferID = 0;
		}

		if (m_ColorTextureID)
		{
			GL_CHECK(glDeleteTextures(1, &m_ColorTextureID));
			m_ColorTextureID = 0;
		}

		if (m_DepthStencilRenderBufferID)
		{
			OpenGLExtensionHandler::Instance()->extGlDeleteRenderbuffers(1, &m_DepthStencilRenderBufferID);
			m_DepthStencilRenderBufferID = 0;

			if (m_StencilRenderBufferID)
			{
				OpenGLExtensionHandler::Instance()->extGlDeleteRenderbuffers(1, &m_StencilRenderBufferID);
				m_StencilRenderBufferID = 0;
			}
		}
		else if (m_DepthTextureID)
		{
			GL_CHECK(glDeleteTextures(1, &m_DepthTextureID));
			m_DepthTextureID = 0;
		}
	}

	//! resize the texture
	void OpenGLRenderTargetES2::Resize(const Vector2& vSize)
	{
		m_vSize = vSize;
		OpenGLExtensionHandler::Instance()->extGlActiveTexture(GL_TEXTURE0);

		if (m_ColorTextureID)
		{			
			GL_CHECK(glBindTexture(GL_TEXTURE_2D, m_ColorTextureID));
			GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
			GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));

			auto wrap = (m_TypeMask & GraphicsDriver::BF_EnableWrap) != 0;
			if (wrap)
			{
				GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
				GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
			}
			else
			{
				GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
				GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
			}
			GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, int(vSize.X), int(vSize.Y), 0, GL_RGB, GL_UNSIGNED_BYTE, 0));
		}

		if (m_DepthStencilRenderBufferID)
		{			
			OpenGLExtensionHandler::Instance()->extGlBindRenderbuffer(GL_RENDERBUFFER, m_DepthStencilRenderBufferID);
			if (m_StencilRenderBufferID)
			{
				OpenGLExtensionHandler::Instance()->extGlRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, int(vSize.X), int(vSize.Y));
				if (m_TypeMask & GraphicsDriver::BF_StencilBuffer)
				{
					OpenGLExtensionHandler::Instance()->extGlBindRenderbuffer(GL_RENDERBUFFER, m_StencilRenderBufferID);
					OpenGLExtensionHandler::Instance()->extGlRenderbufferStorage(GL_RENDERBUFFER, GL_STENCIL_INDEX8, int(vSize.X), int(vSize.Y));
				}
			}
			else
			{
				// packed depth-stencil
				GLenum format = (m_TypeMask & GraphicsDriver::BF_StencilBuffer) ? (GL_DEPTH24_STENCIL8_OES) : (GL_DEPTH_COMPONENT16);
				OpenGLExtensionHandler::Instance()->extGlRenderbufferStorage(GL_RENDERBUFFER, format, int(vSize.X), int(vSize.Y));
			}
		}
		else if (m_DepthTextureID)
		{
			GL_CHECK(glBindTexture(GL_TEXTURE_2D, m_DepthTextureID));
			GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
			GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
			GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
			GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
            GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, int(vSize.X), int(vSize.Y), 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, 0));
		}
	}	
    
    void OpenGLRenderTargetES2::Bind()
    {
        if(m_FrameBufferID == 0)
            GraphicLoad();
        
        OpenGLExtensionHandler::Instance()->extGlBindFramebuffer(GL_FRAMEBUFFER, m_FrameBufferID);
        glViewport(0, 0, int(m_vSize.X), int(m_vSize.Y));
		GraphicsDriver::Instance()->ClearBuffers(m_TypeMask);
    }

	//! binds a texture attached to this render target
	void OpenGLRenderTargetES2::BindTexture(int slot, int type)
	{
		OpenGLExtensionHandler::Instance()->extGlActiveTexture(GL_TEXTURE0 + slot);
		if (type & GraphicsDriver::BF_ColorBuffer)
			glBindTexture(GL_TEXTURE_2D, m_ColorTextureID);
		else
			glBindTexture(GL_TEXTURE_2D, m_DepthTextureID);
	}
}

