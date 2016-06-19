/* 

Amine Rehioui
Created: April 2nd 2012

*/

#pragma once


#include "Resource.h"
#include "Texture.h"

namespace shoot
{
	// forwards
	class Texture;

	//! CubeMap Resource class
	class CubeMapResource : public Resource
	{
		DECLARE_OBJECT(CubeMapResource, Resource);

	public:

		//! serializes the object to/from a PropertyStream
		void Serialize(PropertyStream& stream);

		//! returns the cube map texture
		Texture* GetTexture() { return m_CubeMapTexture.Get(); }

		//! returns true if this resource is using the specified data
		virtual bool IsUsingData(const char* strPath) const { return m_CubeMapTexture->IsUsingData(strPath); }

	protected:

		Reference<Texture> m_CubeMapTexture;

		// properties
		Array<IPath> m_aTextures;
	};
}



