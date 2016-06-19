/* 

Amine Rehioui
Created: April 19th 2012

*/

#include "Shoot.h"

#include "SkyBoxVertexBufferNoCubemap_NoVAO.h"

#include "FakeCubeMapTexture.h"

namespace shoot
{
	DEFINE_OBJECT(SkyBoxVertexBufferNoCubemap_NoVAO);

	//! constructor
	SkyBoxVertexBufferNoCubemap_NoVAO::SkyBoxVertexBufferNoCubemap_NoVAO(FakeCubeMapTexture* pTexture)
		: super()
		, m_pTexture(pTexture)
	{
	}

	//! render using this vertex buffer
	void SkyBoxVertexBufferNoCubemap_NoVAO::Draw(const std::vector<Matrix44>& aWorldTransforms, const std::vector<Matrix44>& aTextureTransforms)
	{
		for(int i=0; i<6; ++i)
		{
			m_pTexture->GetTexture(i)->Begin(0);
			SetStartVertex(i*6);
			super::Draw(aWorldTransforms, aTextureTransforms);
		}
	}	
}
