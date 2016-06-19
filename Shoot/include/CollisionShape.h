/* 

Amine Rehioui
Created: December 28th 2010

*/

#pragma once


namespace shoot
{
	//! CollisionShapeTransform
	class CollisionShapeTransform
	{
	public:

		//! constructor
		CollisionShapeTransform(const Matrix44& vMatrix, const Vector3& vScale)
			: m_vMatrix(vMatrix)
			, m_vScale(vScale)
		{
		}

		inline const Matrix44& GetMatrix() const {return m_vMatrix; } 
		inline const Vector3& GetScale() const {return m_vScale; } 

	private:

		Matrix44 m_vMatrix;
		Vector3 m_vScale;
	};

	//! Base collision shape class
	class CollisionShape : public Object
	{
		DECLARE_OBJECT(CollisionShape, Object);

	public:

		//! collision information
		struct CollisionInfo
		{
			CollisionInfo()
				: pShape1(NULL)
				, pShape2(NULL)
				, vIntersection(Vector3::Zero)
			{
			}

			CollisionShape* pShape1;
			CollisionShape* pShape2;
			Vector3 vIntersection;
		};		

		//! collision test
		virtual bool CollidesWith(CollisionShape* pOther, const CollisionShapeTransform& transform, const CollisionShapeTransform& otherTransform, CollisionInfo& info) { return false; }

	protected:

		void InitCollisionInfo(CollisionShape* pOther, CollisionInfo& info)
		{
			info.pShape1 = this;
			info.pShape2 = pOther;
		}
	};

	//! CollisionShapeBox
	class CollisionShapeBox : public CollisionShape
	{
		DECLARE_OBJECT(CollisionShapeBox, CollisionShape);

	public:

		//! constructor
		CollisionShapeBox();

		//! Reads/Writes struct properties from/to a stream
		void Serialize(PropertyStream& stream);

		//! returns the box
		const AABBox3D& GetBox() const { return m_Box; }

		//! sets the rigid body box
		void SetBox(const AABBox3D& box) { m_Box = box; }

		//! collision test
		bool CollidesWith(CollisionShape* pOther, const CollisionShapeTransform& transform, const CollisionShapeTransform& otherTransform, CollisionInfo& info);

	private:

		AABBox3D m_Box;
	};

	//! CollisionShapeSphere
	class CollisionShapeSphere : public CollisionShape
	{
		DECLARE_OBJECT(CollisionShapeSphere, CollisionShape);

	public:

		//! constructor
		CollisionShapeSphere();

		//! Reads/Writes struct properties from/to a stream
		void Serialize(PropertyStream& stream);

		//! returns the radius
		float GetRadius() const { return m_fRadius; }

		//! returns the center
		const Vector3& GetCenter() const { return m_vCenter; }

		//! returns the transformed center
		Vector3 GetCenter(const Matrix44& matrix) const;

		//! collision test
		bool CollidesWith(CollisionShape* pOther, const CollisionShapeTransform& transform, const CollisionShapeTransform& otherTransform, CollisionInfo& info);

	private:

		float m_fRadius;
		Vector3 m_vCenter;
	};

}



