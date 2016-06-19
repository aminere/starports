/* 

Amine Rehioui
Created: May 5th 2014

*/

#pragma once


#include "AnimationVisitor.h"

namespace shoot
{
	//! LightVisitor class
	class LightVisitor : public AnimationVisitor
	{
	public:

		DECLARE_OBJECT(LightVisitor, AnimationVisitor);

		//! constructor
		LightVisitor();

		//! destructor
		virtual ~LightVisitor();

		//! Reads/Writes struct properties from/to a stream
		virtual void Serialize(PropertyStream& stream);		

		//! visits a particular entity
		virtual void Visit(Entity* pTarget);
	
		//! updates the visitor
		virtual void Update();

	protected:

		Color m_SrcColor;
		float m_fInterpolator;

		//! updates the color
		void UpdateColor(const Color& color);

		// properties		
		Color m_DestColor;
		float m_fDuration;
	};
}




