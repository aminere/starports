/* 

Amine Rehioui
Created: April 20th 2012

*/

#pragma once


#include "Texture.h"

namespace shoot
{
	//! Fake CubeMap Texture class
	class FakeCubeMapTexture : public Texture
	{
		DECLARE_OBJECT(FakeCubeMapTexture, Texture);

	public:

		//! constructor
		FakeCubeMapTexture(const char* strPath[6]);

		//! constructor
		FakeCubeMapTexture();

		//! returns a texture
		Texture* GetTexture(int index) { return m_Textures[index]; }

		//! start rendering with this texture
		void Begin(int stage) override { }

		//! ResourceInit
		void ResourceInit() override { }

	protected:

		Reference<Texture> m_Textures[6];
	};
}



