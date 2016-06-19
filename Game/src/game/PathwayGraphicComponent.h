/* 

Amine Rehioui
Created: December 27th 2013

*/

#pragma once


#include "GraphicComponent.h"

namespace shoot
{
	//! PathwayGraphicComponent class
	class PathwayGraphicComponent : public GraphicComponent
	{
		DECLARE_OBJECT(PathwayGraphicComponent, GraphicComponent);

	public:	

		//! constructor
		PathwayGraphicComponent();

		//! serializes the entity to/from a PropertyStream
		void Serialize(PropertyStream& stream);

		//! Initializes the component
		void Init();

		//! Updates the component
		void Update();

		//! returns the world transformation matrix
		Matrix44 GetWorldTransform() const { return Matrix44::Identity; }

		//! updates the path
		void  UpdatePath();

		//! SetNumSegments
		void SetNumSegments(int segments);

	private:

		float m_fTextureOffsetX;

		// properties
		int m_NumSegments;
		float m_fAnimSpeed;
		float m_fThickness;
		float _UPerSegment;
	};
}



