/* 

Amine Rehioui
Created: Marche 20th 2011

*/

#include "Shoot.h"

#include "OpenGLVertexBuffer.h"
#include "OpenGLShader.h"

#include "OpenGLExtensionHandler.h"

namespace shoot
{
	//! constructor
	OpenGLVertexBuffer::OpenGLVertexBuffer()
		: m_VAO_ID(0)
		, m_VBO_ID(0)
		, m_IBO_ID(0)
		, m_ProgramID(-1)
		, _glIndexType(GL_UNSIGNED_SHORT)
	{
	}

	//! destructor
	OpenGLVertexBuffer::~OpenGLVertexBuffer()
	{
		GraphicUnload();
	}

	//! Load into video memory
	void OpenGLVertexBuffer::GraphicLoad()
	{
		SHOOT_ASSERT(m_VAO_ID == 0, "VertexBuffer already loaded");
		SHOOT_ASSERT(m_VBO_ID == 0, "OpenGLVertexBuffer error");
		SHOOT_ASSERT(m_IBO_ID == 0, "OpenGLVertexBuffer error");

		OpenGLExtensionHandler::Instance()->extGlGenVertexArrays(1, &m_VAO_ID);
		OpenGLExtensionHandler::Instance()->extGlBindVertexArray(m_VAO_ID);

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

		OpenGLExtensionHandler::Instance()->extGlBindVertexArray(0);
		OpenGLExtensionHandler::Instance()->extGlBindBuffer(GL_ARRAY_BUFFER, 0);
		OpenGLExtensionHandler::Instance()->extGlBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		if(!m_bDynamic)
		{
#ifndef SHOOT_EDITOR // keep the vertices when in the editor, needed for triangle picking
			DeleteVertices();
			DeleteIndices();
#endif // SHOOT_EDITOR
		}
	}

	//! updates the vertex buffer data
	void OpenGLVertexBuffer::UpdateData()
	{
		if (!(m_pVertices && m_VBO_ID))
			return;

		OpenGLExtensionHandler::Instance()->extGlBindBuffer(GL_ARRAY_BUFFER, m_VBO_ID);
		GLenum usage = m_bDynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW;
		OpenGLExtensionHandler::Instance()->extGlBufferData(GL_ARRAY_BUFFER, m_MaxVertices*m_VertexSize, m_pVertices, usage);
		OpenGLExtensionHandler::Instance()->extGlBindBuffer(GL_ARRAY_BUFFER, 0);
		m_bDirty = false;
	}

	//! begins rendering using this vertex buffer
	void OpenGLVertexBuffer::Begin(Shader* pShader)
	{
		VertexBuffer::Begin(pShader);

		pShader->BeginInstance();

		if(m_VAO_ID == 0)
		{
			GraphicLoad();
		}

		OpenGLExtensionHandler::Instance()->extGlBindVertexArray(m_VAO_ID);

		const OpenGLShader* pGLShader = static_cast<const OpenGLShader*>(pShader->GetImpl());
		if(pGLShader->m_ProgramID != m_ProgramID)
		{
			OpenGLExtensionHandler::Instance()->extGlBindBuffer(GL_ARRAY_BUFFER, m_VBO_ID);

			#ifndef _RETAIL_
			std::map<size_t, int> offsetChecker;
			#endif

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

					#ifndef _RETAIL_
						if (offsetChecker.find(pAttribInfo->m_Offset) != offsetChecker.end())
						{
							SHOOT_WARNING(false, "Duplicate vertex offset detected for attribute '%s' (shader %s)", name.c_str(), pShader->GetTemplatePath().c_str());
						}
						else
							offsetChecker[pAttribInfo->m_Offset] = 0;
					#endif
				}
			}

			m_ProgramID = pGLShader->m_ProgramID;
		}
	}

	//! render method
	void OpenGLVertexBuffer::Draw(const std::vector<Matrix44>& aWorldTransforms, const std::vector<Matrix44>& aTextureTransforms)
	{
		static int glPrimitives[] = { GL_POINTS,
									  GL_LINES,
									  GL_LINE_LOOP,
									  GL_LINE_STRIP,
									  GL_TRIANGLES,
									  GL_TRIANGLE_STRIP,
									  GL_TRIANGLE_FAN };

		const OpenGLShader* pGLShader =  static_cast<const OpenGLShader*>(m_Shader->GetImpl());
		auto hasWVP = pGLShader->m_WorldViewProjection >= 0;
		auto hasWorldMatrix = false;
		if (!hasWVP)
			hasWorldMatrix = pGLShader->m_WorldMatrix >= 0;
		
		if(m_bApplyWorldTransforms)
		{
			for (size_t i=0; i<aWorldTransforms.size(); ++i)
			{
				if (hasWVP)
				{
					const Matrix44& viewProjection = GraphicsDriver::Instance()->GetTransform(GraphicsDriver::TS_ViewProjection);
					OpenGLExtensionHandler::Instance()->extGlUniformMatrix4fv(pGLShader->m_WorldViewProjection, 1, false, (aWorldTransforms[i]*viewProjection).GetFloats());
				}
				else if (hasWorldMatrix)
				{
					OpenGLExtensionHandler::Instance()->extGlUniformMatrix4fv(pGLShader->m_WorldMatrix, 1, false, aWorldTransforms[i].GetFloats());
				}

				if (pGLShader->m_TextureMatrix >= 0)
				{
					if (i < aTextureTransforms.size())
						OpenGLExtensionHandler::Instance()->extGlUniformMatrix4fv(pGLShader->m_TextureMatrix, 1, false, aTextureTransforms[i].GetFloats());
					else
						OpenGLExtensionHandler::Instance()->extGlUniformMatrix4fv(pGLShader->m_TextureMatrix, 1, false, Matrix44::Identity.GetFloats());
				}

				if(m_IBO_ID)
				{
					GL_CHECK(glDrawElements(glPrimitives[m_ePrimitiveType], m_NumIndices, _glIndexType, 0));
				}
				else
				{
					GL_CHECK(glDrawArrays(glPrimitives[m_ePrimitiveType], m_StartVertex, m_NumVertices));
				}

				EntityRenderer::Instance()->GetStats().Increment(this);
			}
		}
		else
		{
			if (hasWVP)
			{
				const Matrix44& viewProjection = GraphicsDriver::Instance()->GetTransform(GraphicsDriver::TS_ViewProjection);
				OpenGLExtensionHandler::Instance()->extGlUniformMatrix4fv(pGLShader->m_WorldViewProjection, 1, false, viewProjection.GetFloats());
			}
			else if (hasWorldMatrix)
			{
				OpenGLExtensionHandler::Instance()->extGlUniformMatrix4fv(pGLShader->m_WorldMatrix, 1, false, Matrix44::Identity.GetFloats());
			}

			if (pGLShader->m_TextureMatrix >= 0)
			{
				if (aTextureTransforms.size() > 0)
					OpenGLExtensionHandler::Instance()->extGlUniformMatrix4fv(pGLShader->m_TextureMatrix, 1, false, aTextureTransforms[0].GetFloats());
				else
					OpenGLExtensionHandler::Instance()->extGlUniformMatrix4fv(pGLShader->m_TextureMatrix, 1, false, Matrix44::Identity.GetFloats());
			}

			if(m_IBO_ID)
			{
				GL_CHECK(glDrawElements(glPrimitives[m_ePrimitiveType], m_NumIndices, _glIndexType, 0));
			}
			else
			{
				GL_CHECK(glDrawArrays(glPrimitives[m_ePrimitiveType], 0, m_NumVertices));
			}

			EntityRenderer::Instance()->GetStats().Increment(this);
		}
	}

	//! Unload from video memory
	void OpenGLVertexBuffer::GraphicUnload()
	{
		if(m_VAO_ID)
		{
			OpenGLExtensionHandler::Instance()->extGlBindVertexArray(0);
			OpenGLExtensionHandler::Instance()->extGlDeleteVertexArrays(1, &m_VAO_ID);
			m_VAO_ID = 0;
		}

		if(m_VBO_ID)
		{
			OpenGLExtensionHandler::Instance()->extGlBindBuffer(GL_ARRAY_BUFFER, 0);
			OpenGLExtensionHandler::Instance()->extGlDeleteBuffers(1, &m_VBO_ID);
			m_VBO_ID = 0;
		}

		if(m_IBO_ID)
		{
			OpenGLExtensionHandler::Instance()->extGlBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
			OpenGLExtensionHandler::Instance()->extGlDeleteBuffers(1, &m_IBO_ID);
			m_IBO_ID = 0;
		}

		m_Shader = NULL;
		m_ProgramID = -1;
	}

	//! create a copy of the object
	Object* OpenGLVertexBuffer::Copy() const
	{
		OpenGLVertexBuffer* pCopy = snew OpenGLVertexBuffer();
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

