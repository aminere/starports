/* 

Amine Rehioui
Created: May 23rd 2014

*/

#include "Shoot.h"

#include "OpenGLMemoryTexture.h"

#include "OpenGLExtensionHandler.h"

namespace shoot
{
	//! constructor
	OpenGLMemoryTexture::OpenGLMemoryTexture()
		: m_pMemoryTextureParent(NULL)
		, m_GLTextureID(0)
	{
	}

	//! destructor
	OpenGLMemoryTexture::~OpenGLMemoryTexture()
	{
		GraphicUnload();
	}

	//! Load into video memory
	void OpenGLMemoryTexture::GraphicLoad()
	{
		if(m_GLTextureID)
		{
			SHOOT_WARNING(false, "OpenGLMemoryTexture video data already loaded");
			return;
		}

		auto data = m_pParent->GetData();
		if (!data)
		{
			SHOOT_WARNING(false, "OpenGLMemoryTexture data not available");
			return;
		}

		GL_CHECK(glGenTextures(1, &m_GLTextureID));
		GL_CHECK(glBindTexture(GL_TEXTURE_2D, m_GLTextureID));
		GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
		GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE_ALPHA, (int)m_pParent->GetHardwareSize().X, (int)m_pParent->GetHardwareSize().Y, 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, data));
		GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
		m_pMemoryTextureParent->SetDirty(false);
	}

	//! Unload from video memory
	void OpenGLMemoryTexture::GraphicUnload()
	{
		if(m_GLTextureID)
		{
			GL_CHECK(glBindTexture(GL_TEXTURE_2D, 0));
			GL_CHECK(glDeleteTextures(1, &m_GLTextureID));
			m_GLTextureID = 0;
		}
	}

	//! start rendering with this texture
	void OpenGLMemoryTexture::Begin(int stage)
	{
		if(m_GLTextureID == 0)		
			GraphicLoad();		

		OpenGLExtensionHandler::Instance()->extGlActiveTexture(GL_TEXTURE0+stage);

		GL_CHECK(glBindTexture(GL_TEXTURE_2D, m_GLTextureID));

		if (m_pMemoryTextureParent->IsDirty())
		{
			GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE_ALPHA, (int)m_pParent->GetHardwareSize().X, (int)m_pParent->GetHardwareSize().Y, 0, GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, m_pParent->GetData()));
			m_pMemoryTextureParent->SetDirty(false);
		}
	}
}

