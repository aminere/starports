/* 

Amine Rehioui
Created: February 8th 2014

*/

#include "Shoot.h"

#include "OpenGLVertexBufferNoVAO.h"

#include "OpenGLExtensionHandler.h"
#include "OpenGLShader.h"

namespace shoot
{
	//! Load into video memory
	void OpenGLVertexBufferNoVAO::GraphicLoad()
	{
		SHOOT_ASSERT(GraphicExtensionHandler::Instance()->HasExtension(GraphicExtensionHandler::E_VBO), "VBO extension not supported");
		SHOOT_ASSERT(m_VBO_ID == 0 && m_IBO_ID == 0, "VertexBuffer already loaded");
		
		if(m_pIndices)
		{
			OpenGLExtensionHandler::Instance()->extGlGenBuffers(1, &m_IBO_ID);
			OpenGLExtensionHandler::Instance()->extGlBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO_ID);
			OpenGLExtensionHandler::Instance()->extGlBufferData(GL_ELEMENT_ARRAY_BUFFER, m_NumIndices*_sizeOfIndex, m_pIndices, GL_STATIC_DRAW);
			_glIndexType = (_sizeOfIndex == 4) ? GL_UNSIGNED_INT : GL_UNSIGNED_SHORT;
		}

		OpenGLExtensionHandler::Instance()->extGlGenBuffers(1, &m_VBO_ID);
		OpenGLExtensionHandler::Instance()->extGlBindBuffer(GL_ARRAY_BUFFER, m_VBO_ID);
		GLenum usage = m_bDynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW;
		OpenGLExtensionHandler::Instance()->extGlBufferData(GL_ARRAY_BUFFER, m_MaxVertices*m_VertexSize, m_pVertices, usage);
		m_bDirty = false;

		if(!m_bDynamic)
		{
#ifndef SHOOT_EDITOR // keep the vertices when in the editor, needed for triangle picking
			DeleteVertices();
			DeleteIndices();
#endif // SHOOT_EDITOR
		}
	}

	//! updates the vertex buffer data
	void OpenGLVertexBufferNoVAO::UpdateData()
	{
		if(m_pVertices && m_VBO_ID)
		{
			GLenum usage = m_bDynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW;
			OpenGLExtensionHandler::Instance()->extGlBindBuffer(GL_ARRAY_BUFFER, m_VBO_ID);
			OpenGLExtensionHandler::Instance()->extGlBufferData(GL_ARRAY_BUFFER, m_MaxVertices*sizeof(Vertex3D), m_pVertices, usage);
			m_bDirty = false;
		}
	}

	//! begins rendering using this vertex buffer
	void OpenGLVertexBufferNoVAO::Begin(Shader* pShader)
	{
		VertexBuffer::Begin(pShader);

		pShader->BeginInstance();

		if(m_VBO_ID == 0)
		{
			GraphicLoad();
		}

		OpenGLExtensionHandler::Instance()->extGlBindBuffer(GL_ARRAY_BUFFER, m_VBO_ID);

		const OpenGLShader* pGLShader = static_cast<const OpenGLShader*>(pShader->GetImpl());
		//if(pGLShader->m_ProgramID != m_ProgramID)
		{
			int attribToDisable = 0;
			// setup attributes
			for (size_t i = 0; i<pShader->GetAttributes().GetSize(); ++i)
			{
				std::string name = pShader->GetAttributes()[i];
				int attribLocation = -1;
				OpenGLExtensionHandler::Instance()->extGlGetAttribLocation(pGLShader->m_ProgramID, name.c_str(), &attribLocation);
				SHOOT_WARNING(attribLocation >= 0, "Shader '%s' attribute '%s' not found", pShader->GetTemplatePath().c_str(), name.c_str());
				AttributeInfo* pAttribInfo = GetAttribInfo(name);
				SHOOT_WARNING(pAttribInfo, "VertexBuffer attribute '%s' not found", name.c_str());
				if(pAttribInfo)
				{
					OpenGLExtensionHandler::Instance()->extGlEnableVertexAttribArray(attribLocation);
					GLenum glTypes[ADT_Count] = { GL_FLOAT, GL_UNSIGNED_INT };
					OpenGLExtensionHandler::Instance()->extGlVertexAttribPointer(attribLocation, pAttribInfo->m_DataCount, glTypes[pAttribInfo->m_DataType], GL_FALSE, m_VertexSize, (void*)pAttribInfo->m_Offset);
					attribToDisable = attribLocation + 1;
				}
			}

			//for (int i = attribToDisable; i < 5; ++i)
			//	OpenGLExtensionHandler::Instance()->extGlDisableVertexAttribArray(i);
					
			m_ProgramID = pGLShader->m_ProgramID;
		}

		if(m_IBO_ID)
		{
			OpenGLExtensionHandler::Instance()->extGlBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO_ID);
		}
	}

	//! create a copy of the object
	Object* OpenGLVertexBufferNoVAO::Copy() const
	{
		OpenGLVertexBufferNoVAO* pCopy = snew OpenGLVertexBufferNoVAO();
		int ID = pCopy->GetID();

		*pCopy = *this;

		// make sure copy properties are initialized properly
		pCopy->SetID(ID);
		pCopy->m_pVertices = NULL;
		pCopy->m_pIndices = NULL;
		pCopy->m_VAO_ID = 0;
		pCopy->m_VBO_ID = 0;
		pCopy->m_IBO_ID = 0;
		pCopy->m_ProgramID = -1;
		return pCopy;
	}
}

