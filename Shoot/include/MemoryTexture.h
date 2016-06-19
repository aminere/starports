/* 

Amine Rehioui
Created: May 23rd 2014

*/

#pragma once


namespace shoot
{
	//! MemoryTexture
	class MemoryTexture : public Texture
	{
		DECLARE_OBJECT(MemoryTexture, Texture);

	public:
		
		//! constructor
		MemoryTexture(uchar* pData, const Vector2& vSize);

		//! constructor
		MemoryTexture();

		//! ResourceInit
		void ResourceInit() override { }

		//! ResourceUninit
		void ResourceUninit() override { }

		//! Updates the data
		void UpdateData(uchar* pData, const Vector2& vSize);

		//! IsDirty
		inline bool IsDirty() const { return m_bDirty; }

		//! SetDirty
		inline void SetDirty(bool dirty) { m_bDirty = dirty; }

	protected:
		
		bool m_bDirty;
	};
}



