/* 

Amine Rehioui
Created: March 21th 2010

*/

#pragma once


namespace shoot
{
	//! Image entity
	/** Ideal for still backgrounds and panels */
	class Image : public Entity2D				 
	{	
	public:

		//! Macro to provide type information
		DECLARE_OBJECT(Image, Entity2D);

		//! Constructor
		Image();

		//! serializes the entity to/from a PropertyStream
		virtual void Serialize(PropertyStream& stream);

		//! called during the initialization of the entity
		virtual void Init();

		//! updates the geometry
		void UpdateGeometry();

	private:

		//! updates the texture transform
		void UpdateTextureTransform();
	};
}

