/* 

Amine Rehioui
Created: August 4th 2010

*/

#include "Shoot.h"

#include "App.h"

namespace shoot
{
	//! statics
	App* App::m_spInstance = NULL;

	//! constructor
	App::App(bool bFullScreen /*= false*/)
		: m_bFullScreen(bFullScreen)
	{
		SHOOT_ASSERT(!m_spInstance, "Multiple App instances detected");
		m_spInstance = this;
	}

	//! destructor
	App::~App()
	{
		m_spInstance = NULL;
	}
}

