/* 

Amine Rehioui
Created: February 8th 2014

*/

#pragma once


#include "OpenGLVertexBuffer.h"

namespace shoot
{
	//! OpenGL vertex buffer
	class OpenGLVertexBufferNoVAO : public OpenGLVertexBuffer
	{
	public:

		//! Load into video memory
		void GraphicLoad();

		//! returns loaded status
		bool IsLoaded() const { return m_VBO_ID != 0; }

		//! updates the vertex buffer data
		void UpdateData();

		//! begins rendering using this vertex buffer
		void Begin(Shader* pShader);

		//! create a copy of the object
		Object* Copy() const;
	};
}



