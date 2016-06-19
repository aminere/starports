/* 

Amine Rehioui
Created: August 14th 2010

*/

#pragma once

#include "Vector2.h"
#include "Color.h"

namespace shoot
{
	//! 3D vertex class used by the graphics driver
	class Vertex3D
	{
	public:

		//! vertex flags
		enum E_VertexFlag
		{
			VF_Pos =	0x1 << 0,
			VF_UV  =	0x1 << 1,
			VF_Normal = 0x1 << 2,
			VF_Color =	0x1 << 3,
			VF_TangentBinormal = 0x1 << 4
		};

		Vector3 Pos;
		Vector2 UV;
		Vector3 Normal;
		Color color;
		Vector4 Tangent; // w component used to determine the direction of the Binormal ( Binormal = TxN * w )
	};
}

