/* 

Amine Rehioui
Created: December 11th 2013

*/

#pragma once


#include "Path.h"

namespace shoot
{
	//! Pathway Element
	class PathwayElement : public PathElement
	{
	public:

		//! Macro to provide type information
		DECLARE_OBJECT(PathwayElement, PathElement);

		//! constructor
		PathwayElement();

		//! serializes the entity to/from a PropertyStream
		void Serialize(PropertyStream& stream);

		//! returns the tilt
		inline float GetTilt() const { return m_fTilt; }

	private:

		//! properties
		float m_fTilt;
	};

	//! Pathway
	class Pathway : public Path
	{
		DECLARE_OBJECT(Pathway, Path);

	public:

		//! adds a child to the entity
		void AddChild(Entity* pChild, bool bImmediate = false);

		//! returns a path element
		inline PathwayElement* GetElement(int index) const { return static_cast<PathwayElement*>(GetChild(index)); }
	};
}




