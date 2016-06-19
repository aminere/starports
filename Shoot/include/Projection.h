/* 

Amine Rehioui
Created: October 24th 2013

*/

#pragma once


namespace shoot
{
	//! Projection
	class Projection
	{
	public:

		enum Type
		{
			Perspective,
			Ortho3D,
			Ortho2D
		};

		static const char* const Literals[4];
	};	
}



