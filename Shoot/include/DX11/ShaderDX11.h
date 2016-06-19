/* 

Amine Rehioui
Created: July 30th 2013

*/

#pragma once

#include "Shader.h"

namespace shoot
{
	//! ShaderDX11 class
	class ShaderDX11 : public ShaderImpl
	{
	public:

		//! constructor
		ShaderDX11();

		//! Load into video memory
		void GraphicLoad();

		//! Unload from video memory
		void GraphicUnload();

		//! starts rendering using this shader
		void Begin();

		//! binds instance specific info
		void BeginInstance();

		//! sets the vertex buffer
		inline void SetVB(VertexBuffer* pVB) { m_VB = pVB; }

		//! compile HLSL code
		static ID3DBlob* CompileHLSL(const char* strSource, const char* strEntryPoint, const char* strShaderModel);

	private:

		//! returns compiled HLSL
		uchar* GetCompiledHLSL(const char* strSource, int& size);

		COMReference<ID3D11VertexShader> m_VS;
		COMReference<ID3D11PixelShader> m_PS;
		COMReference<ID3D11InputLayout> m_VertexLayout;
		Handle<VertexBuffer> m_VB;
	};
}

