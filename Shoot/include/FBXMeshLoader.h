/* 

Amine Rehioui
Created: July 17th 2013

*/

#pragma once


#include "MeshResource.h"

// forwards
namespace fbxsdk_2015_1
{
	class FbxManager;
	class FbxNode;
	class FbxMesh;
}

namespace shoot
{
	//! FBX mesh loader
	class FBXMeshLoader
	{
	public:

		//! constructor
		FBXMeshLoader(MeshResource* pMesh)
			: m_pMesh(pMesh)
			, m_SkipTextureLoad(false)
		{
		}

		//! loads a mesh from a file
		void Load(const std::string& path, File* binFile, bool skipTextureLoad = false);

		//! static uninitialization
		static void Uninit();

	private:

		//! FBX Loading
		void LoadNode(fbxsdk_2015_1::FbxNode* node);
		void LoadMesh(fbxsdk_2015_1::FbxNode* meshNode);
		void LoadVertices(fbxsdk_2015_1::FbxMesh* mesh, Vertex3D* pVertices, int numVertices, bool orientable);
		void LoadUVs(fbxsdk_2015_1::FbxMesh* mesh, Vertex3D* pVertices, int numVertices, uint& vertexFormat);
		void LoadVertexColors(fbxsdk_2015_1::FbxMesh* mesh, Vertex3D* pVertices, int numVertices, uint& vertexFormat);
		void LoadVertexNormals(fbxsdk_2015_1::FbxMesh* mesh, Vertex3D* pVertices, int numVertices, uint& vertexFormat, bool orientable);
		void LoadTangentBinormals(fbxsdk_2015_1::FbxMesh* mesh, Vertex3D* pVertices, int numVertices, uint& vertexFormat);
		Material::CreationInfo LoadMaterial(fbxsdk_2015_1::FbxNode* node);
				
		MeshResource* m_pMesh;
		std::string m_Path;
		bool m_SkipTextureLoad;
		std::vector< Reference<Texture> > m_DummyTextures;
		
		//! FbxSubMesh
		struct FbxSubMesh
		{
			std::string m_Name;
			Vector3 m_Position;
			Vertex3D* m_Vertices;
			uint m_NumVertices;
			uint m_VertexFormat;
		};

		//! Group
		struct Group
		{
			Material::CreationInfo m_MaterialInfo;
			std::vector<FbxSubMesh> m_SubMeshes;
		};

		std::vector<Group> m_Groups;
		static fbxsdk_2015_1::FbxManager* ms_pManager;
	};
}



