/*	

Amine Rehioui
Created: January 14th 2012

*/

#include "Shoot.h"

#include "RigidBodyComponent.h"

namespace shoot
{
	DEFINE_OBJECT(RigidBodyComponent);
	
	//! constructor
	RigidBodyComponent::RigidBodyComponent()
		: m_vVelocity(Vector3::Zero)
		// properties
		, m_fMass(1.0f)		
	{		
	}

	//! destructor
	RigidBodyComponent::~RigidBodyComponent()
	{		
	}

	//! Reads/Writes struct properties from/to a stream
	void RigidBodyComponent::Serialize(PropertyStream& stream)
	{
		super::Serialize(stream);

		stream.Serialize("Mass", &m_fMass);
		stream.Serialize("Shapes", &m_aShapes);
	}

	//! Initializes the component
	void RigidBodyComponent::Init()
	{
		super::Init();

		// TODO: restrict component type depending on entity, and enforce it in the editor
		SHOOT_ASSERT(m_pOwner->IsA(Entity3D::TypeID), "CollisionComponent parent must be an Entity3D");
	}

	//! checks collision with another component
	void RigidBodyComponent::CheckCollisions(CollisionComponent* pOther)
	{
		auto other = DYNAMIC_CAST(pOther, RigidBodyComponent);
		if (!other)
			return;

		std::vector<CollisionShape::CollisionInfo> infos;
		Entity3D* pOwner = static_cast<Entity3D*>(GetOwner());
		Entity3D* pOtherOwner = static_cast<Entity3D*>(pOther->GetOwner());
		CollisionShapeTransform component1Transform(pOwner->GetWorldTransform(), pOwner->GetAbsoluteScale());
		CollisionShapeTransform component2Transform(pOtherOwner->GetWorldTransform(), pOtherOwner->GetAbsoluteScale());

		for (auto &shape : GetShapes())
		{
			if (!shape->Get())
				continue;

			for (auto &otherShape : other->GetShapes())
			{
				if (!otherShape->Get())
					continue;

				CollisionShape::CollisionInfo info;
				if (shape->Get()->CollidesWith(otherShape->Get(), component1Transform, component2Transform, info))
					infos.push_back(info);
			}
		}

		if (!infos.empty())
		{
			OnCollision(pOther, infos);
			pOther->OnCollision(this, infos);
		}
	}
}

