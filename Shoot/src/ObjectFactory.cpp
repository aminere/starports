/* 

Amine Rehioui
Created: April 15th 2011

*/

#include "Shoot.h"

// Core
#include "Context.h"
#include "ContextStack.h"
#include "Material.h"
#include "InputRecorder.h"

// Events
#include "Event.h"

// Resources
#include "Texture.h"
#include "CubeMapResource.h"
#include "MeshResource.h"
#include "MemoryTexture.h"

// 2D
#include "Image.h"
#include "Sprite.h"
#include "Text.h"
#include "LayoutComponent.h"

// 3D
#include "Camera.h"
#include "MeshEntity.h"
#include "ParticleGenerator.h"
#include "BillBoardEntity.h"
#include "SkyBoxEntity.h"
#include "Path.h"
#include "PathElement.h"
#include "CameraPath.h"
#include "CameraPathElement.h"
#include "ShaderParameter.h"
#include "QuadTree.h"
#include "Light.h"
#include "Text3D.h"
#include "EffectEntity.h"
#include "TrailEntity.h"
#include "FakeCubeMapTexture.h"

// Collision
#include "CollisionShape.h"
#include "CollisionComponent.h"
#include "EnvironmentCollisionComponent.h"

// Physics
#include "RigidBodyComponent.h"

// Visitors
#include "AnimationVisitor.h"
#include "Translation2DVisitor.h"
#include "Rotation2DVisitor.h"
#include "Scale2DVisitor.h"
#include "Translation3DVisitor.h"
#include "Rotation3DVisitor.h"
#include "Scale3DVisitor.h"
#include "DelayVisitor.h"
#include "SequenceVisitor.h"
#include "CombineVisitor.h"
#include "FollowPathVisitor.h"
#include "FollowCameraPathVisitor.h"
#include "ExplosionVisitor.h"
#include "RemoveEntityVisitor.h"
#include "ColorVisitor.h"
#include "MaterialVisitor.h"
#include "FadeVisitor.h"
#include "EntityVisitor.h"
#include "ParticleVisitor.h"
#include "SendTriggerVisitor.h"
#include "Shake3DVisitor.h"
#include "CameraVisitor.h"
#include "TextVisitor.h"
#include "LoadContextVisitor.h"
#include "LightVisitor.h"
#include "TextureAnimationVisitor.h"

// UI
#include "Button.h"
#include "UISlider.h"
#include "MenuRootContext.h"

namespace shoot
{
	//! Constructor
	ObjectFactory::ObjectFactory()
	{
		RegisterDefaultObjects();
	}

	//! Destructor
	ObjectFactory::~ObjectFactory()
	{
	}

	//! creates an object from a class name
	/** \param strClassName: the object class name */
	Object* ObjectFactory::Create(const std::string& strClassName)
	{
#ifndef _RETAIL_
		if(m_CreationInfo.find(strClassName) == m_CreationInfo.end())
		{
			SHOOT_WARNING(false, "Unknown Object type '%s'", strClassName.c_str());
			return NULL;
		}
#endif
		
		return m_CreationInfo[strClassName]();
	}

	//! creates an object given an index in the creatable object table
	/** \param typeIndex: type index of the object */
	Object* ObjectFactory::Create(int typeIndex)
	{
		SHOOT_ASSERT(typeIndex < (int)GetNumCreatableTypes(), "Type index out of range");
		CreationMap::iterator it = m_CreationInfo.begin();
		std::advance(it, typeIndex);
		return it->second();
	}

	//! returns an entity type name given its index
	const std::string& ObjectFactory::GetCreatableClassName(int typeIndex)
	{
		SHOOT_ASSERT(typeIndex < (int)GetNumCreatableTypes(), "Type index out of range");
		CreationMap::iterator it = m_CreationInfo.begin();
		std::advance(it, typeIndex);
		return it->first;
	}

	//! returns the creatable child class names for a specific class name
	void ObjectFactory::GetCreatableChildClassNames(std::string className, std::vector<std::string>& childClassNames)
	{
		if(m_ClassInfo.find(className) != m_ClassInfo.end())
		{
			for (size_t i = 0; i<m_ClassInfo[className].size(); ++i)
			{
				if(m_ClassInfo[className][i] != className)				
					GetCreatableChildClassNames(m_ClassInfo[className][i], childClassNames);				
			}
		}
		
		// check if type is creatable
		if(m_CreationInfo.find(className) != m_CreationInfo.end())		
			childClassNames.push_back(className);
	}

	//! returns true if a type is a derived from another type
	bool ObjectFactory::IsDerived(std::string className, std::string parentClassName)
	{
		if(className == parentClassName)
			return true;

		std::vector<std::string> childClassNames;
		GetCreatableChildClassNames(parentClassName, childClassNames);

		for (size_t i=0; i<childClassNames.size(); ++i)
		{
			if(childClassNames[i] == className)
				return true;

			if(childClassNames[i] != parentClassName && IsDerived(className, childClassNames[i]))
				return true;
		}

		return false;
	}

	//! registers class information
	void ObjectFactory::RegisterClassInfo(std::string strClassName, std::string strParentClassName)
	{
		m_ClassInfo[strParentClassName].push_back(strClassName);
	}

	//! register default objects
	void ObjectFactory::RegisterDefaultObjects()
	{
		// Core
		Register<Object>();
		Register<RenderableEntity>();
		Register<Entity>();
		Register<Context>();
		Register<ContextStack>();
		Register<Material>();
		Register<MaterialParameterFloat>();
		Register<MaterialParameterColor>();
		Register<MaterialParameterInt>();
		Register<InputRecorder::DataObject>();

		// Events
		Register<TriggerEvent>();
		Register<UIEvent>();
		Register<WheelEvent>();

		// Resources
		Register<Resource>();
		Register<Texture>();
		Register<MemoryTexture>();
		Register<CubeMapResource>();
		Register<MeshResource>();		

		// 2D
		Register<Entity2D>();
		Register<Image>();
		Register<Sprite>();
		Register<Text>();
		Register<LayoutComponent>();		

		// 3D
		Register<Entity3D>();
		Register<Camera>();
		Register<MeshEntity>();
		Register<SubMesh>();
		Register<ParticleGenerator>();
		Register<BillBoardEntity>();	
		Register<SkyBoxEntity>();	
		Register<Path>();
		Register<PathElement>();	
		Register<CameraPath>();
		Register<CameraPathElement>();
		Register<Shader>();
		Register<ShaderParameter>();
		Register<QuadTree>();
		Register<Light>();
		Register<LightLookAt>();
		Register<Text3D>();
		Register<EffectEntity>();
		Register<TrailEntity>();
		Register<FakeCubeMapTexture>();

		// Collision
		Register<CollisionShapeBox>();
		Register<CollisionShapeSphere>();
		Register<CollisionComponent>();
		Register<EnvironmentCollisionComponent>();

		// Physics
		Register<RigidBodyComponent>();

		// Visitors
		Register<Visitor>();
		Register<AnimationVisitor>();	
		Register<Translation2DVisitor>();	
		Register<Rotation2DVisitor>();	
		Register<Scale2DVisitor>();	
		Register<Translation3DVisitor>();	
		Register<Rotation3DVisitor>();	
		Register<Scale3DVisitor>();	
		Register<DelayVisitor>();		
		Register<SequenceVisitor>();	
		Register<CombineVisitor>();	
		Register<FollowPathVisitor>();
		Register<FollowCameraPathVisitor>();
		Register<ExplosionVisitor>();
		Register<RemoveEntityVisitor>();
		Register<ColorVisitor>();
		Register<MaterialVisitor>();
		Register<FadeVisitor>();
		Register<EntityVisitor>();
		Register<ParticleVisitor>();
		Register<SendTriggerVisitor>();
		Register<Shake3DVisitor>();
		Register<CameraVisitor>();
		Register<TextVisitor>();
		Register<LoadContextVisitor>();
		Register<LightVisitor>();
		//Register<TextureAnimationVisitor>();

		// Components
		Register<Component>();
		Register<GraphicComponent>();

		// UI
		Register<Button>();
		Register<UISlider>();
		Register<MenuRootContext>();
	}
}


