/* 

Amine Rehioui
Created: Feb. 9th 2015

*/

#pragma once


namespace shoot
{
	//! TrailEntity
	class TrailEntity : public Entity3D
	{
	public:

		//! Macro to provide type information
		DECLARE_OBJECT(TrailEntity, Entity3D);

		//! constructor
		TrailEntity();

		//! serializes the entity to/from a PropertyStream
		virtual void Serialize(PropertyStream& stream) override;

		//! called during the initialization of the entity
		virtual void Init() override;

		//! called during the update of the entity
		virtual void Update() override;

	protected:

		std::vector<Vector3> m_Points;

		// properties
		uint m_NumEdges;
		float m_TrailDuration;
		float m_Thickness;
		float m_Treshold;
		float m_UPerSegment;
		float m_FadeFactor;
	};
}



