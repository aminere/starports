/* 

Amine Rehioui
Created: May 12th 2010

*/

#include "Shoot.h"

#include "MeshEntity.h"

#include "MaterialProvider.h"

namespace shoot
{
	DEFINE_OBJECT(SubMesh);
	DEFINE_OBJECT(MeshEntity);

	//! Constructor
	MeshEntity::MeshEntity() 
	{
	}

	//! Destructor
	MeshEntity::~MeshEntity()
	{
		m_aSubMeshes.clear();
	}

	//! serializes the entity to/from a PropertyStream
	void MeshEntity::Serialize(PropertyStream& stream)
	{
		super::Serialize(stream);

		stream.Serialize("Mesh", &m_Mesh);
	}

	//! called during the initialization of the entity
	void MeshEntity::Init()
	{
		m_aSubMeshes.clear();
		std::vector< Reference<Entity> > aUnusedSubMeshes;		
		FindSubMeshes(this, aUnusedSubMeshes);

		if(m_Mesh.IsValid())
		{
			const auto& aSubMesheInfos = m_Mesh->GetSubMeshInfos();
			for (auto& subMeshInfo : aSubMesheInfos)
			{
				SubMesh* pSubMesh = NULL;
				GraphicComponent* pSubMeshGraphic = NULL;

				std::string name = subMeshInfo.m_VertexBuffer->GetName();
				Reference<Entity> subMeshEntity = GetChildByName(name);				
				if(!subMeshEntity.IsValid())
				{
					pSubMesh = snew SubMesh();					
					pSubMesh->SetSavableToXML(true);					
					pSubMesh->SetName(subMeshInfo.m_VertexBuffer->GetName());
					pSubMesh->SetPosition(subMeshInfo.m_vPosition);
					AddChild(pSubMesh, true);

					pSubMeshGraphic = snew GraphicComponent();
					pSubMeshGraphic->SetRenderingPass(GraphicComponent::RP_3D);
					pSubMesh->AddComponent(pSubMeshGraphic, true);
				}
				else
				{
					pSubMesh = DYNAMIC_CAST(subMeshEntity.Get(), SubMesh);					
					SHOOT_ASSERT(pSubMesh, "Invalid SubMesh Entity");
					//pSubMesh->SetPosition(aSubMeshes[i].m_vPosition);
					pSubMeshGraphic = pSubMesh->GetComponent<GraphicComponent>();

					std::vector< Reference<Entity> >::iterator it = std::find(aUnusedSubMeshes.begin(), aUnusedSubMeshes.end(), subMeshEntity);
					aUnusedSubMeshes.erase(it);
				}
				
				pSubMeshGraphic->SetVertexBuffer(subMeshInfo.m_VertexBuffer);

				if(!pSubMeshGraphic->GetMaterial())
					pSubMeshGraphic->SetMaterial(MaterialProvider::Instance()->GetMaterial(subMeshInfo.m_MaterialInfo));

				m_aSubMeshes.push_back(pSubMesh);
			}
		}

		// release unused submeshes
		for (auto& subMesh : aUnusedSubMeshes)		
			RemoveChild(subMesh.Get(), true);

		super::Init();
	}
	
	//! Reloads the mesh entity given a mesh path
	void MeshEntity::SetMesh(const std::string& path)
	{
		m_Mesh = ResourceManager::Instance()->GetResource<MeshResource>(path);		
	}

	//! find the submeshes among children
	void MeshEntity::FindSubMeshes(Entity* pParent, std::vector< Reference<Entity> >& aSubMeshes)
	{
		for (size_t i = 0; i<pParent->GetChildCount(); ++i)
		{
			Entity* pChild = pParent->GetChild(i);
			if(pChild->IsA(SubMesh::TypeID))
			{
				aSubMeshes.push_back(pChild);
				FindSubMeshes(pChild, aSubMeshes);
			}
			else if(pChild->GetTypeID() == Entity3D::TypeID)
			{
				FindSubMeshes(pChild, aSubMeshes);
			}
		}
	}

	//! returns the bounding box for a particular submesh
	const AABBox3D& MeshEntity::GetBoundingBox(const SubMesh* pSubMesh) const
	{
		for (size_t i=0; i<m_aSubMeshes.size(); ++i)
		{
			if(m_aSubMeshes[i] == pSubMesh)
			{
				return m_Mesh->GetBoundingBox((int)i);
			}
		}
		SHOOT_ASSERT(false, "SubMesh not found");
		return m_Mesh->GetBoundingBox();
	}

	//! returns the bounding box
	AABBox3D SubMesh::GetBoundingBox() const
	{
		MeshEntity* pMeshEntity = GetAncestor<MeshEntity>();
		Matrix44 matrix = const_cast<SubMesh*>(this)->GetWorldTransform();
		AABBox3D bbox3D = (pMeshEntity->GetBoundingBox(this) * matrix.GetScale()) + matrix.GetTranslation();
		return bbox3D;
	}
}

