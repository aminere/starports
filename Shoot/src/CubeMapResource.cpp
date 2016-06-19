/* 

Amine Rehioui
Created: April 2nd 2012

*/

#include "Shoot.h"

#include "CubeMapResource.h"

namespace shoot
{
	DEFINE_OBJECT(CubeMapResource);

	//! serializes the object to/from a PropertyStream
	void CubeMapResource::Serialize(PropertyStream& stream)
	{
		stream.Serialize("Textures", &m_aTextures);

		if(stream.GetMode() == SM_Read)
		{
			SHOOT_ASSERT(m_aTextures.GetSize() == 6, "Invalid CubeMap Resource");
			const char* aPaths[6] =
			{
				m_aTextures[0],
				m_aTextures[1],
				m_aTextures[2],
				m_aTextures[3],
				m_aTextures[4],
				m_aTextures[5]
			};
			m_CubeMapTexture = GraphicsDriver::Instance()->CreateCubeMapTexture(aPaths);
		}
	}
}
