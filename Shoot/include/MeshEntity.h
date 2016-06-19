/* 

Amine Rehioui
Created: May 12th 2010

*/

#pragma once


#include "MeshResource.h"

namespace shoot
{
	//! Submesh entity
	class SubMesh : public Entity3D
	{
		DECLARE_OBJECT(SubMesh, Entity3D);

	public:

		//! returns the bounding box
		virtual AABBox3D GetBoundingBox() const;
	};

	//! 3D Mesh entity
	class MeshEntity : public Entity3D
	{
	public:

		//! Macro to provide type information
		DECLARE_OBJECT(MeshEntity, Entity3D);

		//! constructor
		MeshEntity();

		//! destructor
		virtual ~MeshEntity();

		//! serializes the entity to/from a PropertyStream
		virtual void Serialize(PropertyStream& stream);

		//! called during the initialization of the entity
		virtual void Init();

		//! Reloads the mesh entity given a mesh path
		void SetMesh(const std::string& path);

		//! returns the mesh resource
		MeshResource* GetMesh() { return m_Mesh; }

		//! returns the submeshes
		std::vector<SubMesh*>& GetSubMeshes() { return m_aSubMeshes; }

		//! returns the bounding box for a particular submesh
		const AABBox3D& GetBoundingBox(const SubMesh* pSubMesh) const;

	private:

		std::vector<SubMesh*> m_aSubMeshes;

		//! find the submeshes among children
		void FindSubMeshes(Entity* pParent, std::vector< Reference<Entity> >& aSubMeshes);
			
		// properties
		Reference<MeshResource> m_Mesh;
	};
}



