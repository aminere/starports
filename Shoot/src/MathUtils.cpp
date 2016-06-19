/* 

Amine Rehioui
Created: March 20th 2010

*/

#include "Shoot.h"

#include <math.h> // cos

namespace shoot
{
	const float Math::PI = 3.141592f;
	const float Math::PIBy2 = 1.571428f;
	const float Math::PIBy4 = 0.785714f;
	const float Math::PITimes2 = 6.285714f;

	const float Math::Epsilon = 0.00001f;

	const float Math::Maxf32 = 3.402823466E+38f;

	//! factor to convert from degrees to radians
	const float Math::DegToRadFactor = 0.017453288f; // PI / 180.0f;
	const float Math::RadToDegFactor = 57.29579143f; // 180.0f / PI;

	//! returns true if a point is on a the line segment [A B]
	bool Math::IsPointOnLineSegment(const Vector3& vPoint, const Vector3& A, const Vector3& B)
	{		
		Vector3 delta = B - A;		
		bool bTIsSet = false;
		float t = 0.0f;
		
		// check X
		if(!FIsZero(delta.X))
		{
			t = (vPoint.X - A.X) / delta.X;
			bTIsSet = true;
		}
		else if(!FEqual(vPoint.X, A.X))
		{
			return false;
		}		

		// check Y
		if(!FIsZero(delta.Y))
		{
			float newT = (vPoint.Y - A.Y) / delta.Y;
			t = bTIsSet ? t : newT;
			bTIsSet = true;
			if(!FEqual(t, newT))
			{
				return false;
			}
		}
		else if(!FEqual(vPoint.Y, A.Y))
		{
			return false;
		}	

		// check Z
		if(!FIsZero(delta.Z))
		{
			float newT = (vPoint.Z - A.Z) / delta.Z;
			t = bTIsSet ? t : newT;
			bTIsSet = true;
			if(!FEqual(t, newT))
			{
				return false;
			}
		}
		else if(!FEqual(vPoint.Z, A.Z))
		{
			return false;
		}

		return bTIsSet ? (t >= 0.0f && t <= 1.0f) : true;		
	}

	//! returns the closest point on a line segment [A B]
	Vector3 Math::GetClosestPointOnLine(const Vector3& vPoint, const Vector3& vA, const Vector3& vB)
	{		
		Vector3 vC = vPoint-vA;
		Vector3 vD = vB-vA; 
		float fLength = vD.GetLength();

		vD = vD / fLength;
		float t = vD.DotProduct(vC);

		if(t < 0.0f) 
		{
			return vA;
		}
		else if(t > fLength) 
		{
			return vB;
		}

		return vA + vD*t;
	}

	//! projects a point on a line segment [A B]
	// return a factor T such as Intersection = A + (B-A).Normalized() * T
	float Math::ProjectPointOnLine(const Vector3& vPoint, const Vector3& vA, const Vector3& vB)
	{
		return (vB-vA).Normalize().DotProduct(vPoint-vA);
	}

	//! return the cosine of an angle
	/** \param fAngle: angle in radians */
	float Math::Cos(float fAngle)
	{
		return cos(fAngle);
	}

	//! return the sine of an angle
	/** \param fAngle: angle in radians */
	float Math::Sin(float fAngle)
	{
		return sin(fAngle);
	}

	//! returns the tangent of an angle
	float Math::Tan(float fAngle)
	{
		return tan(fAngle);
	}

	//! return the arc cosine
	float Math::ACos(float f)
	{
		return acos(Math::Clamp(f, -1.0f, 1.0f));
	}

	//! return the arc sine
	float Math::ASin(float f)
	{
		return asin(Math::Clamp(f, -1.0f, 1.0f));
	}

	//! return the arc tangent
	float Math::ATan(float f)
	{
		return atan(f);
	}

	//! return the arc tangent
	float Math::ATan2(float y, float x)
	{
		return atan2(y, x);
	}

	//! returns the floor of a number
	float Math::Floor(float f)
	{
		return floorf(f);
	}

	//! returns the square root of a number
	float Math::SquareRoot(float f)
	{
		return sqrtf(f);
	}

	//! converts from spherical coords to cartesian coords
	/** \param fRadius: sphere radius
		\param fAzimuth: azimuth angle [0 - 2Pi] 
		\parem fInclination: inclination angle [0 - Pi] */
	Vector3 Math::SphericalToCartesian(float fRadius, float fAzimuth, float fInclination)
	{
		Vector3 v;
		v.X = fRadius*Cos(fAzimuth)*Sin(fInclination);
		v.Y = fRadius*Sin(fAzimuth)*Sin(fInclination);
		v.Z = fRadius*Cos(fInclination);
		return v;
	}

	//! returns the next power of 2
	int Math::GetNextPow2(int i)
	{
		i--;
		i |= i >> 1;
		i |= i >> 2;
		i |= i >> 4;
		i |= i >> 8;
		i |= i >> 16;
		i++;
		return i;
	}
}

