/* 

Amine Rehioui
Created: July 8th 2010

*/

#pragma once


#include "Texture.h"

namespace shoot
{
	//! OpenGL Texture class
	class OpenGLTexture : public TextureImpl
	{
	public:

		//! constructor
		OpenGLTexture();

		//! destructor
		virtual ~OpenGLTexture();

		//! Init
		void InitHardwareSize() override;

		//! Load into video memory
		void GraphicLoad();

		//! Unload from video memory
		void GraphicUnload();

		//! returns loaded status
		bool IsLoaded() const { return m_GLTextureID != 0; }

		//! start rendering with this texture
		void Begin(int stage);

	protected:

		GLuint m_GLTextureID;
	};
}



