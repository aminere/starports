/* 

Amine Rehioui
Created: February 5th 2012

*/

#include "Game.h"

#include "PulseManager.h"

#include "MaterialProvider.h"

#include "GraphicExtensionHandler.h"

#include "Quaternion.h"

namespace shoot
{
	DEFINE_OBJECT(PulseManager);

	//! Constructor
	PulseManager::PulseManager()
	{
	}

	//! destructor
	PulseManager::~PulseManager()
	{
	}

	//! serializes the entity to/from a PropertyStream
	void PulseManager::Serialize(PropertyStream& stream)
	{
		super::Serialize(stream);
	}

	//! called during the initialization of the entity
	void PulseManager::Init()
	{
		super::Init();
		
		GraphicComponent* pGraphic = GetComponent<GraphicComponent>();

		if(!pGraphic->GetMaterial())
		{
			Material* pMaterial = snew Material();
			pMaterial->SetShader(Material::m_strDefaultShader);
			pGraphic->SetMaterial(pMaterial);
		}

		int maxBullets = m_MaxBullets;
		int maxVertices = maxBullets*6;
		VertexBuffer* pVB = GraphicsDriver::Instance()->CreateVertexBuffer();		
		pVB->SetName(GetClassName());
		pVB->SetPrimitiveType(GraphicsDriver::PT_Triangle);
		pVB->SetVertices(snew Vertex3D[maxVertices], maxVertices, Vertex3D::VF_Pos|Vertex3D::VF_UV);		
		pVB->SetNumVertices(0);

		pGraphic->SetVertexBuffer(pVB);

		pVB->SetDynamic(true);
	}	

	//! Prepares a bullet for rendering
	void PulseManager::SetupRendering(Bullet* pBullet, int index, VertexBuffer* pVB)
	{
		const Matrix44& viewInverse = EntityRenderer::Instance()->Get3DCamera()->GetViewMatrixInverse();
		const float fHalfSize = pBullet->fRadius;
		const int vtx = index*6;
		Vertex3D* pVertices = reinterpret_cast<Vertex3D*>(pVB->GetVertices());
		pVertices[vtx+0].UV = Vector2::Create(0.0f, 1.0f); pVertices[vtx+0].Pos = Vector3::Create(-fHalfSize, -fHalfSize, 0.0f);
		pVertices[vtx+1].UV = Vector2::Create(0.0f, 0.0f); pVertices[vtx+1].Pos = Vector3::Create(-fHalfSize, fHalfSize, 0.0f);
		pVertices[vtx+2].UV = Vector2::Create(1.0f, 0.0f); pVertices[vtx+2].Pos = Vector3::Create(fHalfSize, fHalfSize, 0.0f);
		pVertices[vtx+3].UV = Vector2::Create(0.0f, 1.0f); pVertices[vtx+3].Pos = Vector3::Create(-fHalfSize, -fHalfSize, 0.0f);
		pVertices[vtx+4].UV = Vector2::Create(1.0f, 0.0f); pVertices[vtx+4].Pos = Vector3::Create(fHalfSize, fHalfSize, 0.0f);
		pVertices[vtx+5].UV = Vector2::Create(1.0f, 1.0f); pVertices[vtx+5].Pos = Vector3::Create(fHalfSize, -fHalfSize, 0.0f);
		Matrix44 transform = viewInverse;
		transform.SetTranslation(pBullet->vPosition);
		for(int i=0; i<6; ++i)
		{
			pVertices[vtx+i].Pos = transform.TransformVect(pVertices[vtx+i].Pos);
		}
		pVB->SetNumVertices(vtx+6);
	}
}

