/* 

Amine Rehioui
Created:  July 17th 2013

*/

#include "Shoot.h"

#include "FBXMeshLoader.h"

#include "Utils.h"

#include "File.h"
#include <functional>

#include "fbxsdk.h"

namespace shoot
{
	// statics
	fbxsdk_2015_1::FbxManager* FBXMeshLoader::ms_pManager = NULL;

	//! loads a mesh from a file
	void FBXMeshLoader::Load(const std::string& path, File* binFile, bool skipTextureLoad /*= false*/)
	{
		m_Path = path;
		m_SkipTextureLoad = skipTextureLoad;

		if(!ms_pManager)
		{
			ms_pManager = FbxManager::Create();
			FbxIOSettings* pIOs = FbxIOSettings::Create(ms_pManager, IOSROOT);
			ms_pManager->SetIOSettings(pIOs);
		}
		
		FbxScene* pScene = FbxScene::Create(ms_pManager, "Scene");
		int lFileMajor, lFileMinor, lFileRevision;
		int lSDKMajor,  lSDKMinor,  lSDKRevision;		
		FbxManager::GetFileFormatVersion(lSDKMajor, lSDKMinor, lSDKRevision);

		FbxImporter* lImporter = FbxImporter::Create(ms_pManager, "");
		std::string _path = std::string("data/") + path;
		bool lImportStatus = lImporter->Initialize(_path.c_str(), -1, ms_pManager->GetIOSettings());
		lImporter->GetFileVersion(lFileMajor, lFileMinor, lFileRevision);
		if( !lImportStatus )
		{
			FbxString error = lImporter->GetStatus().GetErrorString();
			SHOOT_ASSERT(false, "FBX Error: %s", error.Buffer());
			if (lImporter->GetStatus().GetCode() == FbxStatus::eInvalidFileVersion)
			{
				SHOOT_ASSERT(false, "FBX file format version for this FBX SDK is %d.%d.%d\n", lSDKMajor, lSDKMinor, lSDKRevision);
				SHOOT_ASSERT(false, "FBX file format version for file '%s' is %d.%d.%d\n\n", path.c_str(), lFileMajor, lFileMinor, lFileRevision);
			}
			return;
		}
		bool lStatus = lImporter->Import(pScene);
		SHOOT_ASSERT(lStatus, "FBX import error");		
		lImporter->Destroy();

        LoadNode(pScene->GetRootNode());		
				
		// TODO OPTIM: merge vertices that are alike.
		//std::function<void(void*, uint, uint)> setIndex16 = [](void* pIndices, uint idx, uint value) { static_cast<ushort*>(pIndices)[idx] = value; };
		//std::function<void(void*, uint, uint)> setIndex32 = [](void* pIndices, uint idx, uint value) { static_cast<uint*>(pIndices)[idx] = value; };
		//auto setIndex = m_pMesh->Has32BitIndices() ? setIndex32 : setIndex16;
		if (m_pMesh->HasMergeSubmeshes())
		{
			for (auto it = m_Groups.begin(); it != m_Groups.end(); ++it)
			{
				auto& materialInfo = (*it).m_MaterialInfo;
				auto& subMeshes = (*it).m_SubMeshes;
				auto vb = GraphicsDriver::Instance()->CreateVertexBuffer(m_pMesh->HasInstancedRendering());
				auto name = subMeshes[0].m_Name + "_Merged";
				auto vertexFormat = subMeshes[0].m_VertexFormat;
				std::vector<Vertex3D> vertices;
				auto numVertices = 0;
				for (auto& subMesh : subMeshes)
					numVertices += subMesh.m_NumVertices;
				vertices.reserve(numVertices);

				for (auto& subMesh : subMeshes)
				{
					SHOOT_ASSERT(subMesh.m_VertexFormat == vertexFormat, "FBX SubMeshes must have the same vertex format");
					for (uint i = 0; i < subMesh.m_NumVertices; ++i)
					{
						auto vertex = subMesh.m_Vertices[i];
						vertex.Pos += subMesh.m_Position;
						vertices.push_back(vertex);
					}
					sdelete_array(subMesh.m_Vertices);
				}

				vb->SetName(name);

				auto pVertices = snew Vertex3D[numVertices];
				memcpy(pVertices, vertices.data(), numVertices*sizeof(Vertex3D));
				vb->SetVertices(pVertices, numVertices, vertexFormat);

				//auto numIndices = numVertices;
				//void* pIndices = m_pMesh->Has32BitIndices() ? ((void*)snew uint[numIndices]) : ((void*)snew ushort[numIndices]);
				//for (uint i = 0; i < numIndices; ++i)
				//	setIndex(pIndices, i, i);
				//vb->SetIndices(pIndices, numIndices, m_pMesh->Has32BitIndices());

				m_pMesh->AddSubMeshInfo(vb, Vector3::Zero, materialInfo);
			}
		}
		else
		{
			for (auto it = m_Groups.begin(); it != m_Groups.end(); ++it)
			{
				auto& materialInfo = (*it).m_MaterialInfo;
				auto& subMeshes = (*it).m_SubMeshes;
				auto& meshInfos = m_pMesh->GetSubMeshInfos();
				for (auto& subMesh : subMeshes)
				{
					VertexBuffer* vb = NULL;
					auto infoIt = std::find_if(meshInfos.begin(), meshInfos.end(), [&](const MeshResource::SubMeshInfo& info){ return info.m_VertexBuffer->GetName() == subMesh.m_Name; });
					auto existingVB = (infoIt != meshInfos.end());
					if (existingVB)
					{
						vb = (*infoIt).m_VertexBuffer.Get();
						(*infoIt).m_vPosition = subMesh.m_Position;
						(*infoIt).m_MaterialInfo = materialInfo;
					}
					else
					{
						vb = GraphicsDriver::Instance()->CreateVertexBuffer(m_pMesh->HasInstancedRendering());
						vb->SetName(subMesh.m_Name);
						m_pMesh->AddSubMeshInfo(vb, subMesh.m_Position, materialInfo);
					}
					
					vb->SetVertices(subMesh.m_Vertices, subMesh.m_NumVertices, subMesh.m_VertexFormat);

					//auto numIndices = subMesh.m_NumVertices;
					//void* pIndices = m_pMesh->Has32BitIndices() ? ((void*)snew uint[numIndices]) : ((void*)snew ushort[numIndices]);
					//for (uint i = 0; i < numIndices; ++i)
					//	setIndex(pIndices, i, i);
					//vb->SetIndices(pIndices, numIndices, m_pMesh->Has32BitIndices());
				}
			}
		}

		pScene->Destroy();

		// write bin file
		const auto& subMeshInfos = m_pMesh->GetSubMeshInfos();
		uint numSubMeshes = (uint)subMeshInfos.size();
		binFile->WriteData(&numSubMeshes, 1);		
		for (auto& subMeshInfo : subMeshInfos)
		{
			VertexBuffer* pVB = subMeshInfo.m_VertexBuffer;
			const Material::CreationInfo& material = subMeshInfo.m_MaterialInfo;
			uint numVertices = pVB->GetNumVertices();
			uint numIndices = pVB->GetNumIndices();
			uint flags = pVB->GetFormat();
			uint nameLength = (uint)pVB->GetName().length();
			uint numTextures = (uint)material.m_Textures.GetSize();

			binFile->WriteData(&subMeshInfo.m_vPosition, 1);
			binFile->WriteData(&numVertices, 1);
			//binFile->WriteData(&numIndices, 1);
			binFile->WriteData(&flags, 1);
			binFile->WriteData(&nameLength, 1);
			binFile->WriteData(pVB->GetName().c_str(), nameLength);
			//if (m_pMesh->Has32BitIndices())
			//	binFile->WriteData<uint>((uint*)pVB->GetIndices(), numIndices);
			//else
			//	binFile->WriteData<ushort>((ushort*)pVB->GetIndices(), numIndices);
			binFile->WriteData(pVB->GetVertices(), numVertices*sizeof(Vertex3D));
			binFile->WriteData(&material.m_Color, 1);
			binFile->WriteData(&numTextures, 1);

			for (uint i = 0; i<numTextures; ++i)
			{
				const auto& path = material.m_Textures[i]->GetTemplatePath();
				uint texturePathLength = (uint)path.length();
				binFile->WriteData(&texturePathLength, 1);
				binFile->WriteData(path.c_str(), texturePathLength);
			}
		}
	}

	//! FBX Loading
	void FBXMeshLoader::LoadNode(FbxNode* node)
	{		
		if(auto attribute = node->GetNodeAttribute())
		{
			if(attribute->GetAttributeType() == FbxNodeAttribute::eMesh)
				LoadMesh(node);
		}

		for (int i=0; i<node->GetChildCount(); ++i)
			LoadNode(node->GetChild(i));
	}
	
	void FBXMeshLoader::LoadMesh(FbxNode* meshNode)
	{
		auto mesh = meshNode->GetMesh();
		if(!mesh->IsTriangleMesh())
		{
			FbxGeometryConverter converter(ms_pManager);
			converter.Triangulate(mesh, false);
			mesh = meshNode->GetMesh();
			if(!mesh->IsTriangleMesh())
			{
				SHOOT_ASSERT(false, "mesh '%s': FBX triangulation failed, please triangulate in Max/Maya", meshNode->GetName());
				return;
			}
		}

		FbxSubMesh subMesh;
		auto materialInfo = LoadMaterial(meshNode);
		subMesh.m_Position = Vector3::Create(float(meshNode->LclTranslation.Get()[0]), -float(meshNode->LclTranslation.Get()[2]), float(meshNode->LclTranslation.Get()[1]));		

		uint vertexFormat = Vertex3D::VF_Pos;
		int numVertices = mesh->GetPolygonCount() * 3;
		Vertex3D* pVertices = snew Vertex3D[numVertices];

		auto orientable = (std::string(meshNode->GetName()).find("orient") != std::string::npos);
		LoadVertices(mesh, pVertices, numVertices, orientable);
		LoadUVs(mesh, pVertices, numVertices, vertexFormat);
		LoadVertexColors(mesh, pVertices, numVertices, vertexFormat);
		LoadVertexNormals(mesh, pVertices, numVertices, vertexFormat, orientable);
		LoadTangentBinormals(mesh, pVertices, numVertices, vertexFormat);

		// TODO multiple material support
		//auto materialElem = mesh->GetElementMaterial();
		//auto mode = materialElem->GetMappingMode();
		//fbxsdk_2015_1::FbxLayerElementArrayTemplate<int>* arr = NULL; 
		//mesh->GetMaterialIndices(&arr);

		subMesh.m_Name = meshNode->GetName();
		subMesh.m_Vertices = pVertices;
		subMesh.m_NumVertices = numVertices;
		subMesh.m_VertexFormat = vertexFormat;
		auto it = std::find_if(m_Groups.begin(), m_Groups.end(), [&](const Group& g) { return g.m_MaterialInfo == materialInfo; });
		if (it != m_Groups.end())
			(*it).m_SubMeshes.push_back(subMesh);
		else
		{
			Group group;
			group.m_MaterialInfo = materialInfo;
			group.m_SubMeshes.push_back(subMesh);
			m_Groups.push_back(group);
		}
	}

	//! LoadIndices
	void FBXMeshLoader::LoadVertices(FbxMesh* mesh, Vertex3D* pVertices, int numVertices, bool orientable)
	{
		Matrix44 baseRotation;
		baseRotation.SetRotation(Vector3::Create(Math::PIBy2, 0.0f, 0.0f));
		int currentVertex = 0;
		for(int i=0; i<mesh->GetPolygonCount(); ++i)
		{
			int numPolygonVtx = mesh->GetPolygonSize(i);
			SHOOT_ASSERT(numPolygonVtx == 3, "mesh '%s': Found non-triangular polygon in a supposedly triangulated mesh", mesh->GetName());
			int idx[3] = { 0, 2, 1 }; // shoot needs clock-wise triangles
			for (int j=0; j<numPolygonVtx; ++j)
			{
				int index = mesh->GetPolygonVertex(i, idx[j]);
				FbxVector4 vtx = mesh->GetControlPoints()[index];
				pVertices[currentVertex].Pos = Vector3::Create(float(vtx[0]), float(vtx[1]), float(vtx[2]));

				if (orientable)
				{
					// Apply base rotation to make this mesh orientable using a LookAt Matrix
					pVertices[currentVertex].Pos = baseRotation.TransformVect(pVertices[currentVertex].Pos);
				}

				currentVertex++;
			}
		}
	}

	//! LoadUVs
	void FBXMeshLoader::LoadUVs(FbxMesh* mesh, Vertex3D* pVertices, int numVertices, uint& vertexFormat)
	{
		auto uvElement = mesh->GetElementUV();
		if(!uvElement)
			return;

		if(uvElement->GetMappingMode() != FbxGeometryElement::eByPolygonVertex)
		{
			int result = uvElement->RemapIndexTo(FbxGeometryElement::eByPolygonVertex);
			SHOOT_ASSERT(result == 1, "mesh '%s': UVElement should have PolygonVertex mapping (and failed to change it). Current mapping: %d", mesh->GetName(), uvElement->GetMappingMode());
		}

		bool bIndexed = uvElement->GetReferenceMode() != FbxGeometryElement::eDirect;
		int count = bIndexed ? uvElement->GetIndexArray().GetCount() : uvElement->GetDirectArray().GetCount();
		int dir[3] = { 0, 1, -1 }; // hack because vertices were shuffled to make clock-wise triangles (see LoadIndices)
		for(int i=0; i<count && i<int(numVertices); ++i)
		{
			int localIdx = i%3;
			int idx = i + dir[localIdx];
			int uvIndex = bIndexed ? uvElement->GetIndexArray().GetAt(idx) : idx;
			auto uv = uvElement->GetDirectArray().GetAt(uvIndex);
			pVertices[i].UV = Vector2::Create(float(uv[0]), 1.0f-float(uv[1]));
		}
		vertexFormat |= Vertex3D::VF_UV;
	}

	//! LoadVertexColors
	void FBXMeshLoader::LoadVertexColors(FbxMesh* mesh, Vertex3D* pVertices, int numVertices, uint& vertexFormat)
	{
		if(!m_pMesh->HasVertexColors())
			return;

		auto vertexColor = mesh->GetElementVertexColor();
		if(vertexColor)
		{
			if(vertexColor->GetMappingMode() != FbxGeometryElement::eByPolygonVertex)
			{
				int result = vertexColor->RemapIndexTo(FbxGeometryElement::eByPolygonVertex);
				SHOOT_ASSERT(result == 1, "mesh '%s': VertexColor should have PolygonVertex mapping (and failed to change it). Current mapping: %d", mesh->GetName(), vertexColor->GetMappingMode());
			}

			bool bIndexed = vertexColor->GetReferenceMode() != FbxGeometryElement::eDirect;
			int count = bIndexed ? vertexColor->GetIndexArray().GetCount() : vertexColor->GetDirectArray().GetCount();
			int dir[3] = { 0, 1, -1 }; // hack because vertices were shuffled to make clock-wise triangles (see above)
			for(int i=0; i<count && i<int(numVertices); ++i)
			{
				int localIdx = i%3;
				int idx = i + dir[localIdx];
				int colorIndex = bIndexed ? vertexColor->GetIndexArray().GetAt(idx) : idx;
				FbxColor fbxColor = vertexColor->GetDirectArray().GetAt(colorIndex);
				pVertices[i].color = Color::Create(float(fbxColor[0]), float(fbxColor[1]), float(fbxColor[2]), float(fbxColor[3]));

				// hack to support vertex alpha => pink vertices shall be considered transparent.
				if(pVertices[i].color == Color::Pink)
					pVertices[i].color.A = 0.0f;
			}
		}
		else
		{
			for(int i=0; i<int(numVertices); ++i)				
				pVertices[i].color = Color::White;				
		}
		vertexFormat |= Vertex3D::VF_Color;
	}

	//! LoadVertexNormals
	void FBXMeshLoader::LoadVertexNormals(FbxMesh* mesh, Vertex3D* pVertices, int numVertices, uint& vertexFormat, bool orientable)
	{
		if(!m_pMesh->HasVertexNormals())
			return;

		auto normals = mesh->GetElementNormal();
		if(!normals)
		{
			SHOOT_ASSERT(false, "TODO: should auto-compute vertex normals");
			return;
		}

		Matrix44 baseRotation;
		baseRotation.SetRotation(Vector3::Create(Math::PIBy2, 0.0f, 0.0f));
		if(normals->GetMappingMode() == FbxGeometryElement::eByControlPoint)
		{
			if(normals->GetReferenceMode() == FbxGeometryElement::eDirect)
			{
				int currentIndex = 0;
				for(int i=0; i<mesh->GetPolygonCount(); ++i)
				{
					int numPolygonVtx = mesh->GetPolygonSize(i);
					SHOOT_ASSERT(numPolygonVtx == 3, "mesh '%s': Found non-triangular polygon in a supposedly triangulated mesh", mesh->GetName());
					int idx[3] = { 0, 2, 1 }; // shoot needs clock-wise triangles
					for (int j=0; j<numPolygonVtx; ++j)
					{
						int index = mesh->GetPolygonVertex(i, idx[j]);											
						FbxVector4 lNormal = normals->GetDirectArray().GetAt(index);	
						pVertices[currentIndex].Normal = Vector3::Create(float(lNormal[0]), float(lNormal[1]), float(lNormal[2]));

						if (orientable)							
							pVertices[currentIndex].Normal = baseRotation.TransformVect(pVertices[currentIndex].Normal);

						currentIndex++;
					}
				}
			}
			else if (normals->GetReferenceMode() == FbxGeometryElement::eIndexToDirect)
			{
				SHOOT_ASSERT(false, "FBX normal format not supported yet");
			}
		}
		else if(normals->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
		{
			bool bIndexed = normals->GetReferenceMode() != FbxGeometryElement::eDirect;
			int count = bIndexed ? normals->GetIndexArray().GetCount() : normals->GetDirectArray().GetCount();
			int dir[3] = { 0, 1, -1 }; // hack because vertices were shuffled to make clock-wise triangles (see above)
			for(int i=0; i<count && i<int(numVertices); ++i)
			{
				int localIdx = i%3;
				int idx = i + dir[localIdx];
				int normalIndex = bIndexed ? normals->GetIndexArray().GetAt(idx) : idx;
				FbxVector4 lNormal = normals->GetDirectArray().GetAt(normalIndex);
				pVertices[i].Normal = Vector3::Create(float(lNormal[0]), float(lNormal[1]), float(lNormal[2]));

				if (orientable)
					pVertices[i].Normal = baseRotation.TransformVect(pVertices[i].Normal);
			}
		}

		vertexFormat |= Vertex3D::VF_Normal;
	}

	//! LoadTangentBinormals
	void FBXMeshLoader::LoadTangentBinormals(FbxMesh* mesh, Vertex3D* pVertices, int numVertices, uint& vertexFormat)
	{
		if(!m_pMesh->HasTangentBinormals())
			return;

		Vector3 *tan1 = snew Vector3[numVertices*2];
		Vector3 *tan2 = tan1 + numVertices;
		memset(tan1, 0, numVertices*2*sizeof(Vector3));

		for (int i = 0; i<numVertices; i += 3)
		{
			auto i1 = i + 0;
			auto i2 = i + 1;
			auto i3 = i + 2;

			const Vector3& v1 = pVertices[i1].Pos;
			const Vector3& v2 = pVertices[i2].Pos;
			const Vector3& v3 = pVertices[i3].Pos;

			const Vector2& w1 = pVertices[i1].UV;
			const Vector2& w2 = pVertices[i2].UV;
			const Vector2& w3 = pVertices[i3].UV;

			float x1 = v2.X - v1.X;
			float x2 = v3.X - v1.X;
			float y1 = v2.Y - v1.Y;
			float y2 = v3.Y - v1.Y;
			float z1 = v2.Z - v1.Z;
			float z2 = v3.Z - v1.Z;

			float s1 = w2.X - w1.X;
			float s2 = w3.X - w1.X;
			float t1 = w2.Y - w1.Y;
			float t2 = w3.Y - w1.Y;

			float r = 1.0f / (s1 * t2 - s2 * t1);
			Vector3 sdir = Vector3::Create((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r, (t2 * z1 - t1 * z2) * r);
			Vector3 tdir = Vector3::Create((s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r, (s1 * z2 - s2 * z1) * r);

			tan1[i1] += sdir;
			tan1[i2] += sdir;
			tan1[i3] += sdir;

			tan2[i1] += tdir;
			tan2[i2] += tdir;
			tan2[i3] += tdir;
		}

		for(int i=0; i<numVertices; ++i)
		{
			const Vector3& n = pVertices[i].Normal;
			const Vector3& t = tan1[i];

			// Gram-Schmidt orthogonalize
			pVertices[i].Tangent.XYZ = (t - n * n.DotProduct(t)).Normalize();

			// Calculate handedness
			pVertices[i].Tangent.W = (n.CrossProduct(t).DotProduct(tan2[i]) < 0.0f) ? -1.0f : 1.0f;
		}

		delete[] tan1;
		vertexFormat |= Vertex3D::VF_TangentBinormal;
	}

	//! LoadMaterial
	Material::CreationInfo FBXMeshLoader::LoadMaterial(FbxNode* node)
	{
		Material::CreationInfo materialInfo;
		if(auto material = static_cast<FbxSurfaceMaterial*>(node->GetSrcObject(FbxCriteria::ObjectType(FbxSurfaceMaterial::ClassId))))
		{
			FbxPropertyT<FbxColor> diffuse = material->FindProperty(FbxSurfaceMaterial::sDiffuse);
			if(diffuse.IsValid())
			{
				FbxColor fbxColor = diffuse.Get();
				materialInfo.m_Color = Color::Create(float(fbxColor[0]), float(fbxColor[1]), float(fbxColor[2]), float(fbxColor[3]));
			}

			int diffuseIndex = FbxLayerElement::eTextureDiffuse - FbxLayerElement::sTypeTextureStartIndex;
			if(FbxFileTexture* pTexture = material->FindProperty(FbxLayerElement::sTextureChannelNames[diffuseIndex]).GetSrcObject<FbxFileTexture>(0))
			{
				std::string texturePath = pTexture->GetRelativeFileName();
				if(texturePath.length() > 0)
				{
					// determine texture path
					std::string meshPath = m_Path;
					size_t dirIndex = meshPath.rfind("/");
					std::string basePath = meshPath.substr(0, dirIndex+1);
					texturePath = Utils::FormatResourcePath(basePath + texturePath);

					auto xmlPath = Utils::GetFilePathNoExt(texturePath) + ".xml";
					if (File::Exists(xmlPath.c_str()))
					{
						if (m_SkipTextureLoad)
						{
							// add empty texture
							auto texture = snew Texture();
							ObjectManager::Instance()->SetTemplatePath(texture, xmlPath);
							materialInfo.AddTexture(texture);
						}
						else
						{
							materialInfo.AddTexture(xmlPath);
						}
					}
					else
					{
						if (m_pMesh->HasMergeSubmeshes())
						{
							// still need to add a texture so meshes with a different texture don't get merged together
							auto it = std::find_if(m_DummyTextures.begin(), m_DummyTextures.end(), [&](const Reference<Texture>& tex) { return tex->GetTemplatePath() == xmlPath; });
							if (it != m_DummyTextures.end())
							{
								auto texture = (*it).Get();
								materialInfo.AddTexture(Reference<Texture>(texture));
							}
							else
							{
								// add empty texture
								auto texture = snew Texture();
								ObjectManager::Instance()->SetTemplatePath(texture, xmlPath);
								materialInfo.AddTexture(texture);
								m_DummyTextures.push_back(Reference<Texture>(texture));
							}							
						}
					}
				}
			}
		}		
		return materialInfo;
	}

	//! static uninitialization
	void FBXMeshLoader::Uninit()
	{
		if (!ms_pManager)
			return;
		
		ms_pManager->Destroy();
		ms_pManager = NULL;		
	}
}


