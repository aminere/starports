/* 

Amine Rehioui
Created: August 1st 2013

*/

#pragma once

#include "VertexBufferDX11.h"

namespace shoot
{
	// forwards
	class FakeCubeMapTexture;
		
	//! SkyBoxVertexBufferNoCubemapDX11
	class SkyBoxVertexBufferNoCubemapDX11 : public VertexBufferDX11
	{
		DECLARE_OBJECT(SkyBoxVertexBufferNoCubemapDX11, VertexBufferDX11);

	public:

		//! constructor
		SkyBoxVertexBufferNoCubemapDX11()
		{
		}

		//! constructor
		SkyBoxVertexBufferNoCubemapDX11(FakeCubeMapTexture* pTexture);

		//! begins rendering using this vertex buffer
		void Begin(Shader* pShader);

		//! render using this vertex buffer
		void Draw(const std::vector<Matrix44>& aWorldTransforms, const std::vector<Matrix44>& aTextureTransforms);

		//! ends rendering by this vertex buffer
		void End();

	private:

		FakeCubeMapTexture* m_pTexture;
	};
}

