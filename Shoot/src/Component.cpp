/*	

Amine Rehioui
Created: January 14th 2012

*/

#include "Shoot.h"

namespace shoot
{
	DEFINE_OBJECT(Component);

	//! constructor
	Component::Component()
		: m_pOwner(NULL)
	{
	}

	//! destructor
	Component::~Component()
	{
	}

	//! returns the root context
	Context* Component::GetRootContext() const
	{
		return m_pOwner->GetRootContext();
	}
}

