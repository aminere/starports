/* 

Amine Rehioui
Created: October 27th 2013

*/

#pragma once


#include "AnimationVisitor.h"

namespace shoot
{
	//! MaterialVisitor class
	class MaterialVisitor : public AnimationVisitor
	{
	public:

		DECLARE_OBJECT(MaterialVisitor, AnimationVisitor);

		//! constructor
		MaterialVisitor();

		//! destructor
		virtual ~MaterialVisitor();

		//! Reads/Writes struct properties from/to a stream
		virtual void Serialize(PropertyStream& stream);		

		//! visits a particular entity
		virtual void Visit(Entity* pTarget);
	
		//! updates the visitor
		virtual void Update();

	protected:
		
		float m_fInterpolator;
		float m_fCurrentDuration;

		//! updates the color
		void UpdateColor(const Color& color);

		// properties
		Color m_SrcColor;
		Color m_DestColor;
		float m_fDuration;
		bool m_bRandomize;
	};
}




