/* 

Amine Rehioui
Created: January 4th 2012

*/

#include "Shoot.h"

#include "Texture.h"
#include "File.h"
#include "PNGLoader.h"

namespace shoot
{
	DEFINE_OBJECT(Texture);

	//! constructor
	Texture::Texture()
		: m_eFormat(TF_Unknown)
		, m_vSize(Vector2::Zero)
		, m_vHardwareSize(Vector2::Zero)
		, m_pData(NULL)
		, m_bpp(2)
		, m_bOwnData(true)
		, m_IgnoreResourceFileOnInit(false)
		// properties
		, m_MipMaps(true)
		, m_32Bits(false)
	{
		m_pImpl = GraphicsDriver::Instance()->CreateTexture();
		m_pImpl->SetParent(this);
	}

	//! destructor
	Texture::~Texture()
	{
		ResourceUninit();
		sdelete(m_pImpl);
	}

	//! Serialize
	void Texture::Serialize(PropertyStream& stream)
	{
		stream.Serialize("MipMaps", &m_MipMaps);
		stream.Serialize("32Bits", &m_32Bits);
	}

	//! ResourceInit
	void Texture::ResourceInit()
	{
		if (m_pData)
		{
			SHOOT_LOG_WARNING(false, "Texture '%s' disk data already loaded", GetTemplatePath().c_str());
			return;
		}

		int channels;
		auto texResourcePath = GetTemplatePath();
		auto pngPath = Utils::GetFilePathNoExt(texResourcePath) + ".png";

#ifndef SHOOT_EDITOR
		LoadFromBinPng(pngPath, channels, m_bpp);
#else
		auto binPath = Utils::GetBinPath(pngPath);
		bool dirty = true;
		if (File::Exists(binPath.c_str()))
		{
			dirty = Utils::IsFileMoreRecent(pngPath, binPath);
			dirty = dirty || (!m_IgnoreResourceFileOnInit && Utils::IsFileMoreRecent(texResourcePath, binPath));
		}			

		if (dirty)
		{
			Log.Print("Converting PNG '%s'\n", pngPath.c_str());
			auto binFile = Utils::CreateBinFile(binPath);

			int width, height;
			m_pData = PNGLoader::LoadFromRawPng(pngPath, width, height, channels, m_bpp, binFile, m_32Bits);
			m_vSize.Set(float(width), float(height));

			binFile->Close();
			sdelete(binFile);
		}
		else
			LoadFromBinPng(binPath, channels, m_bpp);
#endif

		if (!m_pData)
			return;

		m_pImpl->InitHardwareSize();

		// Emulate non-pow 2
		{
			Size size((int)m_vSize.X, (int)m_vSize.Y);
			Size hwSize((int)m_vHardwareSize.X, (int)m_vHardwareSize.Y);
			if ((size != hwSize))
			{
				uchar* pOldData = static_cast<uchar*>(m_pData);
				uchar* pNewData = snew uchar[hwSize.Width * hwSize.Height * m_bpp];
				memset(pNewData, 0, hwSize.Width * hwSize.Height * m_bpp);

				for (int row = 0; row < size.Height; ++row)
					memcpy(pNewData + row*hwSize.Width*m_bpp, pOldData + row*size.Width*m_bpp, size.Width*m_bpp);

				if (m_bOwnData)
				{
					sdelete_array(pOldData);
				}
				m_pData = pNewData;
				m_bOwnData = true;
			}
		}

		switch (channels)
		{
		case 3:	m_eFormat = TF_RGB;	break;
		case 4:	m_eFormat = TF_RGBA; break;
		default: SHOOT_ASSERT(0, "Texture '%s' has unsupported format", pngPath.c_str());
		}
	}

	//! ResourceUninit
	void Texture::ResourceUninit()
	{
		if (!m_pImpl)
			return;

		m_pImpl->GraphicUnload();
		ClearData();		
	}

	//! LoadFromBinPng
	void Texture::LoadFromBinPng(const std::string& path, int& channels, int& bpp)
	{
		int width;
		int height;
		File* pFile = File::Create(path.c_str(), File::M_ReadBinary);
		pFile->Open();
		pFile->ReadData(&width, 1);
		pFile->ReadData(&height, 1);
		pFile->ReadData(&channels, 1);
		pFile->ReadData(&bpp, 1);

		auto size = width*height*bpp;
		if (File::GetUseDataFolder())
		{
			auto data = snew uchar[size];
			pFile->ReadData(data, size);
			m_pData = data;
			m_bOwnData = true;
		}
		else
		{
			m_pData = pFile->GetData(size);
			m_bOwnData = false;
		}

		m_vSize.Set(float(width), float(height));
		pFile->Close();
		delete pFile;
	}

	//! ClearData
	void Texture::ClearData()
	{
		if (m_bOwnData)
		{
			if (m_bpp == 2)
			{
				ushort* pData = reinterpret_cast<ushort*>(m_pData);
				sdelete_array(pData);
			}
			else
			{
				uchar* pData = reinterpret_cast<uchar*>(m_pData);
				sdelete_array(pData);
			}
		}
		m_pData = NULL;
	}

	//! TextureImpl::InitHardwareSize() 
	void TextureImpl::InitHardwareSize()
	{
		m_pParent->SetHardwareSize(m_pParent->GetSize());
	}
}

