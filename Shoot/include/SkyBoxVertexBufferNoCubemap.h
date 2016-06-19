/* 

Amine Rehioui
Created: April 19th 2012

*/

#pragma once


#include "OpenGLVertexBuffer.h"

namespace shoot
{
	// forwards
	class FakeCubeMapTexture;
		
	class SkyBoxVertexBufferNoCubemap : public OpenGLVertexBuffer
	{
		DECLARE_OBJECT(SkyBoxVertexBufferNoCubemap, OpenGLVertexBuffer);

	public:

		//! constructor
		SkyBoxVertexBufferNoCubemap()
		{
		}

		//! constructor
		SkyBoxVertexBufferNoCubemap(FakeCubeMapTexture* pTexture);

		//! render using this vertex buffer
		void Draw(const std::vector<Matrix44>& aWorldTransforms, const std::vector<Matrix44>& aTextureTransforms);

	private:

		FakeCubeMapTexture* m_pTexture;
	};
}




