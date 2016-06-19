/* 

Amine Rehioui
Created: August 1st 2013

*/

#include "Shoot.h"

#include "SkyBoxVertexBufferNoCubemapDX11.h"

#include "FakeCubeMapTexture.h"

#include "DirectX11Driver.h"

namespace shoot
{
	DEFINE_OBJECT(SkyBoxVertexBufferNoCubemapDX11);

	//! constructor
	SkyBoxVertexBufferNoCubemapDX11::SkyBoxVertexBufferNoCubemapDX11(FakeCubeMapTexture* pTexture)
		: super()
		, m_pTexture(pTexture)
	{
	}

	//! begins rendering using this vertex buffer
	void SkyBoxVertexBufferNoCubemapDX11::Begin(Shader* pShader)
	{
		super::Begin(pShader);
	}

	//! render using this vertex buffer
	void SkyBoxVertexBufferNoCubemapDX11::Draw(const std::vector<Matrix44>& aWorldTransforms, const std::vector<Matrix44>& aTextureTransforms)
	{
		//GraphicsDriver::Instance()->SetColor(m_Material->GetColor()); // TODO DX11
		GraphicsDriver::Instance()->SetTransform(GraphicsDriver::TS_World, aWorldTransforms[0]);
		GraphicsDriver::Instance()->SetTransform(GraphicsDriver::TS_Texture, Matrix44::Identity);
		DirectX11Driver::Instance()->GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		for(int i=0; i<6; ++i)
		{
			m_pTexture->GetTexture(i)->Begin(0);
			DirectX11Driver::Instance()->Draw(6, i*6);
		}
	}
}
