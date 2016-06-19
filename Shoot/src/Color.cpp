/* 

Amine Rehioui
Created: May 1st 2010

*/

#include "Shoot.h"	

#include "Color.h"

namespace shoot
{
	// static variables initialization
	Color Color::Red = Color::Create(1.0f, 0.0f, 0.0f, 1.0f);
	Color Color::Green = Color::Create(0.0f, 1.0f, 0.0f, 1.0f);
	Color Color::Blue = Color::Create(0.0f, 0.0f, 1.0f, 1.0f);
	Color Color::White = Color::Create(1.0f, 1.0f, 1.0f, 1.0f);
	Color Color::Black = Color::Create(0.0f, 0.0f, 0.0f, 1.0f);
	Color Color::Yellow = Color::Create(1.0f, 1.0f, 0.0f, 1.0f);	
	Color Color::Zero = Color::Create(0.0f, 0.0f, 0.0f, 0.0f);
	Color Color::Pink = Color::Create(1.0f, 0.0f, 1.0f, 1.0f);

	bool Color::operator == (const Color& other) const
	{
		return (Math::FEqual(R, other.R)
			 && Math::FEqual(G, other.G)
			 && Math::FEqual(B, other.B)
			 && Math::FEqual(A, other.A));
	}

	Color Color::operator + (const Color& other) const
	{
		Color result = Color::Create(R+other.R, G+other.G, B+other.B, A+other.A);
		return result;
	}

	Color Color::operator - (const Color& other) const
	{
		Color result = Color::Create(R-other.R, G-other.G, B-other.B, A-other.A);
		return result;
	}

	Color Color::operator * (float fValue) const
	{
		Color result = Color::Create(R*fValue, G*fValue, B*fValue, A*fValue);
		return result;
	}
}

