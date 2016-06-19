/* 

Amine Rehioui
Created: February 4th 2014

*/

#pragma once


#include "GraphicObject.h"

namespace shoot
{
	//! base RenderTarget class
	class RenderTarget : public Object, public GraphicObject
	{
		DECLARE_OBJECT(RenderTarget, Object);

	public:

		//! constructor
		RenderTarget()
		{
		}

		//! constructor
		RenderTarget(const Vector2& vSize, int typeMask);

		//! destructor
		virtual ~RenderTarget()
		{
		}

		//! resize the texture
		virtual void Resize(const Vector2& vSize) { }

		//! start rendering to this render target
		virtual void Bind() { }

		//! binds a texture attached to this render target
		virtual void BindTexture(int slot, int type) { }

		//! returns the type mask
		inline int GetTypeMask() const { return m_TypeMask; }

		//! returns the size
		inline const Vector2& GetSize() const { return m_vSize; }

	protected:

		Vector2 m_vSize;
		int m_TypeMask;
	};
}



