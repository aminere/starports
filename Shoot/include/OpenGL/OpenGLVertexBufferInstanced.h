/* 

Amine Rehioui
Created: April 7th 2015

*/

#pragma once


#include "VertexBuffer.h"

namespace shoot
{
	//! OpenGLVertexBufferInstanced
	class OpenGLVertexBufferInstanced : public VertexBuffer
	{
	public:

		//! constructor
		OpenGLVertexBufferInstanced();

		//! destructor
		virtual ~OpenGLVertexBufferInstanced();

		//! Load into video memory
		virtual void GraphicLoad();

		//! Unload from video memory
		virtual void GraphicUnload();

		//! returns loaded status
		virtual bool IsLoaded() const { return m_VAO_ID != 0; }

		//! updates the vertex buffer data
		virtual void UpdateData();

		//! begins rendering using this vertex buffer
		virtual void Begin(Shader* pShader);

		//! render method
		virtual void Draw(const std::vector<Matrix44>& aWorldTransforms, const std::vector<Matrix44>& aTextureTransforms);

		//! create a copy of the object
		virtual Object* Copy() const;

	protected:

		GLuint m_VAO_ID;
		GLuint m_VBO_ID;
		GLuint m_IBO_ID;
		GLuint m_WorldMatrixBufferID;
		GLuint m_ProgramID;
		int _glIndexType;
	};
}



