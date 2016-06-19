/* 

Amine Rehioui
Created: April 11th 2010

*/

#pragma once


namespace shoot
{
	//! 2D size class
	class Size
	{
	public:

		//! constructor
		Size() : Width(0), Height(0)
		{
		}

		//! constructor
		Size(int w, int h) : Width(w), Height(h)
		{
		}		

		// operators
		bool operator == (const Size& other) const { return (Width == other.Width && Height == other.Height); }
		inline bool operator != (const Size& other) const { return !((*this) == other); }

		int Width, Height;
	};
}



