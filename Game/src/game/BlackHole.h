/* 

Amine Rehioui
Created: August 18th 2014

*/

#pragma once


namespace shoot
{
	//! BlackHole
	class BlackHole : public Entity3D
	{
		DECLARE_OBJECT(BlackHole, Entity3D);

	public:		

		//! constructor
		BlackHole();

		//! serializes the entity to/from a PropertyStream
		void Serialize(PropertyStream& stream);

		//! called during the initialization of the entity
		void Init();

		//! called during the update of the entity		
		void Update();

	private:

		//! HoleInfo
		struct HoleInfo
		{
			Handle<Entity3D> m_Hole;
			float m_fAngle;
		};

		std::vector<HoleInfo> m_aHoles;

		// properties
		Array<float> m_RotationDegsPerSecond;
	};
}



