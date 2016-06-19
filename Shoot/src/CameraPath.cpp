/* 

Amine Rehioui
Created: August 14th 2011

*/

#include "Shoot.h"

#include "CameraPath.h"

namespace shoot
{
	DEFINE_OBJECT(CameraPath);

	//! adds a child to the entity
	void CameraPath::AddChild(Entity* pChild, bool bImmediate /*= false*/)
	{
		SHOOT_ASSERT(pChild->IsA(CameraPathElement::TypeID), "CameraPath::AddChild: child is not of type CameraPathElement");
		super::AddChild(pChild, bImmediate);
	}

	//! returns a camera look at position along the path
	/** \param fRatio: factor from 0.0f to 1.0f */
	Vector3 CameraPath::GetLookAt(float fRatio)
	{
		auto numElements = (int)GetChildCount();
		int srcIndex;
		int destIndex;
		float fFraction;
		if(m_bWrapAround)
		{
			float fIndex = fRatio*float(numElements);
			int iIndex = int(Math::Floor(fIndex));
			srcIndex = Math::Clamp<int>(iIndex, 0, numElements-1);
			destIndex = (srcIndex+1)%numElements;
			fFraction = (iIndex == numElements) ? 1.0f : Math::Fraction(fIndex);
		}
		else
		{
			float fIndex = fRatio*float(numElements-1);
			srcIndex = Math::Clamp<int>(int(Math::Floor(fIndex)), 0, numElements-1);
			destIndex = Math::Clamp<int>(srcIndex+1, 0, numElements-1);
			fFraction = Math::Fraction(fIndex);
		}

		return GetLookAt(srcIndex, destIndex, fFraction);
	}

	//! returns a camera look at position along the path
	Vector3 CameraPath::GetLookAt(int srcIndex, int destIndex, float fLocalRatio)
	{
		auto numElements = (int)GetChildCount();
		int preSrcIndex;
		int postDestIndex;

		if(m_bWrapAround)
		{
			preSrcIndex = (srcIndex > 0) ? (srcIndex-1) : (numElements-1);
			postDestIndex = (destIndex+1)%numElements;
		}
		else
		{
			preSrcIndex = Math::Clamp<int>(srcIndex-1, 0, numElements-1);
			postDestIndex = Math::Clamp<int>(destIndex+1, 0, numElements-1);
		}

		Vector3 p1 = GetElement(srcIndex)->GetCameraLookAt();
		Vector3 p2 = GetElement(destIndex)->GetCameraLookAt();
		Vector3 p0 = GetElement(preSrcIndex)->GetCameraLookAt();
		Vector3 p3 = GetElement(postDestIndex)->GetCameraLookAt();

		Matrix44 local = Matrix44::Identity;
		local.SetTranslation(Math::CatmullRomInterpolate(p1, p2, p0, p3, fLocalRatio, 1.0f));
		Matrix44 transformed = local * GetWorldTransform();
		return transformed.GetTranslation();
	}
}
