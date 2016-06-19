/* 

Amine Rehioui
Created: September 4th 2012

*/

#include "Shoot.h"

#include "Shader.h"

namespace shoot
{
	//! type literals
	const char* g_strShaderTypes[] =
	{
		"T_Custom",
		"T_Lighting",
		"T_ShadowMapping",
		"T_Fog",
		"T_Fire",
		"T_PostEffect",
		0
	};

	DEFINE_OBJECT(Shader);

	//! constructor
	Shader::Shader()
		// properties
		: m_eType(T_Custom)
	{
		m_pImpl = GraphicsDriver::Instance()->CreateShader();
	}

	//! destructor
	Shader::~Shader()
	{
		sdelete(m_pImpl);
	}

	//! serializes the object to/from a PropertyStream
	void Shader::Serialize(PropertyStream& stream)
	{
		stream.Serialize("Type", &m_eType, ENUM_PARAMS1(g_strShaderTypes));
		stream.Serialize("Constants", &m_Constants);
		stream.Serialize("Attributes", &m_Attributes);

		if(stream.GetMode() == SM_Read)
			m_pImpl->SetParent(this);
	}

#ifndef DX11
	//! returns the VS source path
	std::string Shader::GetVSSource() const { return Utils::GetFilePathNoExt(GetTemplatePath()) + ".vs"; }

	//! returns the PS source path
	std::string Shader::GetPSSource() const { return Utils::GetFilePathNoExt(GetTemplatePath()) + ".ps"; }
#else
	//! returns the VS source path
	std::string Shader::GetVSSource() const { return Utils::GetFilePathNoExt(GetTemplatePath()) + ".vs.hlsl"; }

	//! returns the PS source path
	std::string Shader::GetPSSource() const { return Utils::GetFilePathNoExt(GetTemplatePath()) + ".ps.hlsl"; }
#endif

	//! returns a constant
	ShaderParameter* Shader::GetConstant(const std::string& name)
	{
		for (size_t i=0; i<m_Constants.GetSize(); ++i)
		{
			if(m_Constants[i]->GetName() == name)
			{
				return m_Constants[i];
			}
		}
		return NULL;
	}
}

