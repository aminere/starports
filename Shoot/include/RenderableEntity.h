/* 

Amine Rehioui
Created: December 28th 2010

*/

#pragma once


namespace shoot
{
	//! renderable entity with a material
	class RenderableEntity : public Entity
	{
	public:

		DECLARE_OBJECT(RenderableEntity, Entity);

		//! registers the entity and its children for rendering
		virtual void RegisterForRendering();

		//! returns the world transformation matrix
		virtual Matrix44 GetWorldTransform()
		{
			return Matrix44::Identity;
		}
	};
}

