/* 

Amine Rehioui
Created: July 9th 2012

*/

#pragma once


#include "CollisionShape.h"

namespace shoot
{
	//! Collision Component class
	class CollisionComponent : public Component
	{
		DECLARE_OBJECT(CollisionComponent, Component);

	public:		

		static const int NumCollisionGroups = 8;

		//! collision groups
		enum E_Group
		{
			Group_1 = 0x1 << 0,
			Group_2 = 0x1 << 1,
			Group_3 = 0x1 << 2,
			Group_4 = 0x1 << 3,
			Group_5 = 0x1 << 4,
			Group_6 = 0x1 << 5,
			Group_7 = 0x1 << 6,
			Group_8 = 0x1 << 7
		};

		//! constructor
		CollisionComponent();

		//! destructor
		virtual ~CollisionComponent();

		//! Reads/Writes struct properties from/to a stream
		virtual void Serialize(PropertyStream& stream);

		//! Initializes the component
		virtual void Init();

		//! checks collision with another component
		virtual void CheckCollisions(CollisionComponent* pOther) { }

		//! called when a collision with another collision component is detected
		virtual void OnCollision(CollisionComponent* pOther, std::vector<CollisionShape::CollisionInfo>& infos) { }

		//! collision group test
		inline bool CanCollideWith(E_Group eGroup) const { return m_bEnabled && ((m_Mask.m_Mask & eGroup) != 0); }	

		//! returns the collision group
		E_Group GetGroup() const { return m_eGroup; }

		//! returns the collision check priority
		int GetPriority() const { return m_Priority; }

		//! sets the enabled status
		inline void SetEnabled(bool bEnabled) { m_bEnabled = bEnabled; }

		//! returns the enabled status
		inline bool IsEnabled() const { return m_bEnabled; }

		//! returns the collision radius
		inline float GetCollisionRadius() const { return m_fCollisionRadius; }

	private:

		//! collision mask structure
		struct Mask : public ISerializableStruct
		{
			//! constructor
			Mask() : m_Mask(0)
			{
			}

			//! Reads/Writes struct properties from/to a stream
			void Serialize(PropertyStream& stream);
			
			//! Sets rendering flags
			void Set(E_Group eGroup, bool bEnabled = true) { bEnabled ? (m_Mask |= eGroup) : m_Mask &= ~eGroup; }

			int m_Mask;
		};

		bool m_bEnabled;

	protected:

		// properties
		E_Group m_eGroup;
		Mask m_Mask;
		int m_Priority;
		float m_fCollisionRadius;
	};
}



