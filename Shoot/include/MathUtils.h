/* 

Amine Rehioui
Created: March 20th 2010

*/

#pragma once


namespace shoot
{
	// forwards
	class Vector3;

	//! Math utilities
	class Math
	{
	public:
			
		static const float PI;
		static const float PIBy2;
		static const float PIBy4;
		static const float PITimes2;

		static const float Epsilon;

		static const float Maxf32;

		//! returns true if a point is on a the line segment [A B]
		static bool IsPointOnLineSegment(const Vector3& vPoint, const Vector3& A, const Vector3& B);

		//! returns the closest point on a line segment [A B]
		static Vector3 GetClosestPointOnLine(const Vector3& vPoint, const Vector3& vA, const Vector3& vB);

		//! projects a point on a line segment [A B]
		// return a factor T such as Intersection = A + (B-A).Normalize() * T
		static float ProjectPointOnLine(const Vector3& vPoint, const Vector3& vA, const Vector3& vB);

		//! clamp a value between min and max
		template <class T>
		static T Clamp(T value, T min, T max)
		{
			if(value < min) return min;
			else if(value > max) return max;
			else return value;
		}

		//! swaps two values
		template <class T>
		static void Swap(T& value1, T& value2)
		{
			T temp = value1;
			value1 = value2;
			value2 = temp;
		}

		//! returns the absolute value
		inline static float FAbs(float fValue)
		{
			return ((fValue > 0.0f) ? fValue : -fValue);
		}

		//! compare two floating point values
		static bool FEqual(float fValue1, float fValue2, float fEpsilon = Epsilon)
		{
			return FAbs(fValue1 - fValue2) < fEpsilon;
		}

		//! compare a floating point with zero
		static bool FIsZero(float fValue, float fEpsilon = Epsilon)
		{
			return FAbs(fValue) < fEpsilon;
		}

		//! returns the sign
		static inline float FSign(float f)
		{
			return (f > 0.0f) ? (1.0f) : ((f < 0.0f) ? -1.0f : 0.0f);
		}

		//! returns the minimum of two values
		template <class T>
		static T Min(T a, T b) 
		{
			return (a < b) ? a : b;
		}

		//! returns the maximum of three values
		template <class T>
		static T Min(T a, T b, T c) 
		{
			return (a < b) ? (a < c ? a : c) : (b < c ? b : c);
		}

		//! returns the maximum of two values
		template <class T>
		static T Max(T a, T b) 
		{
			return (a > b) ? a : b;
		}

		//! returns the maximum of three values
		template <class T>
		static T Max(T a, T b, T c) 
		{
			return (a > b) ? (a > c ? a : c) : (b > c ? b : c);
		}

		//! return the cosine of an angle
		/** \param fAngle: angle in radians */
		static float Cos(float fAngle);

		//! return the sine of an angle
		/** \param fAngle: angle in radians */
		static float Sin(float fAngle);

		//! returns the tangent of an angle
		static float Tan(float fAngle);

		//! return the arc cosine
		static float ACos(float f);

		//! return the arc sine
		static float ASin(float f);

		//! return the arc tangent
		static float ATan(float f);

		//! return the arc tangent
		static float ATan2(float y, float x);

		//! linear interpolation
		template <class T, class U>
		static T Lerp(const T a, const T b, U k)
		{
			return (b - a)*k + a;
		}

		//! cubic interpolation
		template <class T, class U>
		static T CubicInterpolate(T p1, T p2, T p0, T p3, U k, U t)
		{
			U k2 = k*k;
			T a0, a1, a2, a3;			
			a0 = p3 - p2 - p0 + p1;
			a1 = p0 - p1 - a0;
			a2 = p2 - p0;
			a3 = p1;
			return (a0*k*k2 + a1*k2 + a2*k + a3);
		}

		//! catmull-rom interpolation
		template <class T, class U>
		static T CatmullRomInterpolate(T p1, T p2, T p0, T p3, U k, U t)
		{
			U k2 = k*k;
			T a0, a1, a2, a3;			
			a0 = p0*-0.5f + p1*1.5f - p2*1.5f + p3*0.5f;
			a1 = p0 - p1*2.5f + p2*2.0f - p3*0.5f;
			a2 = p0*-0.5f + p2*0.5f;
			a3 = p1;
			return (a0*k*k2 + a1*k2 + a2*k + a3);
		}

		//! hermite interpolation
		template <class T, class U>
		static T HermiteInterpolate(T p1, T p2, T p0, T p3, U k, U t)
		{
			const float k2 = k*k;
			const float k3 = k2*k;

			// hermite polynomials
			const float h1 = 2.0f * k3 - 3.0f * k2 + 1.0f;
			const float h2 = -2.0f * k3 + 3.0f * k2;
			const float h3 = k3 - 2.0f * k2 + k;
			const float h4 = k3 - k2;

			// tangents
			T t1 = (p2 - p0) * t;
			T t2 = (p3 - p1) * t;
			
			return (p1*h1 + p2*h2 + t1*h3 + t2*h4);
		}

		//! returns the floor of a number
		static float Floor(float f);

		//! returns the fractional part of a number
		static float Fraction(float f)
		{
			return f - Floor(f);
		}

		//! returns the square root of a number
		static float SquareRoot(float f);

		//! performs damping between two values
		template <class T, class U>
		static T Damp(const T src, const T dest, U deltaTime, U duration)
		{
			duration = Math::Clamp(duration, Epsilon, duration);
			auto factor = Math::Clamp((deltaTime / duration), 0.0f, 1.0f);
			return Lerp<T, U>(src, dest, factor);
		}

		//! converts from spherical coords to cartesian coords
		/** \param fRadius: sphere radius
		    \param fAzimuth: azimuth angle [0 - 2Pi] 
			\param fInclination: inclination angle [0 - Pi] */
		static Vector3 SphericalToCartesian(float fRadius, float fAzimuth, float fInclination);

		//! returns the next power of 2
		static int GetNextPow2(int i);

		//! factor to convert from degrees to radians
		static const float DegToRadFactor;
		static const float RadToDegFactor;
	};
}



