/* 

Amine Rehioui
Created: July 28th 2013

*/

#include "Shoot.h"

#include "VertexBufferDX11.h"
#include "DirectX11Driver.h"
#include "ShaderDX11.h"

namespace shoot
{
	//! constructor
	VertexBufferDX11::VertexBufferDX11()
		: m_AllocatedVerticesDX11(0)
	{
	}

	//! destructor
	VertexBufferDX11::~VertexBufferDX11()
	{
		GraphicUnload();
	}

	//! Load into video memory
	void VertexBufferDX11::GraphicLoad()
	{
		SHOOT_ASSERT(m_VB.Get() == NULL, "VertexBuffer already loaded");
		SHOOT_ASSERT(m_IB.Get() == NULL, "IndexBuffer already loaded");
		
		m_VB = CreateVertexBuffer();

		if(m_pIndices)
		{
			D3D11_BUFFER_DESC bd;
			ZeroMemory(&bd, sizeof(bd));
			bd.Usage = D3D11_USAGE_DEFAULT;
			bd.ByteWidth = sizeof(ushort) * m_NumIndices;
			bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
			bd.CPUAccessFlags = 0;
			D3D11_SUBRESOURCE_DATA InitData;
			ZeroMemory( &InitData, sizeof(InitData) );
			InitData.pSysMem = m_pIndices;
			ID3D11Buffer* pIB = NULL;
			DX_ASSERT(DirectX11Driver::Instance()->GetDevice()->CreateBuffer(&bd, &InitData, &pIB));
			m_IB = pIB;
		}

		if(!m_bDynamic)
		{
#ifndef SHOOT_EDITOR // keep the vertices when in the editor, needed for triangle picking
			sdelete_array(m_pVertices);
			sdelete_array(m_pIndices);
#endif // SHOOT_EDITOR
		}
	}

	//! updates the vertex buffer data
	void VertexBufferDX11::UpdateData()
	{
		if(m_pVertices && m_VB.Get())
		{
			if(m_MaxVertices > m_AllocatedVerticesDX11)
			{
				m_VB = CreateVertexBuffer();
			}
			else
			{
				D3D11_MAPPED_SUBRESOURCE resource = {0};
				DX_ASSERT(DirectX11Driver::Instance()->GetContext()->Map(m_VB, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource));
				memcpy(resource.pData, m_pVertices, m_MaxVertices*sizeof(Vertex3D));
				DirectX11Driver::Instance()->GetContext()->Unmap(m_VB, 0);
				m_bDirty = false;
			}
		}
	}

	//! creates a vertex buffer
	ID3D11Buffer* VertexBufferDX11::CreateVertexBuffer()
	{
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = m_bDynamic ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;
		m_AllocatedVerticesDX11 = m_MaxVertices;
		bd.ByteWidth = sizeof(Vertex3D) * m_AllocatedVerticesDX11;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = m_bDynamic ? D3D11_CPU_ACCESS_WRITE : 0;
		D3D11_SUBRESOURCE_DATA InitData;
		ZeroMemory( &InitData, sizeof(InitData) );
		InitData.pSysMem = m_pVertices;
		ID3D11Buffer* pVB = NULL;
		DX_ASSERT(DirectX11Driver::Instance()->GetDevice()->CreateBuffer(&bd, &InitData, &pVB));
		m_bDirty = false;
		return pVB;
	}

	//! begins rendering using this vertex buffer
	void VertexBufferDX11::Begin(Shader* pShader)
	{
		VertexBuffer::Begin(pShader);

		static_cast<ShaderDX11*>(pShader->GetImpl())->SetVB(this);
		pShader->BeginInstance();

		if(m_VB.Get() == NULL)
		{
			GraphicLoad();
		}

		int stride = m_VertexSize;
		int offset = 0;
		ID3D11Buffer* pVB = m_VB;
		DirectX11Driver::Instance()->GetContext()->IASetVertexBuffers(0, 1, &pVB, &stride, &offset);
		m_VB = pVB;

		if(m_IB.Get())
		{
			DirectX11Driver::Instance()->GetContext()->IASetIndexBuffer(m_IB, DXGI_FORMAT_R16_UINT, 0);
		}
	}

	//! render method
	void VertexBufferDX11::Draw(const std::vector<Matrix44>& aWorldTransforms, const std::vector<Matrix44>& aTextureTransforms)
	{
		static D3D_PRIMITIVE_TOPOLOGY topologies[] = {
			D3D11_PRIMITIVE_TOPOLOGY_POINTLIST,			
			D3D11_PRIMITIVE_TOPOLOGY_LINELIST,
			D3D11_PRIMITIVE_TOPOLOGY_LINELIST, // TODO GL_LINE_LOOP
			D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP,
			D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
			D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP,
			D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ // TODO GL_TRIANGLE_FAN
		};

		ID3D11DeviceContext1* pContext = DirectX11Driver::Instance()->GetContext();
		pContext->IASetPrimitiveTopology(topologies[m_ePrimitiveType]);
			
		//GraphicsDriver::Instance()->SetColor(m_Material->GetColor()); // TODO DX11

		if(m_bApplyWorldTransforms)
		{
			for (size_t i=0; i<aWorldTransforms.size(); ++i)
			{
				GraphicsDriver::Instance()->SetTransform(GraphicsDriver::TS_World, aWorldTransforms[i]);
				GraphicsDriver::Instance()->SetTransform(GraphicsDriver::TS_Texture, aTextureTransforms[i]);

				if(m_IB.Get())
				{
					DirectX11Driver::Instance()->DrawIndexed(m_NumIndices, 0, 0);
				}
				else
				{
					DirectX11Driver::Instance()->Draw(m_NumVertices, 0);
				}

				EntityRenderer::Instance()->GetStats().Increment(this);
			}
		}
		else
		{
			GraphicsDriver::Instance()->SetTransform(GraphicsDriver::TS_World, Matrix44::Identity);
			GraphicsDriver::Instance()->SetTransform(GraphicsDriver::TS_Texture, Matrix44::Identity);

			if(m_IB.Get())
			{
				DirectX11Driver::Instance()->DrawIndexed(m_NumIndices, 0, 0);
			}
			else
			{
				DirectX11Driver::Instance()->Draw(m_NumVertices, 0);
			}

			EntityRenderer::Instance()->GetStats().Increment(this);
		}
	}

	//! Unload from video memory
	void VertexBufferDX11::GraphicUnload()
	{
		m_VB = NULL;
		m_IB = NULL;
	}

	//! create a copy of the object
	Object* VertexBufferDX11::Copy() const
	{
		VertexBufferDX11* pCopy = snew VertexBufferDX11();
		pCopy->m_Format = m_Format;
		pCopy->m_ePrimitiveType = m_ePrimitiveType;
		pCopy->m_bDynamic = m_bDynamic;
		pCopy->m_bApplyWorldTransforms = m_bApplyWorldTransforms;
		return pCopy;
	}
}

