/* 

Amine Rehioui
Created: September 4th 2012

*/

#pragma once


#include "Shader.h"

namespace shoot
{
	//! OpenGLShader class
	class OpenGLShader : public ShaderImpl
	{
	public:

		//! constructor
		OpenGLShader();

		//! destructor
		virtual ~OpenGLShader();

		//! Load into video memory
		void GraphicLoad();

		//! Unload from video memory
		void GraphicUnload();

		//! starts rendering using this shader
		void Begin();

		//! binds instance specific info
		void BeginInstance();

		//! uploads a constant to the GPU
		void UploadConstant(ShaderParameter* pParam);

		//! returns loaded status
		bool IsLoaded() const { return m_ProgramID != 0; }

		GLuint m_VertexShaderID;
		GLuint m_PixelShaderID;
		GLuint m_ProgramID;

		// shader params
		int m_WorldMatrix;
		int m_ViewProjection;
		int m_WorldViewProjection;
		int m_TextureMatrix;
		std::map<std::string, int> m_ConstantLocations;
	};
}



