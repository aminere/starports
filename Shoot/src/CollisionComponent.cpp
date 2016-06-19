/*	

Amine Rehioui
Created: January 14th 2012

*/

#include "Shoot.h"

#include "CollisionComponent.h"

#include "CollisionManager.h"

namespace shoot
{
	DEFINE_OBJECT(CollisionComponent);

	// collision group literals
	const char* const g_strCollisionGroupLiterals[] =
	{
		"Group_1",
		"Group_2",
		"Group_3",
		"Group_4",
		"Group_5",
		"Group_6",
		"Group_7",
		"Group_8",
		0
	};

	// collision group value
	const int g_iCollisionGroupValues[] =
	{
		CollisionComponent::Group_1,
		CollisionComponent::Group_2,
		CollisionComponent::Group_3,
		CollisionComponent::Group_4,
		CollisionComponent::Group_5,
		CollisionComponent::Group_6,
		CollisionComponent::Group_7,
		CollisionComponent::Group_8,
		0
	};

	//! constructor
	CollisionComponent::CollisionComponent()
		: m_bEnabled(true)
		// properties
		, m_eGroup(Group_1)
		, m_Priority(0)
		, m_fCollisionRadius(0.5f)
	{
	}

	//! destructor
	CollisionComponent::~CollisionComponent()
	{		
	}

	//! Reads/Writes struct properties from/to a stream
	void CollisionComponent::Serialize(PropertyStream& stream)
	{
		super::Serialize(stream);

		stream.Serialize("CollisionGroup", &m_eGroup, ENUM_PARAMS2(g_strCollisionGroupLiterals, g_iCollisionGroupValues));
		stream.Serialize("CollisionMask", &m_Mask);
		stream.Serialize("Priority", &m_Priority);
		stream.Serialize("CollisionRadius", &m_fCollisionRadius);
	}

	//! Initializes the component
	void CollisionComponent::Init()
	{
		super::Init();

		if(Context* pRoot = GetOwner()->GetRootContext())
		{
			pRoot->GetCollisionManager()->Add(this);
		}
	}
	
	//! Reads/Writes struct properties from/to a stream
	void CollisionComponent::Mask::Serialize(PropertyStream& stream)
	{
		bool bReadMode = (stream.GetMode() == SM_Read);

#define SERIALIZE_GROUP(x) \
		bool bGroup_##x = (m_Mask & Group_##x) != 0; \
		stream.Serialize("Group_"#x, &bGroup_##x); \
		if(bReadMode) Set(Group_##x, bGroup_##x);

		SERIALIZE_GROUP(1);
		SERIALIZE_GROUP(2);
		SERIALIZE_GROUP(3);
		SERIALIZE_GROUP(4);
		SERIALIZE_GROUP(5);
		SERIALIZE_GROUP(6);
		SERIALIZE_GROUP(7);
		SERIALIZE_GROUP(8);
	}
}

