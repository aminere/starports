/* 

Amine Rehioui
Created: May 31th 2010

*/

#pragma once


namespace shoot
{
	//! 3D BillBoard entity
	class BillBoardEntity : public Entity3D				 
	{
	public:

		//! Macro to provide type information
		DECLARE_OBJECT(BillBoardEntity, Entity3D);

		//! Constructor
		BillBoardEntity();

		//! serializes the entity to/from a PropertyStream
		virtual void Serialize(PropertyStream& stream);

		//! called during the initialization of the entity
		virtual void Init();

		//! called during the update of the entity		
		virtual void Update();

		//! returns the affine transformation matrix
		Matrix44 GetWorldTransform();

	private:
		
		Vector3 m_vPreviousScale;

		//! Resize
		void Resize(const Vector3& newScale);

		// properties
		bool m_bHasVertexColors;
	};
}



