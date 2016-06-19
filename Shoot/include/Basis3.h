/* 

Amine Rehioui
Created: October 25th 2014

*/

#pragma once

#include "Vector3.h"

namespace shoot
{
	//! 3D Basis class
	class Basis3
	{
	public:

		//! constructor
		Basis3(const Vector3& position, const Vector3& forwardDir);

		//! transform a vector into this basis' local coordinates
		Vector3 Transform(const Vector3& v) const;
	
	public:

		Vector3 Position, Forward, Lateral, Vertical;
	};
}

