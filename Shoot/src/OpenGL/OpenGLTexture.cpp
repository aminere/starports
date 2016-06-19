/* 

Amine Rehioui
Created: July 8th 2010

*/

#include "Shoot.h"

#include "OpenGLTexture.h"

#include "OpenGLExtensionHandler.h"
#include "File.h"

namespace shoot
{
	//! constructor
	OpenGLTexture::OpenGLTexture()
		: m_GLTextureID(0)
	{
	}	

	//! destructor
	OpenGLTexture::~OpenGLTexture()
	{
		GraphicUnload();
	}

	//! InitHardwareSize
	void OpenGLTexture::InitHardwareSize()
	{
		const auto& size = m_pParent->GetSize();
		Vector2 hwSize = Vector2::Zero;
		if(!OpenGLExtensionHandler::Instance()->HasExtension(OpenGLExtensionHandler::E_RectangleTexture))			
			hwSize.Set(float(Math::GetNextPow2((int)size.X)), float(Math::GetNextPow2((int)size.Y)));
		else
			hwSize = size;

		m_pParent->SetHardwareSize(hwSize);
	}	

	//! Load into video memory
	void OpenGLTexture::GraphicLoad()
	{
		if(m_GLTextureID)
		{
			SHOOT_WARNING(false, "Texture '%s' video data already loaded", m_pParent->GetTemplatePath().c_str());
			return;
		}

		auto data = m_pParent->GetData();
		if (!data)
		{
			SHOOT_WARNING(false, "Texture '%s' disk data not available", m_pParent->GetTemplatePath().c_str());
			return;
		}
		
		GL_CHECK(glGenTextures(1, &m_GLTextureID));
		GL_CHECK(glBindTexture(GL_TEXTURE_2D, m_GLTextureID)); // 2d texture (x and y size)
		GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));	

		// Load texture into video memory
		switch (m_pParent->GetFormat())
		{
		case Texture::TF_RGB:
			GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, (int)m_pParent->GetHardwareSize().X, (int)m_pParent->GetHardwareSize().Y, 0, GL_RGB, (m_pParent->GetBPP() == 2) ? GL_UNSIGNED_SHORT_5_6_5 : GL_UNSIGNED_BYTE, data));
			break;

		case  Texture::TF_RGBA:
			GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (int)m_pParent->GetHardwareSize().X, (int)m_pParent->GetHardwareSize().Y, 0, GL_RGBA, (m_pParent->GetBPP() == 2) ? GL_UNSIGNED_SHORT_4_4_4_4 : GL_UNSIGNED_BYTE, data));
			break;
		}

		if (m_pParent->HasMipMaps())
		{
			GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST));
			OpenGLExtensionHandler::Instance()->extGlGenerateMipmap(GL_TEXTURE_2D);
		}
		else
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);		

		glBindTexture(GL_TEXTURE_2D, 0);
		m_pParent->ClearData();
	}

	//! Unload from video memory
	void OpenGLTexture::GraphicUnload()
	{
		if(m_GLTextureID)
		{
			GL_CHECK(glBindTexture(GL_TEXTURE_2D, 0));
			GL_CHECK(glDeleteTextures(1, &m_GLTextureID));
			m_GLTextureID = 0;
		}
	}	

	//! start rendering with this texture
	void OpenGLTexture::Begin(int stage)
	{
		if(m_GLTextureID == 0)
			GraphicLoad();

		OpenGLExtensionHandler::Instance()->extGlActiveTexture(GL_TEXTURE0+stage);

		GL_CHECK(glBindTexture(GL_TEXTURE_2D, m_GLTextureID));
	}
}

