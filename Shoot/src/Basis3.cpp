/* 

Amine Rehioui
Created: October 25th 2014

*/

#include "Shoot.h"

#include "Basis3.h"

namespace shoot
{
	//! constructor
	Basis3::Basis3(const Vector3& position, const Vector3& forwardDir)
	{
		Position = position;
		Forward = forwardDir.DifferentiateFromUp(); // Make sure dir != Up to stabilize basis determination
		auto cross = Forward.CrossProduct(Vector3::Up);
		Lateral = Forward.CrossProduct(Vector3::Up).Normalized();
		Vertical = Lateral.CrossProduct(Forward).Normalized();
	}	

	//! transform a vector into this basis' local coordinates
	Vector3 Basis3::Transform(const Vector3& v) const
	{
		Vector3 transformed;
		auto localPos = v - Position;
		transformed.X = Lateral.DotProduct(localPos);
		transformed.Y = Forward.DotProduct(localPos);
		transformed.Z = Vertical.DotProduct(localPos);
		return transformed;
	}
}

