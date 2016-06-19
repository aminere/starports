/* 

Amine Rehioui
Created: July 30th 2013

*/

#include "Shoot.h"

#include "ShaderDX11.h"

#include "DirectX11Driver.h"

#include "File.h"

#include <d3dcompiler.h>

namespace shoot
{
	//! constructor
	ShaderDX11::ShaderDX11()
	{
	}

	//! Load into video memory
	void ShaderDX11::GraphicLoad()
	{
		SHOOT_ASSERT(m_VS.Get() == NULL, "VertexShader already loaded");
		SHOOT_ASSERT(m_PS.Get() == NULL, "PixelShader already loaded");
		SHOOT_ASSERT(!m_pParent->GetVSSource().empty(), "VertexShader path is empty");
		SHOOT_ASSERT(!m_pParent->GetPSSource().empty(), "PixelShader path is empty");

		int VSSize = 0;
		int PSSize = 0;
		uchar* pVSData = NULL;
		uchar* pPSData = NULL;
		
#ifdef SHOOT_EDITOR
		COMReference<ID3DBlob> vsBlob = CompileHLSL(m_pParent->GetVSSource().c_str(), "VSMain", "vs_4_0_level_9_3");
		COMReference<ID3DBlob> psBlob = CompileHLSL(m_pParent->GetPSSource().c_str(), "PSMain", "ps_4_0_level_9_3");
		pVSData = static_cast<uchar*>(vsBlob->GetBufferPointer());
		pPSData = static_cast<uchar*>(psBlob->GetBufferPointer());
		VSSize = vsBlob->GetBufferSize();
		PSSize = psBlob->GetBufferSize();
#else
		pVSData = GetCompiledHLSL(m_pParent->GetVSSource().c_str(), VSSize);
		pPSData = GetCompiledHLSL(m_pParent->GetPSSource().c_str(), PSSize);
#endif

		ID3D11Device1* pDevice = DirectX11Driver::Instance()->GetDevice();
		ID3D11VertexShader* pVS = NULL;
		DX_ASSERT(pDevice->CreateVertexShader(pVSData, VSSize, NULL, &pVS));
		m_VS = pVS;

		ID3D11PixelShader* pPS = NULL;
		DX_ASSERT(pDevice->CreatePixelShader(pPSData, PSSize, NULL, &pPS));
		m_PS = pPS;

		const Array<std::string>& attribs = m_pParent->GetAttributes();
		D3D11_INPUT_ELEMENT_DESC* pLayout = new D3D11_INPUT_ELEMENT_DESC[attribs.GetSize()];
		for (size_t i=0; i<attribs.GetSize(); ++i)
		{
			VertexBuffer::AttributeInfo* pAttribInfo = m_VB->GetAttribInfo(attribs[i]);
			SHOOT_ASSERT(pAttribInfo, "VertexBuffer attribute '%s' not found", attribs[i].c_str());
			pLayout[i].SemanticName = attribs[i].c_str();
			pLayout[i].SemanticIndex = 0;
			DXGI_FORMAT dxgiFormat = DXGI_FORMAT_UNKNOWN;
			switch(pAttribInfo->m_DataType)
			{
			case VertexBuffer::ADT_Float:
				{
					switch(pAttribInfo->m_DataCount)
					{
					case 2: dxgiFormat = DXGI_FORMAT_R32G32_FLOAT; break;
					case 3: dxgiFormat = DXGI_FORMAT_R32G32B32_FLOAT; break;
					case 4: dxgiFormat = DXGI_FORMAT_R32G32B32A32_FLOAT; break;
					default: SHOOT_ASSERT(false, "Unsupported Vertex attribute format");
					}
				}
				break;

			case VertexBuffer::ADT_U32: SHOOT_ASSERT(false, "TODO");
			}
			pLayout[i].Format = dxgiFormat;
			pLayout[i].InputSlot = 0;
			pLayout[i].AlignedByteOffset = pAttribInfo->m_Offset;
			pLayout[i].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			pLayout[i].InstanceDataStepRate = 0;
		}
		
		ID3D11InputLayout* pInputLayout = NULL;
		DX_ASSERT(pDevice->CreateInputLayout(pLayout, attribs.GetSize(), pVSData, VSSize, &pInputLayout));
		m_VertexLayout = pInputLayout;
		sdelete_array(pLayout);

#ifndef SHOOT_EDITOR
		sdelete_array(pVSData);
		sdelete_array(pPSData);
#endif
	}

	//! Unload from video memory
	void ShaderDX11::GraphicUnload()
	{
		m_VS = NULL;
		m_PS = NULL;
		m_VertexLayout = NULL;
	}	

	//! starts rendering using this shader
	void ShaderDX11::Begin()
	{
		if(m_VS.Get())
		{
			ID3D11DeviceContext1* pContext = DirectX11Driver::Instance()->GetContext();
			pContext->IASetInputLayout(m_VertexLayout.Get());
			pContext->VSSetShader(m_VS.Get(), NULL, 0);
			pContext->PSSetShader(m_PS.Get(), NULL, 0);
		}

		DirectX11Driver::Instance()->BindMaterialStates();

		// TODO send global constants
	}

	//! binds instance specific info
	void ShaderDX11::BeginInstance()
	{
		if(m_VS.Get() == NULL)
		{
			GraphicLoad();
			ID3D11DeviceContext1* pContext = DirectX11Driver::Instance()->GetContext();
			pContext->IASetInputLayout(m_VertexLayout.Get());
			pContext->VSSetShader(m_VS.Get(), NULL, 0);
			pContext->PSSetShader(m_PS.Get(), NULL, 0);
		}

		// TODO send instance constants
	}

	//! compile HLSL code
	ID3DBlob* ShaderDX11::CompileHLSL(const char* strSource, const char* strEntryPoint, const char* strShaderModel)
	{
		ID3DBlob* pShaderBlob = NULL;
		COMReference<ID3DBlob> errorBlob;
		int flags = D3DCOMPILE_ENABLE_STRICTNESS;
		flags |= D3DCOMPILE_DEBUG;

		//const D3D_SHADER_MACRO defines[] = 
		//{
		//	"EXAMPLE_DEFINE", "1",
		//	NULL, NULL
		//};

		std::string source("data/");
		source = source + strSource;
		std::wstring wstr(source.begin(), source.end());
		ID3DBlob* pErrorBlob = NULL;
		HRESULT hr = D3DCompileFromFile(wstr.c_str(), NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, strEntryPoint, strShaderModel, flags, 0, &pShaderBlob, &pErrorBlob);
		errorBlob = pErrorBlob;
		if(FAILED(hr))
		{
			if(errorBlob.Get())
			{
				SHOOT_ASSERT(false, "Shader error (%s): %s", strSource, (char*)errorBlob->GetBufferPointer());
			}
			else
			{
				SHOOT_ASSERT(false, "Shader error (%s)", strSource);
			}
		}
		return pShaderBlob;
	}

	//! returns compiled HLSL
	uchar* ShaderDX11::GetCompiledHLSL(const char* strSource, int& size)
	{
		File* pFile = File::Create(strSource, File::M_ReadBinary);
		pFile->Open();
		size = pFile->GetSize();
		uchar* pData = snew uchar[size];
		pFile->Read(pData, size);
		pFile->Close();
		delete pFile;
		return pData;
	}
}


