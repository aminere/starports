/* 

Amine Rehioui
Created: March 27h 2012

*/

#pragma once


#include "Material.h"

namespace shoot
{
	//! Material provider
	class MaterialProvider : public Singleton<MaterialProvider>
	{
		DECLARE_SINGLETON(MaterialProvider);

	public:

		//! FindMaterial - returns NULL if not found
		Material* FindMaterial(const Material::CreationInfo& info);

		//! CreateMaterial - create a new material
		Material* CreateMaterial(const Material::CreationInfo& info);

		//! GetMaterial - creates a new instance if not found
		Material* GetMaterial(const Material::CreationInfo& info);

	private:

		// material pool
		std::list< Handle<Material> > m_lMaterials;
	};
}




