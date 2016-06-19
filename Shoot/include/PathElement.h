/* 

Amine Rehioui
Created: August 3rd 2011

*/

#pragma once


namespace shoot
{
	//! Path Element entity	
	class PathElement : public Entity3D				    			   
	{
	public:

		//! Macro to provide type information
		DECLARE_OBJECT(PathElement, Entity3D);

		//! Constructor
		PathElement();

		//! serializes the entity to/from a PropertyStream
		void Serialize(PropertyStream& stream);

		//! GetTime
		inline float GetTime() const { return m_fTime; }

	private:

		// properties
		float m_fTime;
	};
}



