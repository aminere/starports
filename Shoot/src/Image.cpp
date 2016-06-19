/* 

Amine Rehioui
Created: March 21th 2010

*/

#include "Shoot.h"

#include "Image.h"

#include "GeometryProvider.h"

namespace shoot
{
	DEFINE_OBJECT(Image);

	//! Constructor
	Image::Image()
	{
	}	

	//! serializes the entity to/from a PropertyStream
	void Image::Serialize(PropertyStream& stream)
	{
		super::Serialize(stream);

		if(IsInitialized())		
			UpdateTextureTransform();		
	}

	//! called during the initialization of the entity
	void Image::Init()
	{
		super::Init();

		UpdateGeometry();
	}

	//! updates the geometry
	void Image::UpdateGeometry()
	{
		if(auto gc = GetComponent<GraphicComponent>())
		{
			auto texture = gc->GetMaterial() ? gc->GetMaterial()->GetTexture() : NULL;
			if (texture)
			{
				m_BoundingBox.Set(Vector2::Create(0.0f, 0.0f), texture->GetSize());
				Vector2 vUV = Vector2::Create(texture->GetSize().X / texture->GetHardwareSize().X, texture->GetSize().Y / texture->GetHardwareSize().Y);
				gc->SetVertexBuffer(GeometryProvider::Instance()->Get2DQuad(m_BoundingBox.Size(), vUV));
			}
			else
			{
				gc->SetVertexBuffer(GeometryProvider::Instance()->Get2DQuad(m_BoundingBox.Size(), Vector2::One));
			}

			if (!gc->GetMaterial()->GetShader())
				gc->GetMaterial()->SetShader(Material::m_strDefaultShader);
		}

		UpdateTextureTransform();
	}

	//! Updates the texture transform
	void Image::UpdateTextureTransform()
	{
		if(GraphicComponent* pGraphic = GetComponent<GraphicComponent>())
		{
			Matrix44 textureTransform = Matrix44::Identity;
			textureTransform.SetScale(Vector3::Create(m_bHFlip ? -1.0f : 1.0f, m_bVFlip ? -1.0f : 1.0f, 1.0f));
			pGraphic->SetTextureTransform(textureTransform);
		}
	}
}

