/* 

Amine Rehioui
Created: May 23rd 2014

*/

#pragma once


#include "MemoryTexture.h"

namespace shoot
{
	//! OpenGLMemoryTexture
	class OpenGLMemoryTexture : public TextureImpl
	{
	public:

		//! constructor
		OpenGLMemoryTexture();

		//! destructor
		virtual ~OpenGLMemoryTexture();

		//! Load into video memory
		void GraphicLoad() override;

		//! Unload from video memory
		void GraphicUnload() override;

		//! returns loaded status
		bool IsLoaded() const override { return m_GLTextureID != 0; }

		//! start rendering with this texture
		void Begin(int stage) override;

		//! SetParent
		void SetParent(Texture* parent) override 
		{ 
			TextureImpl::SetParent(parent);
			m_pMemoryTextureParent = static_cast<MemoryTexture*>(parent); 
		}

	private:

		MemoryTexture* m_pMemoryTextureParent;
		GLuint m_GLTextureID;
	};
}



