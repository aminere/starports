/*

Amine Rehioui
Created: February 28th 2015

*/

#include "Shoot.h"

#include "MaterialParameter.h"

namespace shoot
{	
	DEFINE_OBJECT(MaterialParameter);
	DEFINE_OBJECT(MaterialParameterFloat);
	DEFINE_OBJECT(MaterialParameterColor);	
	DEFINE_OBJECT(MaterialParameterInt);

	//! MaterialParameter
	MaterialParameter::MaterialParameter()
		: m_Type(ShaderParameter::T_None)
	{
	}

	void MaterialParameter::Serialize(PropertyStream& stream)
	{
		stream.Serialize("Name", &m_strName);
	}

	//! MaterialParameterFloat
	MaterialParameterFloat::MaterialParameterFloat()		
		: m_Data(0.0f)
	{
		m_Type = ShaderParameter::T_Float;
	}

	void MaterialParameterFloat::Serialize(PropertyStream& stream)
	{
		super::Serialize(stream);
		stream.Serialize("Value", &m_Data);
	}

	bool MaterialParameterFloat::Equals(MaterialParameter* other) const
	{
		if (!super::Equals(other))
			return false;

		return Math::FEqual(m_Data, static_cast<MaterialParameterFloat*>(other)->m_Data);
	}

	//! MaterialParameterColor
	MaterialParameterColor::MaterialParameterColor()
		: m_Data(Color::White)
	{
		m_Type = ShaderParameter::T_Color;
	}

	void MaterialParameterColor::Serialize(PropertyStream& stream)
	{
		super::Serialize(stream);
		stream.Serialize("Value", &m_Data);
	}

	bool MaterialParameterColor::Equals(MaterialParameter* other) const
	{
		if (!super::Equals(other))
			return false;

		return m_Data == static_cast<MaterialParameterColor*>(other)->m_Data;
	}

	//! MaterialParameterInt
	MaterialParameterInt::MaterialParameterInt()
		: m_Data(0)
	{
		m_Type = ShaderParameter::T_Int;
	}

	void MaterialParameterInt::Serialize(PropertyStream& stream)
	{
		super::Serialize(stream);
		stream.Serialize("Value", &m_Data);
	}

	bool MaterialParameterInt::Equals(MaterialParameter* other) const
	{
		if (!super::Equals(other))
			return false;

		return m_Data == static_cast<MaterialParameterInt*>(other)->m_Data;
	}
}

