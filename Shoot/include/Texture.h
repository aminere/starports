/* 

Amine Rehioui
Created: April 21st 2010

*/

#pragma once


#include "Resource.h"

namespace shoot
{
	class Texture;

	//! TextureImpl interface
	class TextureImpl : public GraphicObject
	{
	public:

		//! constructor
		TextureImpl()
			: m_pParent(NULL)
		{
		}

		//! destructor
		virtual ~TextureImpl()
		{
		}

		//! start rendering with this texture
		virtual void Begin(int stage) = 0;

		//! Init
		virtual void InitHardwareSize();

		//! SetParent
		virtual void SetParent(Texture* parent) { m_pParent = parent; }

	protected:

		Texture* m_pParent;
	};

	//! NULLTextureImpl
	class NULLTextureImpl : public TextureImpl
	{
	public:
		//! start rendering with this texture
		void Begin(int stage) override { }

		//! Init
		void InitHardwareSize() override { }
	};

	//! base Texture class
	class Texture : public Resource
	{
		DECLARE_OBJECT(Texture, Resource);		

	public:

		//! texture format
		enum E_Format
		{
			TF_RGB,
			TF_RGBA,
			TF_Count,
			TF_Unknown
		};	

		//! constructor
		Texture();

		//! destructor
		virtual ~Texture();

		//! Serialize
		void Serialize(PropertyStream& stream);

		//! ResourceInit
		virtual void ResourceInit();

		//! ResourceUninit
		virtual void ResourceUninit();

		//! returns true if this resource is using the specified data
		bool IsUsingData(const char* strPath) const
		{
			auto pngPath = Utils::GetFilePathNoExt(GetTemplatePath()) + ".png";
			return Utils::Equals(pngPath.c_str(), strPath);
		}

		//! returns the size of the texture
		inline const Vector2& GetSize() const { return m_vSize; }

		//! returns the hardware size of the texture
		inline const Vector2& GetHardwareSize() const { return m_vHardwareSize; }

		//! SetHardwareSize
		inline void SetHardwareSize(const Vector2& size) { m_vHardwareSize = size; }

		//! returns the format of the texture
		inline E_Format GetFormat() const { return m_eFormat; }

		//! GetData
		inline void* GetData() const { return m_pData; }

		//! ClearData
		void ClearData();

		//! GetBPP
		inline int GetBPP() const { return m_bpp; }

		//! start rendering with this texture
		virtual void Begin(int stage) { m_pImpl->Begin(stage); }

		//! HasMipMaps
		inline bool HasMipMaps() const { return m_MipMaps; }

		//! Is32Bits
		inline bool Is32Bits() const { return m_32Bits; }

		//! Set32Bits
		inline void Set32Bits(bool set) { m_32Bits = set; }

		//! SetIgnoreResourceFileOnInit
		inline void SetIgnoreResourceFileOnInit(bool ignore) { m_IgnoreResourceFileOnInit = ignore; }

	protected:

		//! LoadFromBinPng
		void LoadFromBinPng(const std::string& path, int& channels, int& bpp);

		Vector2 m_vSize;
		Vector2 m_vHardwareSize;
		E_Format m_eFormat;
		void* m_pData;
		int m_bpp;
		bool m_bOwnData;
		bool m_IgnoreResourceFileOnInit;

		// properties
		bool m_MipMaps;
		bool m_32Bits;

		TextureImpl* m_pImpl;
	};
}



