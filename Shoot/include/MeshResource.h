/* 

Amine Rehioui
Created: December 28th 2010

*/

#pragma once


#include "Utils.h"

namespace shoot
{
	//! Mesh Resource
	class MeshResource : public Resource
	{
		DECLARE_OBJECT(MeshResource, Resource);

	public:

		//! SubMeshInfo
		struct SubMeshInfo
		{
			Reference<VertexBuffer> m_VertexBuffer;
			Material::CreationInfo m_MaterialInfo;
			Vector3 m_vPosition;			
		};

		//! constructor
		MeshResource();

		//! destructor
		~MeshResource();

		//! serializes the font from a stream
		void Serialize(PropertyStream& stream);

		//! Load from disk
		void ResourceInit();

		//! AddSubMeshInfo
		void AddSubMeshInfo(VertexBuffer*, const Vector3&, const Material::CreationInfo&);

		//! returns the SubMesh infos
		inline std::vector<SubMeshInfo>& GetSubMeshInfos() { return m_SubMeshInfos; }

		//! returns true if this resource is using the specified data
		virtual bool IsUsingData(const char* strPath) const
		{ 
			return Utils::Equals(m_MeshPath.c_str(), strPath);
		}

		//! returns the bounding box
		const AABBox3D& GetBoundingBox();

		//! returns the bounding box per submesh
		const AABBox3D& GetBoundingBox(int subMeshIndex);

		//! returns instanced rendering status
		inline bool HasInstancedRendering() const { return m_bInstancedRendering; }

		//! returns vertex color status
		inline bool HasVertexColors() const { return m_bVertexColors; }

		//! returns vertex normal status
		inline bool HasVertexNormals() const { return m_bVertexNormals; }

		//! returns tangent / binormal status
		inline bool HasTangentBinormals() const { return m_bTangentBinormals; }

		//! return indices type status
		inline bool Has32BitIndices() const { return _32bitIndices; }

		//! HasMergeSubmeshes
		inline bool HasMergeSubmeshes() const { return m_MergeSubmeshes; }

		//! Load from the shoot file system
		void LoadFS(const char* strPath);

		//! MeshReload
		struct MeshReload
		{
			MeshReload() { MeshResource::m_sMeshReload = true; }
			~MeshReload() { MeshResource::m_sMeshReload = false; }
		};

	private:
		
		std::vector<SubMeshInfo> m_SubMeshInfos;
		AABBox3D m_BoundingBox;
		std::vector<AABBox3D> m_BoundingBoxes;
		bool m_bBoundingBoxesDirty;
		std::string m_MeshPath;

		static bool m_sMeshReload;

		// properties
		bool m_bDynamic;
		bool m_bInstancedRendering;
		bool m_bVertexColors;
		bool m_bVertexNormals;
		bool m_bTangentBinormals;
		bool _32bitIndices;
		bool m_MergeSubmeshes;
	};
}



