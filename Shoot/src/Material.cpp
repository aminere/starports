/* 

Amine Rehioui
Created: July 11th 2010

*/

#include "Shoot.h"

#include "Material.h"

namespace shoot
{
	DEFINE_OBJECT(Material);

	// statics
	std::string Material::m_strDefaultShader = "common/shaders/tex.xml";

	//! blending factor literals
	const char* g_strBlendFactors[] =
	{
		"Zero",
		"One",		
		"SrcColor",
		"InvSrcColor",
		"SrcAlpha",
		"InvSrcAlpha",
		"DestAlpha",
		"InvDestAlpha",
		"DestColor",
		"InvDestColor",
		"SrcAlphaSaturate",
		0
	};

	//! cull mode literals
	const char* g_strCullModes[] =
	{
		"CM_ClockWise",
		"CM_CounterClockWise",
		"CM_None",
		0
	};

	//! constructor
	Material::CreationInfo::CreationInfo()
	{
		Reset();
	}

	//! constructor
	Material::CreationInfo::CreationInfo(Material* owner)
		: m_Shader(NULL, owner)
	{
		Reset();
	}

	//! Reset
	void Material::CreationInfo::Reset()
	{
		m_Color = Color::White;
		m_Mask = 0;
		m_SrcBlendFactor = GraphicsDriver::BF_SrcAlpha;
		m_DestBlendFactor = GraphicsDriver::BF_InvSrcAlpha;
		m_CullMode = GraphicsDriver::CM_CounterClockWise;
		m_UseTextureRenderTarget = false;
	}

	//! Reads/Writes struct properties from/to a stream
	void Material::CreationInfo::Serialize(PropertyStream& stream)
	{
		bool bAlphaBlending = (m_Mask & MF_AlphaBlending) != 0;
		stream.Serialize("AlphaBlending", &bAlphaBlending);
		stream.Serialize("SrcBlendFactor", &m_SrcBlendFactor, ENUM_PARAMS1(g_strBlendFactors));
		stream.Serialize("DestBlendFactor", &m_DestBlendFactor, ENUM_PARAMS1(g_strBlendFactors));
		stream.Serialize("CullMode", &m_CullMode, ENUM_PARAMS1(g_strCullModes));
		stream.Serialize("Color", &m_Color);
		stream.Serialize("Alpha", &m_Color.A);
		stream.Serialize("Textures", &m_Textures);
		stream.Serialize("Shader", &m_Shader);
		stream.Serialize("UseTextureRenderTarget", &m_UseTextureRenderTarget);
		stream.Serialize("ShaderParams", &m_MaterialParams);

		if(stream.GetMode() != SM_Read)
			return;

		SetFlag(MF_AlphaBlending, bAlphaBlending);
	}

	//! Initializes a material
	void Material::CreationInfo::Init(Material* pMaterial)
	{
		pMaterial->SetColor(m_Color);

		#ifdef SHOOT_EDITOR
			for (auto& textureRef : m_Textures)
				textureRef->SetOwner(pMaterial);
		#endif

		// force default shader if none selected
		if(!m_Shader.IsValid())
			m_Shader = ResourceManager::Instance()->GetResource<Shader>(m_strDefaultShader);
	}

	//! comparison operator
	bool Material::CreationInfo::operator == (const CreationInfo& other) const
	{
		bool bMatch = m_Mask == other.m_Mask
					&& m_Color == other.m_Color
					&& m_SrcBlendFactor == other.m_SrcBlendFactor
					&& m_DestBlendFactor == other.m_DestBlendFactor
					&& m_Textures == other.m_Textures
					&& m_CullMode == other.m_CullMode
					&& m_UseTextureRenderTarget == other.m_UseTextureRenderTarget;
		
		bool bShaderMatch = false;
		bool bShaderParamMatch = true;
		if (bMatch)
		{
			if (!m_Shader.IsValid() && !other.m_Shader.IsValid())
				bShaderMatch = true;
			else if (m_Shader.IsValid() && other.m_Shader.IsValid())
				bShaderMatch = (other.m_Shader == m_Shader);
			else if (m_Shader.IsValid())
				bShaderMatch = (m_Shader->GetTemplatePath() == m_strDefaultShader);
			else
				bShaderMatch = (other.m_Shader->GetTemplatePath() == m_strDefaultShader);

			if (bShaderMatch)
			{
				if (m_MaterialParams.GetSize() != other.m_MaterialParams.GetSize())
					bShaderParamMatch = false;
				else
				{
					for (size_t i = 0; i < m_MaterialParams.GetSize(); ++i)
					{
						if (!m_MaterialParams[i]->Equals(other.m_MaterialParams[i]))
						{
							bShaderParamMatch = false;
							break;
						}
					}
				}
			}
		}

		return bMatch && bShaderMatch && bShaderParamMatch;
	}

	//! sets the shader from a path
	void Material::CreationInfo::SetShader(const std::string& path)
	{
		m_Shader = ResourceManager::Instance()->GetResource<Shader>(path);
	}

	//! adds a texture
	void Material::CreationInfo::AddTexture(const std::string& path)
	{
		m_Textures.Add(snew Reference<Texture>(ResourceManager::Instance()->GetResource<Texture>(path)));
	}

	//! adds a texture
	void Material::CreationInfo::AddTexture(Texture* texture)
	{
		m_Textures.Add(snew Reference<Texture>(texture));
	}

	//! constructor
	Material::Material()
		: m_CreationInfo(this)
		, m_Color(Color::White)
	{
	}

	//! constructor
	Material::Material(const CreationInfo& info)
		: m_CreationInfo(this)
	{
		m_CreationInfo = info;
		m_CreationInfo.Init(this);
	}

	//! Reads/Writes struct properties from/to a stream
	void Material::Serialize(PropertyStream& stream)
	{
		super::Serialize(stream);
		
		stream.Serialize("CreationInfo", &m_CreationInfo);
		
		if(stream.GetMode() != SM_Read)
			return;

		m_CreationInfo.Init(this);
	}

	//! sets a texture
	void Material::SetTexture(int index, Texture* pTexture)
	{
		if (index < (int)m_CreationInfo.m_Textures.GetSize())		
			m_CreationInfo.m_Textures[index] = pTexture;		
		else
			m_CreationInfo.m_Textures.Add(snew Reference<Texture>(pTexture, this));		
	}

	//! start rendering with this material
	void Material::Begin()
	{
		GraphicsDriver* pDriver = GraphicsDriver::Instance();		

		for (int i = 0; i<int(m_CreationInfo.m_Textures.GetSize()); ++i)
		{
			auto texture = m_CreationInfo.m_Textures[i].Get();
			if(!texture)
				continue;

			texture->Begin(i);
			auto samplerName = std::string("uTexture")+Utils::ToString(i);
			if(auto param = GetShader()->GetConstant(samplerName))
				param->Set(&i, 1);
		}

		bool alphaBlending = m_CreationInfo.GetFlag(MF_AlphaBlending);
		pDriver->SetRenderState(GraphicsDriver::RS_AlphaBlending, alphaBlending);
		if(alphaBlending)		
			pDriver->SetBlendFunc(GraphicsDriver::E_BlendFactor(m_CreationInfo.m_SrcBlendFactor), GraphicsDriver::E_BlendFactor(m_CreationInfo.m_DestBlendFactor));		

		pDriver->SetCullMode(GraphicsDriver::E_CullMode(m_CreationInfo.m_CullMode));

		// set shader constants
		for (auto& materialPram : m_CreationInfo.m_MaterialParams)
		{
			auto matParam = materialPram->Get();
			if (!matParam)
				continue;

			auto matParamName = matParam->GetName();
			const auto& shaderParams = GetShader()->GetConstants();
			ShaderParameter* param = NULL;
			for (auto& sp : shaderParams)
			{
				if ((*sp)->GetName() == matParamName)
				{
					param = (*sp).Get();
					break;
				}
			}
			if (param)
				matParam->UpdateShaderParameter(param);
		}
	}
}

