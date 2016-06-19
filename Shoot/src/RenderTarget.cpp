/* 

Amine Rehioui
Created: February 4th 2014

*/

#include "Shoot.h"

#include "RenderTarget.h"

namespace shoot
{
	DEFINE_OBJECT(RenderTarget);

	//! constructor
	RenderTarget::RenderTarget(const Vector2& vSize, int typeMask)
		: m_vSize(vSize)
		, m_TypeMask(typeMask)
	{
		SetName("RenderTarget");
	}
}

