/* 

Amine Rehioui
Created: May 30th 2010

*/

#pragma once


namespace shoot
{
	//! Range class
	class Range
	{
	public:

		//! constructor
		Range() : Min(0.0f), Max(1.0f)
		{
		}

		//! constructor
		Range(float min, float max) : Min(min), Max(max)
		{
		}	

		inline bool operator == (const Range& other) const
		{
			return Math::FEqual(Min, other.Min) && Math::FEqual(Max, other.Max);
		}

		inline bool operator != (const Range& other) const
		{
			return !this->operator==(other);
		}

		float Min, Max;
	};
}



