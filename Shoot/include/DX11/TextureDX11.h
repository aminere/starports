/* 

Amine Rehioui
Created: July 28th 2013

*/

#pragma once

#include "Texture.h"

namespace shoot
{
	//! TextureDX11 class
	class TextureDX11 : public Texture
	{
	public:

		//! constructor
		TextureDX11();

		//! constructor
		TextureDX11(const char* strPath);

		//! destructor
		virtual ~TextureDX11();

		//! Load from disk
		void ResourceInit();

		//! Unload disk data
		void ResourceUninit();

		//! Load into video memory
		void GraphicLoad();

		//! Unload from video memory
		void GraphicUnload();

		//! returns the size of the texture
		Vector2 GetSize() const { return m_vSize; }

		//! returns the hardware size of the texture
		Vector2 GetHardwareSize() const { return GetSize(); }

		//! returns the format of the texture
		E_Format GetFormat() const { return m_eFormat; }

		//! resize the texture
		virtual void Resize(const Vector2& size) { SHOOT_ASSERT(false, "TODO: implement"); }

		//! returns the memory size in bytes
		virtual int GetMemorySize() { return m_MemorySize; }

		//! start rendering with this texture
		void Begin(int stage);

		//! ends rendering with this texture
		void End() { }

	protected:
				
		COMReference<ID3D11Texture2D> m_Texture;
		COMReference<ID3D11ShaderResourceView> m_TextureRV;

		Vector2 m_vSize;
		E_Format m_eFormat;
		int m_MemorySize;
		void* m_pData;
		int m_bpp;
		std::string m_strPath;
	};
}

