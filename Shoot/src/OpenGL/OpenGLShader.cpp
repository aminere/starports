/* 

Amine Rehioui
Created: September 4th 2012

*/

#include "Shoot.h"

#include "OpenGLShader.h"

#include "OpenGLExtensionHandler.h"

namespace shoot
{
	//! constructor
	OpenGLShader::OpenGLShader()
		: m_VertexShaderID(0)
		, m_PixelShaderID(0)
		, m_ProgramID(0)
		, m_WorldMatrix(-1)
		, m_ViewProjection(-1)
		, m_WorldViewProjection(-1)
		, m_TextureMatrix(-1)
	{
	}

	//! destructor
	OpenGLShader::~OpenGLShader()
	{
		GraphicUnload();
	}

	//! Load into video memory
	void OpenGLShader::GraphicLoad()
	{
		SHOOT_ASSERT(m_ProgramID == 0, "Shader already loaded");
		if(!m_pParent->GetVSSource().empty())
		{
			OpenGLExtensionHandler::Instance()->extGlCreateShader(OpenGLExtensionHandler::ST_Vertex, &m_VertexShaderID);
			const char* vertexShaderSource = Utils::ReadFileText(m_pParent->GetVSSource().c_str());
			OpenGLExtensionHandler::Instance()->extGlShaderSource(m_VertexShaderID, 1, &vertexShaderSource, NULL);
			OpenGLExtensionHandler::Instance()->extGlCompileShader(m_VertexShaderID);
			OpenGLExtensionHandler::Instance()->extGlCheckCompileStatus(m_VertexShaderID, m_pParent->GetVSSource());
			sdelete_array(vertexShaderSource);
		}

		if(!m_pParent->GetPSSource().empty())
		{
			OpenGLExtensionHandler::Instance()->extGlCreateShader(OpenGLExtensionHandler::ST_Fragment, &m_PixelShaderID);
			const char* pixelShaderSource = Utils::ReadFileText(m_pParent->GetPSSource().c_str());
			OpenGLExtensionHandler::Instance()->extGlShaderSource(m_PixelShaderID, 1, &pixelShaderSource, NULL);
			OpenGLExtensionHandler::Instance()->extGlCompileShader(m_PixelShaderID);
			OpenGLExtensionHandler::Instance()->extGlCheckCompileStatus(m_PixelShaderID, m_pParent->GetPSSource());
			sdelete_array(pixelShaderSource);
		}

		if(m_VertexShaderID && m_PixelShaderID)
		{
			OpenGLExtensionHandler::Instance()->extGlCreateProgram(&m_ProgramID);
			OpenGLExtensionHandler::Instance()->extGlAttachShader(m_ProgramID, m_VertexShaderID);
			OpenGLExtensionHandler::Instance()->extGlAttachShader(m_ProgramID, m_PixelShaderID);
			OpenGLExtensionHandler::Instance()->extGlLinkProgram(m_ProgramID);
			OpenGLExtensionHandler::Instance()->extGlCheckLinkStatus(m_ProgramID, m_pParent->GetTemplatePath());
		}

		OpenGLExtensionHandler::Instance()->extGlUseProgram(m_ProgramID);
		OpenGLExtensionHandler::Instance()->extGlGetUniformLocation(m_ProgramID, "uWorldMatrix", &m_WorldMatrix);
		OpenGLExtensionHandler::Instance()->extGlGetUniformLocation(m_ProgramID, "uViewProjection", &m_ViewProjection);
		OpenGLExtensionHandler::Instance()->extGlGetUniformLocation(m_ProgramID, "uWorldViewProjection", &m_WorldViewProjection);
		OpenGLExtensionHandler::Instance()->extGlGetUniformLocation(m_ProgramID, "uTextureMatrix", &m_TextureMatrix);
		Log.Print("[%s] world(%d), vp(%d), wvp(%d), tex(%d)\n", m_pParent->GetTemplatePath().c_str(), m_WorldMatrix, m_ViewProjection, m_WorldViewProjection, m_TextureMatrix);		

		// get constants locations
		m_ConstantLocations.clear();
		for (size_t i = 0; i<m_pParent->GetConstants().GetSize(); ++i)
		{
			if(ShaderParameter* pParameter = m_pParent->GetConstants()[i].Get())
			{
				int constantLocation = -1;
				OpenGLExtensionHandler::Instance()->extGlGetUniformLocation(m_ProgramID, pParameter->GetName().c_str(), &constantLocation);
				SHOOT_WARNING(constantLocation >= 0, "Shader '%s' constant '%s' not found", m_pParent->GetTemplatePath().c_str(), pParameter->GetName().c_str());								
				m_ConstantLocations[pParameter->GetName()] = constantLocation;
			}
		}
		OpenGLExtensionHandler::Instance()->extGlUseProgram(0);
	}

	//! Unload from video memory
	void OpenGLShader::GraphicUnload()
	{
		if(m_ProgramID != 0)
		{
			OpenGLExtensionHandler::Instance()->extGlUseProgram(0);
			OpenGLExtensionHandler::Instance()->extGlDeleteShader(m_VertexShaderID);
			OpenGLExtensionHandler::Instance()->extGlDeleteShader(m_PixelShaderID);
			OpenGLExtensionHandler::Instance()->extGlDeleteProgram(m_ProgramID);

			m_VertexShaderID = 0;
			m_PixelShaderID = 0;
			m_ProgramID = 0;
		}		
	}	

	//! starts rendering using this shader
	void OpenGLShader::Begin()
	{
		if(m_ProgramID == 0)
		{
			GraphicLoad();
		}

		OpenGLExtensionHandler::Instance()->extGlUseProgram(m_ProgramID);

		if(m_ViewProjection >= 0)
		{
			const Matrix44& viewProjection = GraphicsDriver::Instance()->GetTransform(GraphicsDriver::TS_ViewProjection);
			OpenGLExtensionHandler::Instance()->extGlUniformMatrix4fv(m_ViewProjection, 1, false, viewProjection.GetFloats());
		}

		// set constants
		for (size_t i = 0; i<m_pParent->GetConstants().GetSize(); ++i)
		{
			if(ShaderParameter* pParameter = m_pParent->GetConstants()[i].Get())
			{
				UploadConstant(pParameter);
			}
		}
	}

	//! binds instance specific info
	void OpenGLShader::BeginInstance()
	{
		// TODO set instance constants
	}

	//! uploads a constant to the GPU
	void OpenGLShader::UploadConstant(ShaderParameter* pParam)
	{
		int constantLocation = m_ConstantLocations[pParam->GetName()];
		if(constantLocation >= 0)
		{
			switch(pParam->GetType())
			{
			case ShaderParameter::T_Float:
				{
					int count = 0;
					const float* f = static_cast<const float*>(pParam->Get(count));
					if(count)
					{
						OpenGLExtensionHandler::Instance()->extGlUniform1fv(constantLocation, count, f);
					}
				}
				break;

			case ShaderParameter::T_Vec4:
			case ShaderParameter::T_Color:
				{
					int count = 0;
					const float* f = static_cast<const float*>(pParam->Get(count));
					if(count)
					{
						OpenGLExtensionHandler::Instance()->extGlUniform4fv(constantLocation, count, f);
					}
				}
				break;

			case ShaderParameter::T_Mat44:
				{
					int count = 0;
					const float* f = static_cast<const float*>(pParam->Get(count));
					if(count)
					{
						OpenGLExtensionHandler::Instance()->extGlUniformMatrix4fv(constantLocation, count, false, f);
					}
				}
				break;

			case ShaderParameter::T_Int:
				{
					int count = 0;
					const int* i = static_cast<const int*>(pParam->Get(count));
					if(count)
					{
						OpenGLExtensionHandler::Instance()->extGlUniform1iv(constantLocation, count, i);
					}
				}
				break;

			case ShaderParameter::T_Vec2:
				{
					int count = 0;
					const float* f = static_cast<const float*>(pParam->Get(count));
					if(count)
					{
						OpenGLExtensionHandler::Instance()->extGlUniform2fv(constantLocation, count, f);
					}
				}
				break;

			case ShaderParameter::T_Vec3:
				{
					int count = 0;
					const float* f = static_cast<const float*>(pParam->Get(count));
					if(count)
					{
						OpenGLExtensionHandler::Instance()->extGlUniform3fv(constantLocation, count, f);
					}
				}
				break;
			}
		}
	}
}


