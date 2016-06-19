/* 

Amine Rehioui
Created: April 19th 2012

*/

#pragma once


#include "OpenGLVertexBufferNoVAO.h"

namespace shoot
{
	// forwards
	class FakeCubeMapTexture;
		
	class SkyBoxVertexBufferNoCubemap_NoVAO : public OpenGLVertexBufferNoVAO
	{
		DECLARE_OBJECT(SkyBoxVertexBufferNoCubemap_NoVAO, OpenGLVertexBufferNoVAO);

	public:

		//! constructor
		SkyBoxVertexBufferNoCubemap_NoVAO()
		{
		}

		//! constructor
		SkyBoxVertexBufferNoCubemap_NoVAO(FakeCubeMapTexture* pTexture);

		//! render using this vertex buffer
		void Draw(const std::vector<Matrix44>& aWorldTransforms, const std::vector<Matrix44>& aTextureTransforms);

	private:

		FakeCubeMapTexture* m_pTexture;
	};
}




