/* 

Amine Rehioui
Created: April 6th 2014

*/

#pragma once

#include "Text.h"

namespace shoot
{
	//! Text3D
	class Text3D : public Text				 
	{
	public:

		//! Macro to provide type information
		DECLARE_OBJECT(Text3D, Text);

		//! returns the affine transformation matrix
		/** TODO: avoid recalculating when nothing changed */
		Matrix44 GetWorldTransform();
	};
}

