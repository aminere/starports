/* 

Amine Rehioui
Created: March 15th 2014

*/

#pragma once


namespace shoot
{
	//! Tile
	class Tile
	{
	public:

		//! Neighbor type
		enum E_NeighborType
		{
			NT_Left,
			NT_Right,
			NT_TopLeft,
			NT_TopRight,
			NT_BottomLeft,
			NT_BottomRight,
			NT_Count
		};

		//! constructor
		Tile()
			: m_vPosition(Vector3::Zero)
			, m_bAnimating(false)
			, m_bBuildable(true)
			, m_bBlocking(false)
		{
			memset(m_aNeighBours, 0, NT_Count*sizeof(Tile*));
		}

		Handle<Entity3D> m_Actor;
		Tile* m_aNeighBours[NT_Count];
		Vector3 m_vPosition;
		bool m_bAnimating;
		bool m_bBuildable;
		bool m_bBlocking;
	};
}



