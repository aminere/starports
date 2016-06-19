/* 

Amine Rehioui
Created: April 20th 2012

*/

#include "Shoot.h"

#include "FakeCubeMapTexture.h"

namespace shoot
{
	DEFINE_OBJECT(FakeCubeMapTexture);

	//! constructor
	FakeCubeMapTexture::FakeCubeMapTexture(const char* strPath[6])
	{
		sdelete(m_pImpl);
		for(int i=0; i<6; ++i)
		{
			auto texture = snew Texture();
			auto xmlPath = Utils::GetFilePathNoExt(strPath[i]) + ".xml";
			ObjectManager::Instance()->SetTemplatePath(texture, xmlPath);
			//texture->Set32Bits(true);
			texture->SetIgnoreResourceFileOnInit(true);
			texture->ResourceInit();
			
			m_Textures[i] = texture;
		}
		m_vSize = m_Textures[0]->GetSize();
		m_eFormat = m_Textures[0]->GetFormat();
	}

	//! constructor
	FakeCubeMapTexture::FakeCubeMapTexture()
	{
		sdelete(m_pImpl);
	}
}

