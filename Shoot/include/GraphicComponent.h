/* 

Amine Rehioui
Created: February 23rd 2013

*/

#pragma once


#include "Material.h"

#include "VertexBuffer.h"

namespace shoot
{
	//! GraphicComponent class
	class GraphicComponent : public Component
	{
		DECLARE_OBJECT(GraphicComponent, Component);

	public:

		//! rendering pass types
		enum E_RenderingPass
		{
			RP_SkyBox,
			RP_3D,
			RP_3DParticles,
			RP_3DTransparent,
			RP_2D,
			RP_2DTransparent,
			RP_2DPostFade,
			RP_None,
			RP_3DDepthOff,
			RP_2DLayout,
			RP_PostEffect,
			RP_2DLayoutPostTransparent,
			RP_Count
		};

		//! constructor
		GraphicComponent();

		//! destructor
		virtual ~GraphicComponent();

		//! serializes the entity to/from a PropertyStream
		virtual void Serialize(PropertyStream& stream);

		//! Initializes the component
		virtual void Init();

		//! sets the vertex buffer
		void SetVertexBuffer(VertexBuffer* pVB, size_t index = 0);

		//! returns the vertex buffer
		inline VertexBuffer* GetVertexBuffer(size_t index = 0) const { return index < m_aVertexBuffers.size() ? m_aVertexBuffers[index].Get() : NULL; }

		//! returns the vertex buffer count
		inline size_t GetVertexBufferCount() const { return m_aVertexBuffers.size(); }

		//! sets the material
		void SetMaterial(Material* pMaterial, size_t index = 0);

		//! returns the material
		inline Material* GetMaterial(size_t index = 0) const { return index < m_aMaterials.GetSize() ? m_aMaterials[index].Get() : NULL; }

		//! returns the material count
		inline size_t GetMaterialCount() const { return m_aMaterials.GetSize(); }

		//! returns the rendering priority
		inline uint GetRenderingPriority() const { return m_RenderingPriority; }

		//! sets the 2D rendering priority
		inline void SetRenderingPriority(uint priority) { m_RenderingPriority = priority; }

		//! returns the rendering pass
		E_RenderingPass GetRenderingPass() const { return m_eRenderingPass; }

		//! sets the rendering pass
		inline void SetRenderingPass(E_RenderingPass ePass) { m_eRenderingPass = ePass; }

		//! returns the texture transformation matrix
		inline const Matrix44& GetTextureTransform(size_t index = 0) const { return m_aTextureTransforms[index]; }

		//! sets the texture transformation matrix
		void SetTextureTransform(const Matrix44& transform, size_t index = 0);

		//! returns the world transformation matrix
		virtual Matrix44 GetWorldTransform() const { return static_cast<RenderableEntity*>(GetOwner())->GetWorldTransform(); }

		//! returns shadow casting status
		inline bool IsShadowCaster() const { return m_bShadowCaster; }

		//! SetShadowCaster
		inline void SetShadowCaster(bool set) { m_bShadowCaster = set; }

	protected:

		std::vector< Reference<VertexBuffer> > m_aVertexBuffers;
		std::vector<Matrix44> m_aTextureTransforms;

		// properties
		Array< Reference<Material> > m_aMaterials;
		uint m_RenderingPriority;
		E_RenderingPass m_eRenderingPass;
		bool m_bShadowCaster;
	};
}



