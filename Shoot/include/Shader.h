/* 

Amine Rehioui
Created: September 4th 2012

*/

#pragma once


#include "ShaderParameter.h"

#include "Utils.h"

namespace shoot
{
	class Shader;

	//! ShaderImpl interface
	class ShaderImpl : public GraphicObject
	{
	public:

		//! constructor
		ShaderImpl()
			: m_pParent(NULL)
		{
		}

		//! destructor
		virtual ~ShaderImpl()
		{
		}

		//! starts rendering using this shader
		virtual void Begin() = 0;

		//! binds instance specific info
		virtual void BeginInstance() = 0;

		//! sets the parent shader container
		inline void SetParent(const Shader* pParent) { m_pParent = pParent; }

		//! uploads a constant to the GPU
		virtual void UploadConstant(ShaderParameter* pParam) = 0;

	protected:

		const Shader* m_pParent;
	};

	//! NULL ShaderImpl
	class NULLShaderImpl : public ShaderImpl
	{
		//! starts rendering using this shader
		void Begin() { }

		//! binds instance specific info
		void BeginInstance() { }

		//! uploads a constant to the GPU
		void UploadConstant(ShaderParameter* pParam) { }
	};

	//! base Shader class
	class Shader : public Resource
	{
		DECLARE_OBJECT(Shader, Resource);

	public:

		//! Shader type
		enum E_Type
		{
			T_Custom,
			T_Lighting,
			T_ShadowMapping,
			T_Fog,
			T_Fire,
			T_PostEffect,
			T_LightAndFog,
		};

		//! constructor
		Shader();

		//! destructor
		virtual ~Shader();

		//! serializes the object to/from a PropertyStream
		void Serialize(PropertyStream& stream);

		//! starts rendering using this shader
		inline void Begin()
		{
			m_pImpl->Begin();
		}

		//! binds instance specific info
		inline void BeginInstance()
		{
			m_pImpl->BeginInstance();
		}

		//! Unload disk data
		void ResourceUninit() {	m_pImpl->GraphicUnload(); }

		//! returns true if this resource is using the specified data
		bool IsUsingData(const char* strPath) const
		{
			return Utils::Equals(GetVSSource().c_str(), strPath) || Utils::Equals(GetPSSource().c_str(), strPath);
		}

		//! return the ShaderImpl
		ShaderImpl* GetImpl() { return m_pImpl; }

		//! returns the VS source path
		std::string GetVSSource() const;

		//! returns the PS source path
		std::string GetPSSource() const;

		//! returns the constants
		const Array< Reference<ShaderParameter> >& GetConstants() const { return m_Constants; }

		//! returns the attributes
		const Array<std::string>& GetAttributes() const { return m_Attributes; }

		//! returns a constant
		ShaderParameter* GetConstant(const std::string& name);

		//! returns the type
		inline E_Type GetType() const { return m_eType; }

		//! sets a constant
		template <class T>
		void UploadConstant(const char* strName, const T* pData, int count)
		{
			ShaderParameter* pConstant = GetConstant(strName);
			pConstant->Set(pData, count);
			m_pImpl->UploadConstant(pConstant);
		}

	protected:

		// properties
		Array< Reference<ShaderParameter> > m_Constants;
		Array<std::string> m_Attributes;
		E_Type m_eType;

		ShaderImpl* m_pImpl;
	};
}



