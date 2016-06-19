/* 

Amine Rehioui
Created: December 28th 2010

*/

#include "Shoot.h"

#include "MeshResource.h"

#include "Utils.h"

#include "File.h"

#ifdef SHOOT_EDITOR
#include "FBXMeshLoader.h"
#endif

#include "File.h"

namespace shoot
{
	//! statics
	bool MeshResource::m_sMeshReload = false;

	DEFINE_OBJECT(MeshResource);

	//! constructor
	MeshResource::MeshResource()
		: m_bBoundingBoxesDirty(true)
		// properties
		, m_bDynamic(false)
		, m_bInstancedRendering(false)
		, m_bVertexColors(false)
		, m_bVertexNormals(false)
		, m_bTangentBinormals(false)
		, _32bitIndices(false)
		, m_MergeSubmeshes(false)
	{
	}

	//! destructor
	MeshResource::~MeshResource()
	{
	}

	//! serializes the font from a stream
	void MeshResource::Serialize(PropertyStream& stream)
	{
		stream.Serialize("Dynamic", &m_bDynamic);
		stream.Serialize("InstancedRendering", &m_bInstancedRendering);
		stream.Serialize("VertexColors", &m_bVertexColors);
		stream.Serialize("VertexNormals", &m_bVertexNormals);
		stream.Serialize("TangentBinormals", &m_bTangentBinormals);
		stream.Serialize("32bitIndices", &_32bitIndices);
		stream.Serialize("MergeSubmeshes", &m_MergeSubmeshes);
	}

	//! initializes this resource
	void MeshResource::ResourceInit()
	{
		auto meshResourcePath = GetTemplatePath();
		m_MeshPath = Utils::GetFilePathNoExt(meshResourcePath) + ".fbx";

		if (!m_sMeshReload)
			m_SubMeshInfos.clear();

#ifndef SHOOT_EDITOR
		LoadFS(m_MeshPath.c_str());
#else
		auto binPath = Utils::GetBinPath(m_MeshPath);
		bool dirty = true;
		if (File::Exists(binPath.c_str()))
			dirty = Utils::IsFileMoreRecent(m_MeshPath, binPath)
				 || Utils::IsFileMoreRecent(meshResourcePath, binPath);

		if (dirty)
		{
			Log.Print("Converting FBX '%s'\n", m_MeshPath.c_str());
			auto binFile = Utils::CreateBinFile(binPath);
			FBXMeshLoader(this).Load(m_MeshPath.c_str(), binFile);
			binFile->Close();
			sdelete(binFile);
		}
		else
			LoadFS(binPath.c_str());
#endif

		for (auto& submesh : m_SubMeshInfos)
			submesh.m_VertexBuffer->SetDynamic(m_bDynamic);		
	}

	//! AddSubMeshInfo
	void MeshResource::AddSubMeshInfo(VertexBuffer* vb, const Vector3& position, const Material::CreationInfo& materialInfo)
	{
		SubMeshInfo info;
		info.m_VertexBuffer = vb;
		info.m_vPosition = position;
		info.m_MaterialInfo = materialInfo;

		//#ifdef SHOOT_EDITOR
		//	// validate textures
		//	for (auto& texture : info.m_MaterialInfo.m_TextureInfos)
		//	{
		//		auto path = texture->m_Path.GetString();
		//		if (!File::Exists(path.c_str()))
		//		{
		//			texture->m_Path = "";
		//			SHOOT_WARNING(false, "Texture '%s' not found", path.c_str());
		//		}
		//	}
		//#endif

		m_SubMeshInfos.push_back(info);
	}

	//! returns the bounding box
	const AABBox3D& MeshResource::GetBoundingBox()
	{
		if(m_bBoundingBoxesDirty)
		{
			m_BoundingBoxes.clear();

			for (size_t i = 0; i<m_SubMeshInfos.size(); ++i)
			{
				const VertexBuffer* pVB = m_SubMeshInfos[i].m_VertexBuffer.Get();
				Vector3 MinVertex = Vector3::One * Math::Maxf32;
				Vector3 MaxVertex = -Vector3::One * Math::Maxf32;

				for(uint j=0; j<pVB->GetNumIndices(); ++j)
				{
					Vertex3D vertex = reinterpret_cast<Vertex3D*>(pVB->GetVertices())[pVB->GetIndex(j)];

					if(vertex.Pos.X < MinVertex.X) MinVertex.X = vertex.Pos.X;
					else if(vertex.Pos.X > MaxVertex.X) MaxVertex.X = vertex.Pos.X;

					if(vertex.Pos.Y < MinVertex.Y) MinVertex.Y = vertex.Pos.Y;
					else if(vertex.Pos.Y > MaxVertex.Y) MaxVertex.Y = vertex.Pos.Y;

					if(vertex.Pos.Z < MinVertex.Z) MinVertex.Z = vertex.Pos.Z;
					else if(vertex.Pos.Z > MaxVertex.Z) MaxVertex.Z = vertex.Pos.Z;	
				}

				m_BoundingBoxes.push_back(AABBox3D(MinVertex, MaxVertex));
				m_BoundingBox = (i == 0) ? m_BoundingBoxes[i] : (m_BoundingBox + m_BoundingBoxes[i]);				
			}

			m_bBoundingBoxesDirty = false;
		}

		return m_BoundingBox;
	}

	//! returns the bounding box
	const AABBox3D& MeshResource::GetBoundingBox(int subMeshIndex /*= 0*/)
	{
		GetBoundingBox();
		return m_BoundingBoxes[subMeshIndex];
	}

	//! Load from the shoot file system
	void MeshResource::LoadFS(const char* strPath)
	{
		uint numSubMeshes = 0;
		File* pFile = File::Create(strPath, File::M_ReadBinary);
		pFile->Open();
		pFile->ReadData(&numSubMeshes, 1);
		for(uint i=0; i<numSubMeshes; ++i)
		{
			uint numVertices = 0;
			//uint numIndices = 0;
			uint format = 0;
			Vector3 vPosition;
			pFile->ReadData(&vPosition, 1);
			pFile->ReadData(&numVertices, 1);
			//pFile->ReadData(&numIndices, 1);
			pFile->ReadData(&format, 1);

			uint nameLength = 0;
			pFile->ReadData(&nameLength, 1);
			SHOOT_ASSERT(nameLength < 512, "nameLength too long");
			char name[512];
			pFile->ReadData(name, nameLength);
			name[nameLength] = '\0';
			std::string strName(name);
			
			// if vertex buffer already present, do not create a new instance. 
			// This is needed in Android when reloading graphic objects.
			// TODO: verify that this doesn't fuck up editor reloading.			
			MeshResource::SubMeshInfo* subMeshInfo = NULL;
			for (auto& info : m_SubMeshInfos)
			{
				if (info.m_VertexBuffer->GetName() == strName)
				{
					subMeshInfo = &info;
					break;
				}
			}
			if (!subMeshInfo)
			{
				MeshResource::SubMeshInfo newInfo;
				newInfo.m_VertexBuffer = GraphicsDriver::Instance()->CreateVertexBuffer(m_bInstancedRendering);
				newInfo.m_VertexBuffer->SetName(strName);
				m_SubMeshInfos.push_back(newInfo);
				subMeshInfo = &m_SubMeshInfos.back();
			}

			subMeshInfo->m_vPosition = vPosition;
			
			Vertex3D* pVertices = snew Vertex3D[numVertices];
			//void* pIndices = _32bitIndices ? ((void*)snew uint[numIndices]) : ((void*)snew ushort[numIndices]);
			//if (_32bitIndices)
			//	pFile->ReadData<uint>((uint*)pIndices, numIndices);
			//else
			//	pFile->ReadData<ushort>((ushort*)pIndices, numIndices);
			pFile->ReadData(pVertices, numVertices);
			subMeshInfo->m_VertexBuffer->SetVertices(pVertices, numVertices, format);
			//subMeshInfo.m_VertexBuffer->SetIndices(pIndices, numIndices, _32bitIndices);

			Material::CreationInfo materialInfo;
			pFile->ReadData(&materialInfo.m_Color, 1);
			uint numTextures = 0;
			pFile->ReadData(&numTextures, 1);
			for(uint i=0; i<numTextures; ++i)
			{
				uint texturePathLength = 0;
				pFile->ReadData(&texturePathLength, 1);
				SHOOT_ASSERT(texturePathLength < 512, "texturePathLength too long");
				char texturePath[512];
				pFile->ReadData(texturePath, texturePathLength);
				texturePath[texturePathLength] = '\0';
				if (File::Exists(texturePath))
					materialInfo.AddTexture(texturePath);
				else
					SHOOT_LOG_WARNING(false, "Texture '%s' not found", texturePath);
			}

			subMeshInfo->m_MaterialInfo = materialInfo;
		}
		pFile->Close();
		sdelete(pFile);
	}
}
