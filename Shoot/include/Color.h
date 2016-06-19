/* 

Amine Rehioui
Created: April 18th 2010

*/

#pragma once


namespace shoot
{
	//! Color class
	class Color
	{
	public:

		inline static Color Create(float r, float g, float b, float a = 1.0f)
		{
			Color col = { r, g, b, a };
			return col;
		}

		inline static Color Create(const Vector3& rgb, float a = 1.0f)
		{
			Color col = { rgb.X, rgb.Y, rgb.Z, a };
			return col;
		}

		//! sets the color
		void Set(float r, float g, float b, float a = 1.0f)
		{
			R = r;
			G = g;
			B = b;
			A = a;
		}

		// static constants
		static Color Red;
		static Color Green;
		static Color Blue;
		static Color White;
		static Color Black;
		static Color Yellow;
		static Color Pink;
		static Color Zero;

		// operators
		bool operator == (const Color& other) const;
		inline bool operator != (const Color& other) const { return !((*this == other)); }
		Color operator + (const Color& other) const;
		Color operator - (const Color& other) const;
		Color operator * (float fValue) const;

		union
		{
			struct
			{
				float R, G, B, A;
			};

			struct
			{
				Vector3 RGB;
				float A2;
			};

			float v[4];
		};		
	};
}



