/* 

Amine Rehioui
Created: August 12th 2010

*/

#include "Shoot.h"

#include "SkyBoxEntity.h"

#include "GeometryProvider.h"

namespace shoot
{
	DEFINE_OBJECT(SkyBoxEntity);

	//! constructor
	SkyBoxEntity::SkyBoxEntity()
	{
		m_bAlwaysActive = true;
	}

	//! serializes the entity to/from a PropertyStream
	void SkyBoxEntity::Serialize(PropertyStream& stream)
	{
		super::Serialize(stream);
		
		stream.Serialize("CubeMap", &m_CubeMap);
	}

	//! called during the initialization of the entity
	void SkyBoxEntity::Init()
	{
		super::Init();

		if(!m_CubeMap.IsValid())
			return;
		
		GraphicComponent* pGraphic = GetComponent<GraphicComponent>();
		if(!pGraphic)
		{
			pGraphic = snew GraphicComponent();
			pGraphic->SetRenderingPass(GraphicComponent::RP_SkyBox);
			AddComponent(pGraphic, true);
		}

		Material* pMaterial = pGraphic->GetMaterial();
		if(!pMaterial)
		{
			pMaterial = snew Material();
			pMaterial->SetShader(Material::m_strDefaultShader);
			pGraphic->SetMaterial(pMaterial);
		}

		pMaterial->SetTexture(0, m_CubeMap->GetTexture());
		if(!pGraphic->GetVertexBuffer())
		{
			pGraphic->SetVertexBuffer(GeometryProvider::Instance()->GetSkyBoxVertexBuffer(pMaterial->GetTexture()));
		}
	}	

	//! sets the cube map
	void SkyBoxEntity::SetCubeMap(const std::string& path)
	{
		m_CubeMap = ResourceManager::Instance()->GetResource<CubeMapResource>(path);
	}	
}


