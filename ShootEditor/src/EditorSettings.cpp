/* 

Amine Rehioui
Created: August 7th 2010

*/

#include "ShootEditorCommon.h"

#include "EditorSettings.h"

#include "File.h"

namespace shoot
{
	DEFINE_OBJECT(EditorSettings);

	// static variables initialization
	const char* EditorSettings::m_strSettingsFilePath = "../EditorSettings.xml";

	// window size literals
	const char* const g_strWindowSizeLiterals[] =
	{
		"Stretch",
		"iPhonePortrait",
		"iPhoneLandscape",
		0
	};

	Size EditorSettings::WindowSizes[EditorSettings::WS_Count] =
	{
		Size(), // Stretch
		Size(320, 480), // iPhone portrait
		Size(480, 320), // iPhone landscape
	};

	//! constructor
	EditorSettings::EditorSettings()
	{		
		WindowSize = WS_Stretch;

		// post rendering elements configuration
		m_Flags.Set(EF_RenderBBox);
		m_Flags.Set(EF_RenderEntity2DPos);
		m_Flags.Set(EF_RenderEntity3DPos);
		m_Flags.Set(EF_Render2DOrigin);
		m_Flags.Set(EF_Render3DOrigin);
		m_Flags.Set(EF_Render3DGrid);
		m_Flags.Set(EF_DebugIcons);
		m_Flags.Set(EF_RenderQuadTrees);
		m_Flags.Set(EF_RenderPickingInfo);
		m_Flags.Set(EF_RenderDecalInfo);

		BBoxColor.Set(1.0f, 0.0f, 1.0f);
		EntityPosColor.Set(1.0f, 0.0f, 1.0f);

		SelectedEntityColor.Set(1.0f, 1.0f, 0.0f);
		PathColor = Color::Green;
		CollisionShapeColor = Color::White;

		fEntity2DPosSize = 16.0f;
		f2DOriginSize = 64.0f;
		fEntity3DPosSize = 0.25f;

		f2DZoomStep = 0.01f;
		f3DZoomStep = 0.1f;
		f3DStrafeStep = 0.02f;
		f3DLookStep = 0.2f;

		f3DZoomStepIncrFactor = 2.0f;
		f3DStrafeStepIncrFactor = 2.0f;

		fDecalProjectionOffset = 10.0f;
		fPathSmoothness = 50.0f;
		GridSize = 200;

		fDebugIconSize = 0.5f;
		fPinSizeFactor = 0.02f;

		// attempt to load from file
		if(File::Exists(m_strSettingsFilePath))
		{
			LoadFromXML(m_strSettingsFilePath);
		}
	}

	//! destructor
	EditorSettings::~EditorSettings()
	{
		SaveToXML(m_strSettingsFilePath);
	}

	//! serializes the entity to/from a PropertyStream
	void EditorSettings::Serialize(PropertyStream& stream)
	{
		stream.Serialize("WindowSize", &WindowSize, ENUM_PARAMS1(g_strWindowSizeLiterals));

		stream.Serialize("Flags", &m_Flags);

		stream.Serialize("DecalTexture", &DecalTexture);
		stream.Serialize("DecalProjectionOffset", &fDecalProjectionOffset);

		stream.Serialize("BBoxColor", &BBoxColor);
		stream.Serialize("EntityPosColor", &EntityPosColor);

		stream.Serialize("Entity2DPosSize", &fEntity2DPosSize);
		stream.Serialize("2DOriginSize", &f2DOriginSize);
		stream.Serialize("Entity3DPosSize", &fEntity3DPosSize);
		
		stream.Serialize("SelectedEntityColor", &SelectedEntityColor);
		stream.Serialize("PathColor", &PathColor);
		stream.Serialize("CollisionShapeColor", &CollisionShapeColor);
		
		stream.Serialize("2DZoomStep", &f2DZoomStep);
		stream.Serialize("3DZoomStep", &f3DZoomStep);
		stream.Serialize("3DStrafeStep", &f3DStrafeStep);
		stream.Serialize("3DLookStep", &f3DLookStep);

		stream.Serialize("3DZoomStepIncrFactor", &f3DZoomStepIncrFactor);
		stream.Serialize("3DStrafeStepIncrFactor", &f3DStrafeStepIncrFactor);			

		stream.Serialize("PathSmoothness", &fPathSmoothness);
		stream.Serialize("GridSize", &GridSize);

		stream.Serialize("DebugIconSize", &fDebugIconSize);
		stream.Serialize("PinSizeFactor", &fPinSizeFactor);

		stream.Serialize("LastLoadedProject", &LastLoadedProject);

		if(!IsInitialized())
			return;

		ShootEditor::Instance()->UpdateViewPortsSizes();
	}

	//! Reads/Writes struct properties from/to a stream
	void EditorSettings::Flags::Serialize(PropertyStream& stream)
	{
		bool bRenderBBox = (m_Mask & EF_RenderBBox) != 0;
		stream.Serialize("RenderBBox", &bRenderBBox); 

		bool bRenderEntity2DPos = (m_Mask & EF_RenderEntity2DPos) != 0;
		stream.Serialize("RenderEntity2DPos", &bRenderEntity2DPos); 

		bool bRenderEntity3DPos = (m_Mask & EF_RenderEntity3DPos) != 0;
		stream.Serialize("RenderEntity3DPos", &bRenderEntity3DPos); 

		bool bRender2DOrigin = (m_Mask & EF_Render2DOrigin) != 0;
		stream.Serialize("Render2DOrigin", &bRender2DOrigin); 

		bool bRender3DOrigin = (m_Mask & EF_Render3DOrigin) != 0;
		stream.Serialize("Render3DOrigin", &bRender3DOrigin); 

		bool bRender3DGrid = (m_Mask & EF_Render3DGrid) != 0;
		stream.Serialize("Render3DGrid", &bRender3DGrid); 
		
		bool bCollisionShapes = (m_Mask & EF_CollisionShapes) != 0;
		stream.Serialize("CollisionShapes", &bCollisionShapes); 

		bool bDebugIcons = (m_Mask & EF_DebugIcons) != 0;
		stream.Serialize("DebugIcons", &bDebugIcons); 

		bool bRenderQuadTrees = (m_Mask & EF_RenderQuadTrees) != 0;
		stream.Serialize("RenderQuadTrees", &bRenderQuadTrees); 

		bool bRenderFrustums = (m_Mask & EF_RenderFrustums) != 0;
		stream.Serialize("RenderFrustums", &bRenderFrustums); 

		bool bRenderPickingInfo = (m_Mask & EF_RenderPickingInfo) != 0;
		stream.Serialize("RenderPickingInfo", &bRenderPickingInfo); 

		auto renderDecalInfo = (m_Mask & EF_RenderDecalInfo) != 0;
		stream.Serialize("RenderDecalInfo", &renderDecalInfo);

		if(stream.GetMode() != SM_Read)
			return;

		Set(EF_RenderBBox, bRenderBBox);
		Set(EF_RenderEntity2DPos, bRenderEntity2DPos);
		Set(EF_RenderEntity3DPos, bRenderEntity3DPos);
		Set(EF_Render2DOrigin, bRender2DOrigin);
		Set(EF_Render3DOrigin, bRender3DOrigin);
		Set(EF_Render3DGrid, bRender3DGrid);
		Set(EF_CollisionShapes, bCollisionShapes);
		Set(EF_DebugIcons, bDebugIcons);
		Set(EF_RenderQuadTrees, bRenderQuadTrees);
		Set(EF_RenderFrustums, bRenderFrustums);
		Set(EF_RenderPickingInfo, bRenderPickingInfo);
		Set(EF_RenderDecalInfo, renderDecalInfo);
	}
}

