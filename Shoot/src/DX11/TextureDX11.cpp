/* 

Amine Rehioui
Created: July 28th 2013

*/

#include "Shoot.h"

#include "TextureDX11.h"
#include "DirectX11Driver.h"

#include "TextureLoader.h"

namespace shoot
{
	//! constructor
	TextureDX11::TextureDX11()
		: m_pData(NULL)
		, m_MemorySize(0)
		, m_eFormat(TF_Unknown)
		, m_bpp(4)
	{
	}

	//! constructor
	TextureDX11::TextureDX11(const char* strPath)
		: Texture()
		, m_pData(NULL)
		, m_MemorySize(0)
		, m_strPath(strPath)
		, m_bpp(4)
	{
	}

	//! destructor
	TextureDX11::~TextureDX11()
	{
		ResourceUninit();
	}

	//! Load from disk
	void TextureDX11::ResourceInit()
	{
		if(m_pData)
		{
			SHOOT_WARNING(false, "Texture '%s' disk data already loaded", m_strPath.c_str());
			return;
		}

		int width, height, channels;
		m_pData = TextureLoader::Load(m_strPath.c_str(), width, height, channels, m_bpp);
		m_vSize.Set(float(width), float(height));

		switch(channels)
		{
		case 3:	m_eFormat = TF_RGB;	break;
		case 4:	m_eFormat = TF_RGBA; break;
		default: SHOOT_ASSERT(0, "Texture has unsupported format");
		}
	}

	//! Unload disk data
	void TextureDX11::ResourceUninit()
	{
		GraphicUnload();
		sdelete_array(m_pData);
	}

	//! Load into video memory
	void TextureDX11::GraphicLoad()
	{
		if(m_TextureRV.Get())
		{
			SHOOT_WARNING(false, "Texture '%s' data already loaded", m_strPath.c_str());
			return;
		}

		if(!m_pData)
		{
			SHOOT_WARNING(false, "Texture '%s' disk data not available", m_strPath.c_str());
			ResourceInit();
		}

		const Size size((int)m_vSize.X, (int)m_vSize.Y);		

		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));
		desc.Width = size.Width;
		desc.Height = size.Height;
		desc.ArraySize = 1;		
		desc.Format = (m_eFormat == TF_RGBA) ? DXGI_FORMAT_R8G8B8A8_UNORM : DXGI_FORMAT_B5G6R5_UNORM;
		desc.SampleDesc.Count = 1;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.CPUAccessFlags = 0;

		if(m_bMipMaps)
		{
			desc.MipLevels = 0;
			desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
			desc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
			ID3D11Texture2D* pTexture = NULL;
			DX_ASSERT(DirectX11Driver::Instance()->GetDevice()->CreateTexture2D(&desc, NULL, &pTexture));
			m_Texture = pTexture;
			DirectX11Driver::Instance()->GetContext()->UpdateSubresource(m_Texture, 0, NULL, m_pData, size.Width*m_bpp, 0);
		}
		else
		{
			desc.MipLevels = 1;
			desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
			desc.MiscFlags = 0;
			D3D11_SUBRESOURCE_DATA texData;
			ZeroMemory(&texData, sizeof(D3D11_SUBRESOURCE_DATA));
			texData.pSysMem = m_pData;
			texData.SysMemPitch = size.Width*m_bpp;
			ID3D11Texture2D* pTexture = NULL;
			DX_ASSERT(DirectX11Driver::Instance()->GetDevice()->CreateTexture2D(&desc, &texData, &pTexture));
			m_Texture = pTexture;
		}

		D3D11_SHADER_RESOURCE_VIEW_DESC desc2;
		ZeroMemory(&desc2, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
		desc2.Format = desc.Format;
		desc2.Texture2D.MipLevels = -1;
		desc2.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		ID3D11ShaderResourceView* pTextureRV = NULL;
		DX_ASSERT(DirectX11Driver::Instance()->GetDevice()->CreateShaderResourceView(m_Texture, &desc2, &pTextureRV));
		m_TextureRV = pTextureRV;
		
		if(m_bMipMaps)
		{
			DirectX11Driver::Instance()->GetContext()->GenerateMips(m_TextureRV);
		}

		m_MemorySize = size.Width*size.Height*m_bpp;
		sdelete_array(m_pData);
	}

	//! Unload from video memory
	void TextureDX11::GraphicUnload()
	{
		m_TextureRV = NULL;
		m_Texture = NULL;
	}
	
	//! start rendering with this texture
	void TextureDX11::Begin(int stage)
	{
		if(m_TextureRV.Get() == NULL)
		{
			GraphicLoad();
		}

		ID3D11ShaderResourceView* pTextureRV = m_TextureRV;
		DirectX11Driver::Instance()->GetContext()->PSSetShaderResources(0, 1, &pTextureRV);
		m_TextureRV = pTextureRV;
	}
}

