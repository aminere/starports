/* 

Amine Rehioui
Created: December 10th 2010

*/

#include "Shoot.h"

#include "VertexBuffer.h"

namespace shoot
{
	DEFINE_OBJECT(VertexBuffer);

	//! constructor
	VertexBuffer::VertexBuffer()
		: m_pVertices(NULL)
		, m_MaxVertices(0)
		, m_NumVertices(0)
		, m_StartVertex(0)
		, m_AllocatedVertices(0)
		, m_pIndices(NULL)
		, m_MaxIndices(0)
		, m_NumIndices(0)
		, m_Format(0)
		, m_ePrimitiveType(GraphicsDriver::PT_Triangle)
		, m_bDynamic(false)
		, m_bDirty(false)
		, m_bApplyWorldTransforms(true)
		, m_VertexSize(0)
		, _sizeOfIndex(2)
	{
	}

	//! destructor
	VertexBuffer::~VertexBuffer()
	{
		DeleteVertices();
		DeleteIndices();
	}

	//! begins rendering using this vertex buffer
	void VertexBuffer::Begin(Shader* pShader)
	{
		m_Shader = pShader;

		if(m_bDirty)
			UpdateData();
	}

	//! sets the pointers to the vertices
	void VertexBuffer::SetVertices(Vertex3D* pVertices, uint numVertices, uint format)
	{
		uchar* _pVertices = reinterpret_cast<uchar*>(pVertices);
		if(m_pVertices == _pVertices)
			return;

		DeleteVertices();
		m_pVertices = _pVertices;
		m_Format = format;
		m_MaxVertices = m_NumVertices = m_AllocatedVertices = numVertices;			
		m_VertexSize = sizeof(Vertex3D);
		m_aAttribInfos.clear();
		if(format & Vertex3D::VF_Pos)
		{
			AttributeInfo info = { "aPosition", offsetof(Vertex3D, Pos), ADT_Float, 3 };
			m_aAttribInfos.push_back(info);
		}
		if(format & Vertex3D::VF_UV)
		{
			AttributeInfo info = { "aUV", offsetof(Vertex3D, UV), ADT_Float, 2 };
			m_aAttribInfos.push_back(info);
		}
		if(format & Vertex3D::VF_Normal)
		{
			AttributeInfo info = { "aNormal", offsetof(Vertex3D, Normal), ADT_Float, 3 };
			m_aAttribInfos.push_back(info);
		}
		if(format & Vertex3D::VF_Color)
		{
			AttributeInfo info = { "aColor", offsetof(Vertex3D, color), ADT_Float, 4 };
			m_aAttribInfos.push_back(info);
		}
		if(format & Vertex3D::VF_TangentBinormal)
		{
			AttributeInfo tangent = { "aTangent", offsetof(Vertex3D, Tangent), ADT_Float, 4 };
			m_aAttribInfos.push_back(tangent);
		}
		m_bDirty = true;
	}

	//! sets the pointers to the vertices
	void VertexBuffer::SetVertices(uchar* pVertices, uint numVertices, uint vertexSize, std::vector<AttributeInfo>& attribInfos)
	{
		if (pVertices == m_pVertices)
			return;
		
		DeleteVertices();
		m_pVertices = pVertices;
		m_Format = 0;
		m_MaxVertices = m_NumVertices = m_AllocatedVertices = numVertices;
		m_aAttribInfos = attribInfos;
		m_VertexSize = vertexSize;
		m_bDirty = true;
	}

	//! sets the pointers to the indices
	void VertexBuffer::SetIndices(void* pIndices, uint numIndices, bool _32bitIndices /*= false*/)
	{
		if(pIndices == m_pIndices)
			return;
			
		_sizeOfIndex = _32bitIndices ? 4 : 2;
		DeleteIndices();

		m_pIndices = pIndices;
		m_MaxIndices = numIndices;
		m_NumIndices = numIndices;

		if(_32bitIndices)
			_getIndex = [this](uint index){ return ((uint*)m_pIndices)[index]; };
		else
			_getIndex = [this](uint index){ return ((ushort*)m_pIndices)[index]; };
	}

	//! deletes the vertices
	void VertexBuffer::DeleteVertices()
	{
		if(m_Format)
		{
			Vertex3D* pVertices = reinterpret_cast<Vertex3D*>(m_pVertices);
			sdelete_array(pVertices);
		}
		else
		{
			sdelete_array(m_pVertices);
		}

		m_pVertices = NULL;
	}

	//! deletes the indices
	void VertexBuffer::DeleteIndices()
	{
		if (_sizeOfIndex == 2)
		{
			auto indices = reinterpret_cast<ushort*>(m_pIndices);
			sdelete_array(indices); 
		}
		else
		{
			auto indices = reinterpret_cast<uint*>(m_pIndices);
			sdelete_array(indices);
		}
		m_pIndices = NULL;
	}

	//! returns an attribute info
	VertexBuffer::AttributeInfo* VertexBuffer::GetAttribInfo(const std::string& name)
	{
		for (size_t i=0; i<m_aAttribInfos.size(); ++i)
		{
			if(m_aAttribInfos[i].m_Name == name)
				return &m_aAttribInfos[i];
		}
		return NULL;
	}

	//! render helper
	void VertexBuffer::Draw()
	{
		std::vector<Matrix44> aTransforms;
		aTransforms.push_back(GraphicsDriver::Instance()->GetTransform(GraphicsDriver::TS_World));
		Draw(aTransforms, std::vector<Matrix44>());
	}

	//! render helper
	void VertexBuffer::Draw(Shader* pShader)
	{
		pShader->Begin();
		Begin(pShader);
		Draw();
	}
}

