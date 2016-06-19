/* 

Amine Rehioui
Created: April 9th 2011

*/

#pragma once


#include "AnimationVisitor.h"

#include "VertexBuffer.h"

namespace shoot
{
	//! ColorVisitor class
	class ColorVisitor : public AnimationVisitor
	{
	public:

		DECLARE_OBJECT(ColorVisitor, AnimationVisitor);

		//! constructor
		ColorVisitor();

		//! destructor
		virtual ~ColorVisitor();

		//! Reads/Writes struct properties from/to a stream
		virtual void Serialize(PropertyStream& stream);		

		//! visits a particular entity
		virtual void Visit(Entity* pTarget);
	
		//! Leaves the visited entity
		virtual void Leave();

		//! updates the visitor
		virtual void Update();

	protected:
		
		std::vector< Reference<VertexBuffer> > m_VertexBuffers;
		std::vector< Reference<VertexBuffer> > m_OriginalVertexBuffers;

		float m_fInterpolator;	
		Color m_SrcColor;
		Color m_DestColor;

		//! updates the color
		void UpdateColor(const Color& color);

		//! adds a vertex buffer
		void AddVertexBuffer(GraphicComponent* pGraphic);

		// properties
		Color m_Color;
		float m_fAlpha;
		float m_fDuration;
	};
}




