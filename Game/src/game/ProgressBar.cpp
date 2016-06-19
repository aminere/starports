/* 

Amine Rehioui
Created: January 11th 2014

*/

#include "Game.h"

#include "ProgressBar.h"

#include "MaterialProvider.h"
#include "GeometryProvider.h"

namespace shoot
{
	DEFINE_OBJECT(ProgressBar);

	//! Constructor
	ProgressBar::ProgressBar()
		: m_Progress(1.0f)
		, m_Dirty(false)
		// properties
		, m_ColorFull(Color::Green)
		, m_ColorEmpty(Color::Red)
	{
	}

	//! serializes the entity to/from a PropertyStream
	void ProgressBar::Serialize(PropertyStream& stream)
	{
		super::Serialize(stream);

		stream.Serialize("ColorFull", &m_ColorFull);
		stream.Serialize("ColorEmpty", &m_ColorEmpty);
	}

	//! called during the initialization of the entity
	void ProgressBar::Init()
	{
		super::Init();

		GraphicComponent* pGraphic = GetComponent<GraphicComponent>();
		if(!pGraphic)
		{
			Material::CreationInfo materialInfo;
			materialInfo.AddTexture("common/tex/WhiteBG.xml");
			Material* pMaterial = MaterialProvider::Instance()->GetMaterial(materialInfo);
			pGraphic = snew GraphicComponent();
			pGraphic->SetMaterial(pMaterial);
			AddComponent(pGraphic, true);
		}
				
		Vertex3D* pVertices = snew Vertex3D[6];
		Vector3 vScale = GetAbsoluteScale();
		pVertices[0].UV = Vector2::Create(0.0f, 1.0f); pVertices[0].Pos = Vector3::Create(-vScale.X, -vScale.Y, 0.0f);
		pVertices[1].UV = Vector2::Create(0.0f, 0.0f); pVertices[1].Pos = Vector3::Create(-vScale.X, vScale.Y, 0.0f);
		pVertices[2].UV = Vector2::Create(1.0f, 0.0f); pVertices[2].Pos = Vector3::Create(vScale.X, vScale.Y, 0.0f);
		pVertices[3].UV = Vector2::Create(0.0f, 1.0f); pVertices[3].Pos = Vector3::Create(-vScale.X, -vScale.Y, 0.0f);
		pVertices[4].UV = Vector2::Create(1.0f, 0.0f); pVertices[4].Pos = Vector3::Create(vScale.X, vScale.Y, 0.0f);
		pVertices[5].UV = Vector2::Create(1.0f, 1.0f); pVertices[5].Pos = Vector3::Create(vScale.X, -vScale.Y, 0.0f);
		for(int i=0; i<6; ++i)
		{
			pVertices[i].color = m_ColorFull;
		}
		VertexBuffer* pVB = GraphicsDriver::Instance()->CreateVertexBuffer();
		pVB->SetVertices(pVertices, 6, Vertex3D::VF_Pos|Vertex3D::VF_UV|Vertex3D::VF_Color);
		pVB->SetDynamic(true);
		pGraphic->SetVertexBuffer(pVB);
	}	

	//! called during the update of the entity
	void ProgressBar::Update()
	{
		if (!m_Dirty)
			return;

		GraphicComponent* pGraphic = GetComponent<GraphicComponent>();
		VertexBuffer* pVB = pGraphic->GetVertexBuffer();
		Vertex3D* pVertices = reinterpret_cast<Vertex3D*>(pVB->GetVertices());
		float fFactor = Math::Clamp(m_Progress, 0.0f, 1.0f);
		Vector3 vScale = GetAbsoluteScale();
		float fSize = -vScale.X + vScale.X*2.0f*fFactor;
		pVertices[2].Pos.X = fSize;
		pVertices[4].Pos.X = fSize;
		pVertices[5].Pos.X = fSize;
		Color color = Math::Lerp(m_ColorEmpty, m_ColorFull, fFactor);
		for(uint i=0; i<pVB->GetNumVertices(); ++i)			
			pVertices[i].color = color;			
		pVB->SetDirty(true);
		m_Dirty = false;
	}

	//! returns the affine transformation matrix
	Matrix44 ProgressBar::GetWorldTransform()
	{
		auto camera = EntityRenderer::Instance()->Get3DCamera();
		if(!camera)
			return super::GetWorldTransform();

		auto orientMatrix = camera->GetViewMatrixInverse();
		orientMatrix.SetTranslation(super::GetWorldTransform().GetTranslation());
		return orientMatrix;
	}
}

