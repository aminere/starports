/* 

Amine Rehioui
Created: January 4th 2012

*/

#include "Shoot.h"

#include "MemoryTexture.h"

namespace shoot
{
	DEFINE_OBJECT(MemoryTexture);

	//! constructor
	MemoryTexture::MemoryTexture(uchar* pData, const Vector2& vSize)
	{
		UpdateData(pData, vSize);

		sdelete(m_pImpl);
		m_pImpl = GraphicsDriver::Instance()->CreateMemoryTexture();		
		m_pImpl->SetParent(this);
		m_pImpl->InitHardwareSize();
		m_bOwnData = false;
	}

	//! constructor
	MemoryTexture::MemoryTexture()
	{
		sdelete(m_pImpl);
		m_bDirty = false;
	}

	//! Updates the data
	void MemoryTexture::UpdateData(uchar* pData, const Vector2& vSize)
	{
		m_pData = pData;
		m_vSize = vSize;
		m_pImpl->InitHardwareSize();
		m_bDirty = true;
	}
}

