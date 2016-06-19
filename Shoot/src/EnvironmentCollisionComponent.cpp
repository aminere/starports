/*	

Amine Rehioui
Created: January 14th 2012

*/

#include "Shoot.h"

#include "EnvironmentCollisionComponent.h"

#include "RigidBodyComponent.h"

#include "Plane.h"

#include "Triangle.h"

#include "CollisionUtils.h"

namespace shoot
{
	DEFINE_OBJECT(EnvironmentCollisionComponent);
	
	//! constructor
	EnvironmentCollisionComponent::EnvironmentCollisionComponent()
		// properties
		: m_bDynamic(false)
	{
		m_Priority = 1;
	}

	//! destructor
	EnvironmentCollisionComponent::~EnvironmentCollisionComponent()
	{		
	}

	//! Reads/Writes struct properties from/to a stream
	void EnvironmentCollisionComponent::Serialize(PropertyStream& stream)
	{
		super::Serialize(stream);

		stream.Serialize("Mesh", &m_Mesh);
		stream.Serialize("Dynamic", &m_bDynamic);
	}

	//! Initializes the component
	void EnvironmentCollisionComponent::Init()
	{
		super::Init();

		SHOOT_ASSERT(m_pOwner->IsA(Entity3D::TypeID), "Parent not of type Entity3D");
	}

	//! checks collision with another component
	void EnvironmentCollisionComponent::CheckCollisions(CollisionComponent* pOther)
	{
		if(RigidBodyComponent* pRigidBody = DYNAMIC_CAST(pOther, RigidBodyComponent))
		{
			Vector3 vPosition = static_cast<Entity3D*>(pRigidBody->GetOwner())->GetWorldTransform().GetTranslation();
			Vector3 vIntersection;
			if(CheckCollision(vPosition, pRigidBody->GetVelocity(), pRigidBody->GetCollisionRadius(), &vIntersection))
			{
				std::vector<CollisionShape::CollisionInfo> infos;
				CollisionShape::CollisionInfo info;
				info.vIntersection = vIntersection;
				infos.push_back(info);
				pRigidBody->OnCollision(this, infos);
			}
		}
	}

	//! Checks for a collision against the environment
	bool EnvironmentCollisionComponent::CheckCollision(const Vector3& vPosition, const Vector3& vVelocity, float fRadius, Vector3* pIntersection /*= NULL*/)
	{
		auto vb = m_Mesh->GetSubMeshInfos()[0].m_VertexBuffer;
		Vertex3D* pVertices = reinterpret_cast<Vertex3D*>(vb->GetVertices());
		int numIndices = vb->GetNumIndices();
		const Matrix44& matrix = static_cast<Entity3D*>(m_pOwner)->GetWorldTransform();

		for(int i=0; i<numIndices; i += 3)
		{
			Vector3 v1 = matrix.TransformVect(pVertices[vb->GetIndex(i + 0)].Pos);
			Vector3 v2 = matrix.TransformVect(pVertices[vb->GetIndex(i + 1)].Pos);
			Vector3 v3 = matrix.TransformVect(pVertices[vb->GetIndex(i + 2)].Pos);

			Plane plane(v1, v2, v3);
			Plane::E_Classification eObjectClass = plane.ClassifyPoint(vPosition);
			if(eObjectClass == Plane::C_Front)
			{
				Vector3 vPointToCheck = vPosition - (plane.Normal*fRadius);
				float fMotionLength = vVelocity.GetLength();
				Vector3 normalizedVelocity = vVelocity / fMotionLength;
				float cosAngle = normalizedVelocity.DotProduct(plane.Normal);				
				if(cosAngle < 0.0f || m_bDynamic)
				{
					Vector3 vDestPoint = vPointToCheck + vVelocity;
					Vector3 vIntersection;
					Plane::E_Classification eCheckClass;
					bool bIntersection = plane.IntersectWithRay(vPointToCheck, normalizedVelocity, &vIntersection, &eCheckClass);
					bIntersection = bIntersection && (eCheckClass == Plane::C_Front);
					bIntersection = !bIntersection ? (m_bDynamic ? plane.IntersectWithRay(vPosition, (vPointToCheck-vPosition).Normalize(), &vIntersection) : bIntersection) : bIntersection;
					Triangle triangle(v1, v2, v3);
					bool bPointOnTriangle = bIntersection ? triangle.IsPointInside(vIntersection, 0.025f) : false;
					if(bPointOnTriangle)
					{
						Plane::E_Classification eDestClass = plane.ClassifyPoint(vDestPoint);

						// if dest point is not front-facing the plane, we have a collision
						if(eDestClass != Plane::C_Front)
						{
							#ifdef SHOOT_EDITOR
							if(m_aCollisionInfos.size() < 10)
							{
								CollisionInfo info = { vIntersection, 2.0f };
								m_aCollisionInfos.push_back(info);
							}
							#endif // SHOOT_EDITOR

							if(pIntersection)
							{
								*pIntersection = vIntersection;
							}
							return true;
						}
					}
					else
					{
						// Calculate new plane and sphere intersection points
						// Thanks to good old peroxide!! http://www.peroxide.dk/download/tutorials/tut10/pxdtut10.html
						// First time I used it was in the previous millenium and it's still rocking for fucks sake.
						Vector3 vTriangleIntersection = triangle.GetClosestPoint(vIntersection);
						float fDistToSphereIntersection = -1.0f;
						if(CollisionUtils::SphereIntersectsWithRay(vPosition, fRadius, vTriangleIntersection, -normalizedVelocity, NULL, &fDistToSphereIntersection))
						{
							if(fDistToSphereIntersection < fMotionLength)
							{
								if(pIntersection)
								{
									*pIntersection = vTriangleIntersection - normalizedVelocity*fDistToSphereIntersection;
								}
								return true;
							}
						}
					}
				}
			}
		}

		return false;
	}
}

