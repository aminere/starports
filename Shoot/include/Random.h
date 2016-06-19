/* 

Amine Rehioui
Created: May 30th 2010

Implementation based on http://www.codeproject.com/KB/recipes/SimpleRNG.aspx

*/

#pragma once


namespace shoot
{
	//! Random number generator
	class Random
	{
	public:

		//! initializes the random number generator
		static void Initialize();

		//! explicitly changes the seed
		static void SetSeed(uint seed);

		//! returns a number in range, exclusive
		static float GetFloat(float min, float max);

		//! returns an integer number in a range, inclusive
		static uint GetInt(uint min, uint max);

		//! returns a random number between 0 and 1
		static float GetFloat();

		//! Uses George Marsaglia's MWC algorithm to produce an unsigned integer
		static uint NextU32();

	private:

		static uint m_w;
		static uint m_z;
	};
}



