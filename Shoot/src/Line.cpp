/* 

Amine Rehioui
Created: October 26th 2014

*/

#include "Shoot.h"

#include "Line.h"

namespace shoot
{
	Line::Line(const Vector3& p1, const Vector3& p2)
		: P1(p1)
		, P2(p2)
	{		
		Dir = (P2-P1).Normalize();
	}	
}

