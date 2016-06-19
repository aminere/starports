/* 

Amine Rehioui
Created: May 31th 2010

*/

#include "Shoot.h"

#include "BillBoardEntity.h"

#include "GeometryProvider.h"

namespace shoot
{
	DEFINE_OBJECT(BillBoardEntity);

	//! Constructor
	BillBoardEntity::BillBoardEntity()
		// properties
		: m_bHasVertexColors(false)
	{
		m_vPreviousScale = Vector3::One;
	}

	//! serializes the entity to/from a PropertyStream
	void BillBoardEntity::Serialize(PropertyStream& stream)
	{
		super::Serialize(stream);

		stream.Serialize("HasVertexColors", &m_bHasVertexColors);

		if(IsInitialized())
		{
			Vector3 vScale = GetAbsoluteScale();
			GetComponent<GraphicComponent>()->SetVertexBuffer(GeometryProvider::Instance()->Create3DQuad(Vector2::Create(vScale.X, vScale.Y), m_bHasVertexColors));
			m_vPreviousScale = m_vScale;
		}
	}

	//! called during the initialization of the entity
	void BillBoardEntity::Init()
	{
		Vector3 vScale = GetAbsoluteScale();
		GetComponent<GraphicComponent>()->SetVertexBuffer(GeometryProvider::Instance()->Create3DQuad(Vector2::Create(vScale.X, vScale.Y), m_bHasVertexColors));

		super::Init();
	}	

	//! called during the update of the entity		
	void BillBoardEntity::Update()
	{
		if(m_vPreviousScale != m_vScale)
		{
			Vector3 vScale = GetAbsoluteScale();
			Resize(vScale);
			m_vPreviousScale = m_vScale;
		}
	}

	//! returns the affine transformation matrix
	Matrix44 BillBoardEntity::GetWorldTransform()
	{
		if(Camera* pCamera = EntityRenderer::Instance()->Get3DCamera())
		{
			Matrix44 orientMatrix = pCamera->GetViewMatrixInverse();
			orientMatrix.SetTranslation(super::GetWorldTransform().GetTranslation());
			return orientMatrix;
		}
		return super::GetWorldTransform();
	}

	//! Resize
	void BillBoardEntity::Resize(const Vector3& newScale)
	{
		auto vb = GetComponent<GraphicComponent>()->GetVertexBuffer();
		auto vertices = reinterpret_cast<Vertex3D*>(vb->GetVertices());
		vertices[0].Pos = Vector3::Create(-newScale.X, newScale.Y, 0.0f);
		vertices[1].Pos = Vector3::Create(newScale.X, newScale.Y, 0.0f);
		vertices[2].Pos = Vector3::Create(-newScale.X, -newScale.Y, 0.0f);
		vertices[3].Pos = Vector3::Create(newScale.X, -newScale.Y, 0.0f);
		vb->SetDirty(true);
	}
}

