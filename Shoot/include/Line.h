/* 

Amine Rehioui
Created: October 26th 2014

*/

#pragma once


namespace shoot
{
	class Line
	{
	public:

		Line(const Vector3& p1, const Vector3& p2);

		Vector3 P1;
		Vector3 P2;
		Vector3 Dir;
	};
}



