/* 

Amine Rehioui
Created: September 4th 2012

*/

#include "Shoot.h"

#include "ShaderParameter.h"

namespace shoot
{
	//! type literals
	const char* g_strShaderParameterTypes[] =
	{
		"T_Float",
		"T_Vec4",
		"T_Color",
		"T_Mat44",
		"T_Int",
		"T_Vec2",
		"T_Vec3,",
		0
	};

	DEFINE_OBJECT(ShaderParameter);	

	//! constructor
	ShaderParameter::ShaderParameter()
		: m_eType(T_None)
		, m_pData(NULL)
	{
	}

	//! destructor
	ShaderParameter::~ShaderParameter()
	{
		sdelete(m_pData);
	}

	//! serializes the object to/from a PropertyStream
	void ShaderParameter::Serialize(PropertyStream& stream)
	{
		stream.Serialize("Name", &m_strName);
		stream.Serialize("Type", &m_eType, ENUM_PARAMS1(g_strShaderParameterTypes));
		
		if(stream.GetMode() == SM_Read)
		{
			sdelete(m_pData);
			switch(m_eType)
			{
			case T_Float:
				m_pData = snew ShaderData<float>();
				break;

			case T_Vec4:
				m_pData = snew ShaderData<Vector4>();
				break;

			case T_Color:
				m_pData = snew ShaderData<Color>();
				break;

			case T_Mat44:
				m_pData = snew ShaderData<Matrix44>();
				break;

			case T_Int:
				m_pData = snew ShaderData<int>();
				break;

			case T_Vec2:
				m_pData = snew ShaderData<Vector2>();
				break;

			case T_Vec3:
				m_pData = snew ShaderData<Vector3>();
				break;
			}
		}
	}
}

