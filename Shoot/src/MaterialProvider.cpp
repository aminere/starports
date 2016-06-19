/* 

Amine Rehioui
Created: March 27h 2012

*/

#include "Shoot.h"

#include "MaterialProvider.h"

namespace shoot
{
	//! constructor
	MaterialProvider::MaterialProvider()
	{
	}

	//! destructor
	MaterialProvider::~MaterialProvider()
	{
	}

	//! FindMaterial - returns NULL if not found
	Material* MaterialProvider::FindMaterial(const Material::CreationInfo& info)
	{
		for (auto it = m_lMaterials.begin(); it != m_lMaterials.end();)
		{
			if ((*it).IsValid())
			{
				if ((*it)->GetCreationInfo() == info)
					return (*it);

				++it;
			}
			else
				it = m_lMaterials.erase(it);
		}

		return NULL;
	}

	//! CreateMaterial - create a new material
	Material* MaterialProvider::CreateMaterial(const Material::CreationInfo& info)
	{
		auto material = snew Material(info);
		m_lMaterials.push_back(Handle<Material>(material));
		return material;
	}

	//! gets a material using creation info. Attempts to match with an existing instance.
	Material* MaterialProvider::GetMaterial(const Material::CreationInfo& info)
	{
		if (auto material = FindMaterial(info))
			return material;
		else
			return CreateMaterial(info);
	}
}

