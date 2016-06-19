/* 

Amine Rehioui
Created: February 23rd 2013

*/

#include "Shoot.h"

namespace shoot
{
	// type literals
	const char* const g_strRenderingPasses[] =
	{
		"SkyBox",
		"3DSolid",
		"3DParticles",
		"3DTransparent",
		"2DSolid",
		"2DTransparent",
		"2DPostFade",
		"None",
		"3DDepthOff",
		"2DLayout",
		"PostEffect",
		"2DLayoutPostTransparent",
		0
	};

	DEFINE_OBJECT(GraphicComponent);

	//! constructor
	GraphicComponent::GraphicComponent()
		// properties
		: m_RenderingPriority(0)
		, m_eRenderingPass(RP_None)
		, m_bShadowCaster(false)
	{
		SetTextureTransform(Matrix44::Identity);
	}

	//! destructor
	GraphicComponent::~GraphicComponent()
	{
	}

	//! serializes the entity to/from a PropertyStream
	void GraphicComponent::Serialize(PropertyStream& stream)
	{
		super::Serialize(stream);

		stream.Serialize("RenderingPriority", &m_RenderingPriority);
		stream.Serialize("RenderingPass", &m_eRenderingPass, ENUM_PARAMS1(g_strRenderingPasses));
		stream.Serialize("ShadowCaster", &m_bShadowCaster);		
		stream.Serialize("Materials", &m_aMaterials);

		#ifdef SHOOT_EDITOR
		if (stream.GetMode() == SM_Read)
			for (auto& material : m_aMaterials)
				material->SetOwner(this);
		#endif
	}

	//! Initializes the component
	void GraphicComponent::Init()
	{
		SHOOT_ASSERT(GetOwner()->IsA(RenderableEntity::TypeID), "GraphicComponent's parent not of type RenderableEntity");
	}

	//! sets the vertex buffer
	void GraphicComponent::SetVertexBuffer(VertexBuffer* pVB, size_t index /*= 0*/)
	{
		if(index < m_aVertexBuffers.size())
			m_aVertexBuffers[index] = pVB;
		else
		{
			m_aVertexBuffers.push_back(Reference<VertexBuffer>(pVB, this));
			SetTextureTransform(Matrix44::Identity, index);
		}
	}

	//! sets the material
	void GraphicComponent::SetMaterial(Material* pMaterial, size_t index /*= 0*/)
	{
		if(index < m_aMaterials.GetSize())
			m_aMaterials[index] = pMaterial;
		else
			m_aMaterials.Add(snew Reference<Material>(pMaterial, this));
	}

	//! sets the texture transformation matrix
	void GraphicComponent::SetTextureTransform(const Matrix44& transform, size_t index /*= 0*/)
	{
		if (index < (int)m_aTextureTransforms.size())
			m_aTextureTransforms[index] = transform;
		else
			m_aTextureTransforms.push_back(transform);
	}
}

