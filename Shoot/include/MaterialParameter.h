/* 

Amine Rehioui
Created: February 28th 2015

*/

#pragma once


namespace shoot
{
	//! MaterialParameter
	class MaterialParameter : public Object
	{
		DECLARE_OBJECT(MaterialParameter, Object);

	public:

		MaterialParameter();
		virtual ~MaterialParameter()
		{
		}

		virtual void Serialize(PropertyStream& stream) override;
		virtual void UpdateShaderParameter(ShaderParameter*) { }
		virtual bool Equals(MaterialParameter* other) const { return other->m_Type == m_Type; }

	protected:

		ShaderParameter::E_Type m_Type;
	};	

	//! MaterialParameterFloat
	class MaterialParameterFloat : public MaterialParameter
	{
		DECLARE_OBJECT(MaterialParameterFloat, MaterialParameter);

	public:

		MaterialParameterFloat();
		void Serialize(PropertyStream& stream) override;
		void UpdateShaderParameter(ShaderParameter* shaderParam) override { shaderParam->Set(&m_Data, 1); }
		bool Equals(MaterialParameter* other) const override;

	private:

		float m_Data;
	};

	//! MaterialParameterColor
	class MaterialParameterColor : public MaterialParameter
	{
		DECLARE_OBJECT(MaterialParameterColor, MaterialParameter);

	public:

		MaterialParameterColor();
		void Serialize(PropertyStream& stream) override;
		void UpdateShaderParameter(ShaderParameter* shaderParam) override { shaderParam->Set(&m_Data, 1); }
		bool Equals(MaterialParameter* other) const override;

	private:

		Color m_Data;
	};

	//! MaterialParameterInt
	class MaterialParameterInt : public MaterialParameter
	{
		DECLARE_OBJECT(MaterialParameterInt, MaterialParameter);

	public:

		MaterialParameterInt();
		void Serialize(PropertyStream& stream) override;
		void UpdateShaderParameter(ShaderParameter* shaderParam) override { shaderParam->Set(&m_Data, 1); }
		bool Equals(MaterialParameter* other) const override;

	private:

		int m_Data;
	};
}



