/* 

Amine Rehioui
Created: August 3rd 2011

*/

#include "Shoot.h"

#include "Path.h"

namespace shoot
{
	DEFINE_OBJECT(Path);

	// Interpolation literals
	const char* const g_strInterpolationLiterals[] =
	{
		"I_Hermite",
		"I_CatmullRom",
		"I_Cubic",
		0
	};

	//! Constructor
	Path::Path()
		: m_pInterpolator(&Math::HermiteInterpolate<Vector3, float>)
		// properties
		, m_bWrapAround(false)
		, m_eInterpolation(I_Hermite)
		, m_fTightness(0.5f)
	{
	}

	//! serializes the entity to/from a PropertyStream
	void Path::Serialize(PropertyStream& stream)
	{
		super::Serialize(stream);
		stream.Serialize("WrapAround", &m_bWrapAround);
		stream.Serialize("Tightness", &m_fTightness);
		stream.Serialize("Interpolation", &m_eInterpolation, ENUM_PARAMS1(g_strInterpolationLiterals));
	}

	//! called during the initialization of the entity
	void Path::Init()
	{
		super::Init();

		switch(m_eInterpolation)
		{
		case I_Hermite: m_pInterpolator = &Math::HermiteInterpolate<Vector3, float>; break;
		case I_CatmullRom: m_pInterpolator = &Math::CatmullRomInterpolate<Vector3, float>; break;
		case I_Cubic: m_pInterpolator = &Math::CubicInterpolate<Vector3, float>; break;
		}
	}

	//! adds a child to the entity
	void Path::AddChild(Entity* pChild, bool bImmediate /*= false*/)
	{
		SHOOT_ASSERT(pChild->IsA(PathElement::TypeID), "Path::AddChild: child is not of type PathElement");
		super::AddChild(pChild, bImmediate);
	}

	//! returns a position along the path
	/** \param fRatio: factor from 0.0f to 1.0f */
	Vector3 Path::GetPathPosition(float fRatio, int* pSrcElementIndex /*= NULL*/)
	{
		int numElements = (int)GetChildCount();
		int srcIndex;
		int destIndex;
		float fFraction;

		if(m_bWrapAround)
		{
			fRatio = Math::Fraction(fRatio);
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

		if(pSrcElementIndex)
			*pSrcElementIndex = srcIndex;		

		return GetPathPosition(srcIndex, destIndex, fFraction);
	}

	//! returns a position along the path
	Vector3 Path::GetPathPosition(int srcIndex, int destIndex, float fLocalRatio)
	{
		int numElements = (int)GetChildCount();
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

		Vector3 p1 = GetElement(srcIndex)->GetPosition();
		Vector3 p2 = GetElement(destIndex)->GetPosition();
		Vector3 p0 = GetElement(preSrcIndex)->GetPosition();
		Vector3 p3 = GetElement(postDestIndex)->GetPosition();

		Matrix44 local = Matrix44::Identity;
		local.SetTranslation(m_pInterpolator(p1, p2, p0, p3, fLocalRatio, m_fTightness));
		Matrix44 transformed = local * GetWorldTransform();
		return transformed.GetTranslation();
	}
}

