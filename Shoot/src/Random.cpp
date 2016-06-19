/* 

Amine Rehioui
Created: May 30th 2010

Implementation based on http://www.codeproject.com/KB/recipes/SimpleRNG.aspx

*/

#include "Shoot.h"

#include <time.h>

namespace shoot
{
	// static variables initialization
	uint Random::m_z = 0;
	uint Random::m_w = 362436069;

	//! initializes the random number generator
	void Random::Initialize()
	{
		m_z = uint(time(NULL));
	}

	//! explicitly changes the seed
	void Random::SetSeed(uint seed)
	{
		m_z = seed;		
	}

	//! returns a number in range, exclusive
	float Random::GetFloat(float min, float max)
	{
		float value = GetFloat();
		return min + (value * (max - min));
	}

	//! returns an integer number in a range, inclusive
	uint Random::GetInt(uint min, uint max)
	{
		float value = GetFloat();
		return min + uint(value * (max + 1 - min));
	}

	//! returns a random number between 0 and 1
	float Random::GetFloat()
	{
		auto u = NextU32();
		// The magic number below is 1/(2^32 + 2).
		// The result is strictly between 0 and 1.
		return float(u + 1) * float(2.328306435454494e-10);
	}

	//! Uses George Marsaglia's MWC algorithm to produce an unsigned integer
	uint Random::NextU32()
	{
		m_z = 36969 * (m_z & 65535) + (m_z >> 16);
		m_w = 18000 * (m_w & 65535) + (m_w >> 16);
		return (m_z << 16) + (m_w & 65535);
	}
}

