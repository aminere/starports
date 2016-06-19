/* 

Amine Rehioui
Created: August 7th 2010

*/

#pragma once


namespace shoot
{
	//! class to keep track of editor settings
	class EditorSettings : public Entity
						 , public Singleton<EditorSettings>
	{
		DECLARE_SINGLETON(EditorSettings);
		DECLARE_OBJECT(EditorSettings, Entity);

	public:

		//! serializes the entity to/from a PropertyStream
		void Serialize(PropertyStream& stream);

		//! window sizes
		enum E_WindowSize
		{
			WS_Stretch,
			WS_iPhonePortrait,
			WS_iPhoneLandscape,
			WS_Count
		};

		static Size WindowSizes[WS_Count];

		//! Editor flags
		enum E_Flag
		{
			EF_RenderBBox =			0x1 << 0, // render bounding boxes
			EF_RenderEntity2DPos =	0x1 << 1, // render Entity2D positions
			EF_RenderEntity3DPos =	0x1 << 2, // render Entity3D positions
			EF_Render2DOrigin =		0x1 << 3, // renders the 2D world origin
			EF_Render3DOrigin =		0x1 << 4, // render 3D world origin
			EF_Render3DGrid =		0x1 << 5, // render 3D grid
			EF_CollisionShapes =	0x1 << 7, // render collision shapes
			EF_DebugIcons =			0x1 << 8, // render debug icons
			EF_RenderQuadTrees =	0x1 << 9, // render quad trees
			EF_RenderFrustums =		0x1 << 10, // render camera frustums
			EF_RenderPickingInfo =	0x1 << 11, // render 3D picking info
			EF_RenderDecalInfo =	0x1 << 12, // render decal info
		};

		//! Returns flags status
		inline bool HasFlag(E_Flag eFlag) const { return (m_Flags.m_Mask & eFlag) != 0; }

		// public properties
		int WindowSize;

		IPath DecalTexture;
		float fDecalProjectionOffset;

		Color BBoxColor;
		Color EntityPosColor;

		float fEntity2DPosSize;
		float f2DOriginSize;
		float fEntity3DPosSize;

		Color SelectedEntityColor;
		Color PathColor;
		Color CollisionShapeColor;

		float f2DZoomStep;
		float f3DZoomStep;
		float f3DStrafeStep;
		float f3DLookStep;

		float f3DZoomStepIncrFactor;
		float f3DStrafeStepIncrFactor;

		float fPathSmoothness;
		int GridSize;

		float fDebugIconSize;		
		float fPinSizeFactor;

		IPath LastLoadedProject;

		//! data structure for the flags
		struct Flags : public ISerializableStruct
		{
			//! constructor
			Flags() : m_Mask(0)
			{
			}

			//! Reads/Writes struct properties from/to a stream
			void Serialize(PropertyStream& stream);
			
			//! Sets rendering flags
			void Set(E_Flag eFlag, bool bEnabled = true) { bEnabled ? (m_Mask |= eFlag) : m_Mask &= ~eFlag; }

			int m_Mask;
		};

	private:

		// private properties
		Flags m_Flags;

		static const char* m_strSettingsFilePath;
	};
}




