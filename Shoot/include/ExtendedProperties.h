/* 

Amine Rehioui
Created: April 9th 2011

*/

#pragma once


namespace shoot
{
	extern const char* g_strESPTypes[];

	//! extended spatial property
	template <class T>
	struct ESP : public ISerializableStruct
	{		
		//! ESP types
		enum E_Type
		{
			T_Local,
			T_Offset
		};

		//! constructor
		ESP() : Type(T_Local)			
		{
			memset(&Value, 0, sizeof(T));
		}

		//! serialize the ESP
		void Serialize(PropertyStream& stream)
		{
			stream.Serialize("Value", &Value);
			stream.Serialize("Type", &Type, ENUM_PARAMS1(g_strESPTypes));
		}

		//! Computes src and dest properties from source and dest extended properties
		template <class EntityType>
		static void Compute(const ESP<T>& espSrc,
							const ESP<T>& espDest,
							T& src,
							T& dest,
							EntityType* pEntity,
							const T& (EntityType::*GetValue)(void) const)
		{
			switch(espSrc.Type)
			{
			case T_Offset:
				src = (pEntity->*GetValue)() + espSrc.Value;
				break;

			case T_Local:
			default:
				src = espSrc.Value;
			}

			switch(espDest.Type)
			{
			case T_Offset:
				dest = src + espDest.Value;
				break;

			case T_Local:
			default:
				dest = espDest.Value;
			}
		}

		T Value;
		E_Type Type;
	};		
}



