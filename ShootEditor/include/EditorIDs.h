/* 

Amine Rehioui
Created: August 24th 2011

*/

#pragma once


namespace shoot
{
	// constants
	const int MaxEntityTypes = 1024;

	//! event IDs
	enum E_EventID
	{
		ID_Menu_New,
		ID_Menu_Open,
		ID_Menu_OpenPrevious,
		ID_Menu_Save,
		ID_Menu_SaveAs,
		ID_Menu_ReloadLastSave,
		ID_Menu_Insert_FirstEntity = wxID_HIGHEST,
		ID_Menu_Insert_LastEntity = ID_Menu_Insert_FirstEntity + MaxEntityTypes,
		ID_Menu_Duplicate,
		ID_Menu_Remove,
		ID_Menu_ToggleGameCameraControl,
		ID_Menu_ShowSettings,
		ID_Menu_ShowConsole,
		ID_Edit_Translate,
		ID_Edit_Rotate,
		ID_Edit_Scale,
		ID_Edit_Pin,
		ID_Edit_Undo,
		ID_Edit_Redo,
		ID_Edit_UnlinkTemplate,
		ID_Edit_UseTemplate,
		ID_Edit_LockSelection,
		ID_Edit_UnlockSelection,
		ID_Engine_Start,
		ID_Engine_Record,
		ID_Engine_Replay,
		ID_Engine_Stop,
		ID_Engine_Pause,
		ID_3DCameraSelector,
		ID_2DCameraSelector,
		ID_Menu_ToggleEditorPostRendering,
		ID_Menu_ToggleAudio,
		ID_Menu_Toggle3DControl,
		ID_Menu_ToggleGameMouseControl,
		ID_ObjectInspector_Load,
		ID_ObjectInspector_Save,
		ID_ObjectInspector_SaveAs,
		ID_ObjectInspector_Reload,
		ID_ObjectInspector_AddComponent,
		ID_ObjectInspector_RemoveComponent,
		ID_ObjectInspector_CopyComponent,
		ID_ObjectInspector_PasteComponent,
		ID_ObjectInspector_Tree,
		ID_ObjectInspector_ViewDependencies,
		ID_DependencyViewer_List,
		ID_DependencyViewer_Tree,
		ID_DependencyViewer_ViewDependencies,
		ID_Property_MoveUp,
		ID_Property_MoveDown,
		ID_Property_Remove,
		ID_Property_Copy,
		ID_Property_Override,
		ID_Property_Reset,
		ID_Property_DifferentiateMaterial,
		ID_Property_ViewDependencies,
		ID_Tab_ResetView,
		ID_Tab_ClearFade,
		ID_Tab_ToggleBlackBars,
		ID_PropertyOverride,
		ID_ConsoleClear,
		ID_TreeView,
		ID_Last
	};
}



