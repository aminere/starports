/* 

Amine Rehioui
Created: April 21st 2010

*/

#pragma once


#include "Color.h"
#include "Texture.h"
#include "Shader.h"
#include "MaterialParameter.h"

namespace shoot
{
	//! Material
	class Material : public Object
	{
	public:
		
		DECLARE_OBJECT(Material, Object);

		//! Material flags
		enum E_Flag
		{
			MF_AlphaBlending = 0x1 << 0,
		};

		//! Material creation info
		struct CreationInfo : public ISerializableStruct
		{
			//! constructor
			CreationInfo();

			//! constructor
			CreationInfo(Material* owner);

			//! Reset
			void Reset();

			//! Reads/Writes struct properties from/to a stream
			void Serialize(PropertyStream& stream);

			//! Initializes a material
			void Init(Material* pMaterial);

			//! sets a flag status
			inline void SetFlag(E_Flag eFlag, bool bEnabled) { bEnabled ? (m_Mask |= eFlag) : (m_Mask &= ~eFlag); }

			//! returns a flag status
			inline bool GetFlag(E_Flag eFlag) const { return (m_Mask & eFlag) != 0; }

			//! sets the shader from a path
			void SetShader(const std::string& path);

			//! adds a texture
			void AddTexture(const std::string& path);

			//! adds a texture
			void AddTexture(Texture* texture);

			//! comparison operator
			bool operator == (const CreationInfo& other) const;			

			Color m_Color;
			int m_Mask;
			int m_SrcBlendFactor;
			int m_DestBlendFactor;
			int m_CullMode;
			Array< Reference<Texture> > m_Textures;
			Reference<Shader> m_Shader;
			bool m_UseTextureRenderTarget;
			Array< Reference<MaterialParameter> > m_MaterialParams;
		};

		//! constructor
		Material();

		//! constructor
		Material(const CreationInfo& info);

		//! Reads/Writes struct properties from/to a stream
		virtual void Serialize(PropertyStream& stream);
		
		//! sets the color
		inline void SetColor(const Color& color) { m_Color = color; }

		//! returns the color
		inline const Color& GetColor() const { return m_Color; }

		//! returns a texture
		virtual Texture* GetTexture(size_t index = 0) { return (index < m_CreationInfo.m_Textures.GetSize()) ? m_CreationInfo.m_Textures[index].Get() : NULL; }

		//! returns the textures
		inline const Array< Reference<Texture> >& GetTextures() const { return m_CreationInfo.m_Textures; }
		
		//! sets a texture
		void SetTexture(int index, Texture* pTexture);

		//! sets the shader from a path
		inline void SetShader(const std::string& path) { m_CreationInfo.SetShader(path); }

		//! sets the shader
		inline void SetShader(Shader* pShader) { m_CreationInfo.m_Shader = pShader; }

		//! returns the shader
		inline Shader* GetShader() const { return m_CreationInfo.m_Shader.Get(); }

		//! sets a flag status
		inline void SetFlag(E_Flag eFlag, bool bEnabled) { m_CreationInfo.SetFlag(eFlag, bEnabled); }

		//! sets the cull mode
		inline void SetCullMode(int cullMode) { m_CreationInfo.m_CullMode = cullMode; }

		//! returns the creation info
		inline const CreationInfo& GetCreationInfo() const { return m_CreationInfo; }

		//! UsesTextureRenderTarget
		inline bool UsesTextureRenderTarget() const { return m_CreationInfo.m_UseTextureRenderTarget; }

		//! start rendering with this material
		virtual void Begin();

		static std::string m_strDefaultShader;

	private:

		Color m_Color;

		// properties
		CreationInfo m_CreationInfo;
	};
}



