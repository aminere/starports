/* 

Amine Rehioui
Created: May 6th 2012

*/

#include "Shoot.h"

#include "ExplosionVisitor.h"

#include "MeshEntity.h"

namespace shoot
{
	DEFINE_OBJECT(ExplosionVisitor);

	//! constructor
	ExplosionVisitor::ExplosionVisitor()
		: m_fDefaultIntensity(10.0f)
		, m_fDefaultDuration(1.3f)
		// properties
		, m_fIntensity(-1.0f)
		, m_fDuration(-1.0f)
		, m_bDepthTest(false)
	{
	}

	//! destructor
	ExplosionVisitor::~ExplosionVisitor()
	{
		for (auto& vertices : m_OriginalVertices)
			sdelete_array(vertices);

		for (auto& indices : m_IndexMaps)
			sdelete_array(indices);
	}

	//! Reads/Writes struct properties from/to a stream
	void ExplosionVisitor::Serialize(PropertyStream& stream)
	{
		stream.Serialize("Intensity", &m_fIntensity);
		stream.Serialize("Duration", &m_fDuration);
		stream.Serialize("DepthTest", &m_bDepthTest);
		stream.Serialize("Mesh", &m_Mesh);
		stream.Serialize("ChunkGroups", &m_aChunkGroups);		
	}

	//! visits a particular entity
	void ExplosionVisitor::Visit(Entity* pTarget)
	{
		SHOOT_ASSERT(pTarget->IsA(MeshEntity::TypeID), "ExplosionVisitor target is not of type MeshEntity");
		MeshEntity* pMesh = static_cast<MeshEntity*>(pTarget);
		if(m_Mesh.IsValid())
		{
			SHOOT_ASSERT(m_Mesh.Get() == pMesh->GetMesh(), "ExplosionVisitor not meant to be applied on this mesh");
		}
		else
		{
			m_Mesh = pMesh->GetMesh();
		}

		SHOOT_ASSERT(m_OriginalVertices.empty(), "ExplosionVisitor triggered twice");
		SHOOT_ASSERT(m_IndexMaps.empty(), "ExplosionVisitor triggered twice");
		SHOOT_ASSERT(m_VertexBuffers.empty(), "ExplosionVisitor triggered twice");		

		std::vector<SubMesh*>& subMeshes = pMesh->GetSubMeshes();
		for (size_t i=0; i<subMeshes.size(); ++i)
		{
			GraphicComponent* pGraphic = subMeshes[i]->GetComponent<GraphicComponent>();
			VertexBuffer* pSourceVB = pGraphic->GetVertexBuffer();
			SHOOT_ASSERT(pSourceVB->GetDynamic(), "ExplosionVisitor needs a dynamic vertex buffer");
			SHOOT_ASSERT(pSourceVB->GetPrimitiveType() == GraphicsDriver::PT_Triangle, "ExplosionVisitor only supported triangulated meshes for now");

			// set-up vertex buffer based on chunks
			SHOOT_ASSERT(!pSourceVB->Has32bitIndices(), "Trying to explode a VertexBuffer with 32 bit indices");
			Vertex3D* pSourceVertices = reinterpret_cast<Vertex3D*>(pSourceVB->GetVertices());
			Vertex3D* pVertices = snew Vertex3D[pSourceVB->GetNumVertices()];

			// copy the indices defined by the chunks
			// also, calculate the position (center of transformation) for each chunk
			auto originalVertices = snew Vector3[pSourceVB->GetNumVertices()];
			auto indexMap = snew uint[pSourceVB->GetNumVertices()];
			int currentVertex = 0;
			Array<Chunk>& aChunks = m_aChunkGroups[i].m_aChunks;
			for (size_t j = 0; j<aChunks.GetSize(); ++j)
			{
				aChunks[j].Reset();

				for (size_t k = 0; k<aChunks[j].aTriangleIndices.GetSize(); ++k)
				{
					uint triangleIndex = aChunks[j].aTriangleIndices[k]*3;
					SHOOT_ASSERT(triangleIndex+2 < pSourceVB->GetNumVertices(), "ExplosionVisitor: vertex out of range");
					for(uint l=0; l<3; ++l)
					{
						auto sourceIndex = triangleIndex + l;						
						pVertices[currentVertex].Pos = pSourceVertices[sourceIndex].Pos;
						pVertices[currentVertex].UV = pSourceVertices[sourceIndex].UV;						
						pVertices[currentVertex].color = Color::White;
						originalVertices[currentVertex] = pVertices[currentVertex].Pos;

						aChunks[j].vCenter += pVertices[currentVertex].Pos;
						indexMap[sourceIndex] = currentVertex;
						++currentVertex;
					}
				}
				aChunks[j].vCenter = aChunks[j].vCenter / (float(aChunks[j].aTriangleIndices.GetSize())*3);
				aChunks[j].vDirection = aChunks[j].vCenter.Normalized();
				aChunks[j].vDirection.Z = Math::FAbs(aChunks[j].vDirection.Z);

				// randomize the direction a bit
				float fRandom = 1.0f;
				Vector3 vRandom = Vector3::Create(Random::GetFloat(-fRandom, fRandom), Random::GetFloat(-fRandom, fRandom), Random::GetFloat(0.0f, fRandom));
				aChunks[j].vDirection += vRandom;
			}
			
			Material* pMaterial = static_cast<Material*>(pGraphic->GetMaterial()->Copy());
			//if (pMaterial->GetShader()->GetType() != Shader::T_Lighting)
				pMaterial->SetShader("common/shaders/tex_col.xml");
			pMaterial->SetFlag(Material::MF_AlphaBlending, true);
			pMaterial->SetCullMode(GraphicsDriver::CM_None);
			pGraphic->SetMaterial(pMaterial);
			pGraphic->SetRenderingPriority(1);
			
			VertexBuffer* pVB = static_cast<VertexBuffer*>(pSourceVB->Copy());
			pVB->SetVertices(pVertices, pSourceVB->GetNumVertices(), pSourceVB->GetFormat() | Vertex3D::VF_Color);
			pVB->SetNumVertices(currentVertex);
			pGraphic->SetVertexBuffer(pVB);
			pGraphic->SetShadowCaster(false);

			m_VertexBuffers.push_back(Reference<VertexBuffer>(pVB));
			m_OriginalVertices.push_back(originalVertices);
			m_IndexMaps.push_back(indexMap);
		}

		float fDuration = (m_fDuration < 0.0f) ? m_fDefaultDuration : m_fDuration;
		m_Timer.Start(fDuration);

		super::Visit(pTarget);
	}

	//! updates the visitor
	void ExplosionVisitor::Update()
	{
		if(!m_bActive)		
			return;		

		float fIntensity = (m_fIntensity < 0.0f) ? m_fDefaultIntensity : m_fIntensity;
		for (size_t i=0; i<m_VertexBuffers.size(); ++i)
		{
			VertexBuffer* pVB = m_VertexBuffers[i];
			Vertex3D* pVertices = reinterpret_cast<Vertex3D*>(pVB->GetVertices());
			Vector3* pOriginalVertices = m_OriginalVertices[i];
			uint* indexMap = m_IndexMaps[i];

			Array<Chunk>& aChunks = m_aChunkGroups[i].m_aChunks;
			for (size_t j = 0; j<aChunks.GetSize(); ++j)
			{
				Vector3 vSpeed = (aChunks[j].vDirection*fIntensity*g_fDeltaTime);						
				aChunks[j].vPosition += vSpeed;
				aChunks[j].vRotation += vSpeed/3.0f;

				Matrix44 matrix = Matrix44::Identity;
				matrix.Translate(-aChunks[j].vCenter);
				matrix.Rotate(aChunks[j].vRotation);
				matrix.Translate(aChunks[j].vCenter + aChunks[j].vPosition);			

				for (size_t k = 0; k<aChunks[j].aTriangleIndices.GetSize(); ++k)
				{
					int triangleIndex = aChunks[j].aTriangleIndices[k]*3;
					for(int l=0; l<3; ++l)
					{
						auto index = indexMap[triangleIndex + l];
						Vertex3D& v = pVertices[index];
						Vector3 vOriginal = pOriginalVertices[index];
						v.Pos = matrix.TransformVect(vOriginal);
						v.color.A = (1.0f-(m_Timer.GetProgressRatio()*m_Timer.GetProgressRatio()));					
					}
				}
			}

			pVB->SetDirty(true);
		}

		if(!m_Timer.IsRunning())
		{
			m_bActive = false;
		}
	}

	//! Reads/Writes struct properties from/to a stream
	void ExplosionVisitor::Chunk::Serialize(PropertyStream& stream)
	{
		stream.Serialize("Triangles", &aTriangleIndices);		
	}
}

