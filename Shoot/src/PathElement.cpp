/* 

Amine Rehioui
Created: August 3rd 2011

*/

#include "Shoot.h"

#include "PathElement.h"

namespace shoot
{
	DEFINE_OBJECT(PathElement);

	//! Constructor
	PathElement::PathElement()
		: m_fTime(0.0f)
	{
	}

	//! serializes the entity to/from a PropertyStream
	void PathElement::Serialize(PropertyStream& stream)
	{
		stream.Serialize("Position", &m_vPosition);
		stream.Serialize("Time", &m_fTime);
		m_bLocalTransformationMatrixDirty = true;
	}
}

