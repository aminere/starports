/* 

Amine Rehioui
Created: April 10th 2010

*/

#pragma once


namespace shoot
{	
	// forwards
	class MainFrame;
	class ViewPort;
	class TabContainer;
	class Path;

	/** Provides access to common editor operations,
		coordinates the communication between the controls */
	class ShootEditor : public Singleton<ShootEditor>
	{
		DECLARE_SINGLETON(ShootEditor);

	public:

		//! EngineState
		enum E_EngineState
		{
			ES_Stopped,
			ES_Running,
			ES_Paused,
			ES_Recording
		};

		//! Initialize the controls
		virtual void Init(MainFrame* pFrame);

		//! Updates the editor
		void Update();

		//! Returns the tree view
		TreeView* GetTreeView() { return m_pTreeView; }

		//! returns the object inspector
		ObjectInspector* GetObjectInspector() { return m_pObjectInspector; }

		//! Returns a viewport
		ViewPort* GetViewPort();

		//! Returns the resource window
		ResourceWindow* GetResourceWindow() { return m_pResourceWindow; }

		//! returns the main tab control
		MainTabControl* GetMainTabControl() { return m_pMainTabControl; }

		//! Selects an entity
		Entity* SelectEntity(Entity* pEntity);

		//! Selects an entity
		virtual Entity* SelectEntity(Entity* pEntity, const Vector3& vRayStart, const Vector3& vRayDir);

		//! Creates a view port
		virtual ViewPort* CreateViewPort(wxWindow* pParent);

		//! Returns the selected entity
		Entity* GetSelectedEntity();

		//! returns true if the engine is running
		bool IsEngineRunning() const { return m_EngineState == ES_Running || m_EngineState == ES_Recording; }

		//! returns true if the engine is started
		bool IsEngineStarted() const { return m_EngineState != ES_Stopped; }
			
		//! update viewports sizes
		void UpdateViewPortsSizes();

		//! updates the camera selector
		void UpdateCameraSelectors();

		//! control modes
		enum E_ControlMode
		{
			ControlMode_Translate,
			ControlMode_Rotate,
			ControlMode_Scale
		};

		//! sets the control mode
		void SetControlMode(E_ControlMode eMode) { m_eControlMode = eMode; }

		//! returns pin status
		inline bool HasPin() const { return m_bHasPin; }

		//! sets pin status
		void SetHasPin(bool bSet);

		//! returns pin position
		inline const Vector3& GetPinPosition() const { return m_vPinPosition; }

		//! returns the control mode
		E_ControlMode GetControlMode() const { return m_eControlMode; } 

		//! returns game mouse status
		inline bool HasGameMouseControl() const { return m_bGameMouse; }

		//! sets game mouse status
		inline void SetGameMouseControl(bool bGameMouse) { m_bGameMouse = bGameMouse; }

		//! starts the engine
		void StartEngine();

		//! RecordEngine
		void RecordEngine();

		//! stops the engine
		void StopEngine();

		//! pauses the engine
		void PauseEngine();

		//! ReplayEngine
		void ReplayEngine();

		//! returns a tab container
		TabContainer* GetTabContainer();

		//! returns a menu category for a class type
		virtual std::string GetMenuCategory(const char* classType);

		//! sets the delta time
		inline void SetDeltaTime(float fDeltaTime) { m_fDeltaTime = fDeltaTime; }

		//! sets the 3D control status
		inline void Set3DControl(bool b3DControl) { m_b3DControl = b3DControl; }

		//! gets the 3D control status
		inline bool Get3DControl() const { return m_b3DControl; }

		//! sets audio status
		void SetAudioEnabled(bool bEnabled);

		//! gets audio status
		inline bool IsAudioEnabled() const { return m_bAudioEnabled; }

		//! gets the locked entity
		inline Entity* GetLockedEntity() const { return m_LockedEntity; }

		//! sets the locked entity
		inline void SetLockedEntity(Entity* pEntity) { m_LockedEntity = pEntity; }		

		void StartBackgroundThread(const std::string& target, bool replay);		
		void StopBackgroundThread() { _backgroundThreadFinished = true; }
		inline const std::string& GetBackgroundThreadMessage() const { return _backgroundThreadMessage; }

	protected:

		//! StopRecording
		void StopRecording();

		TreeView* m_pTreeView;		
		ObjectInspector* m_pObjectInspector;		
		ResourceWindow* m_pResourceWindow;
		MainTabControl* m_pMainTabControl;

		E_EngineState m_EngineState;		

		ShootEditorEventsHandler m_EditorEventsHandler;
		bool _backgroundThreadStarted;
		bool _backgroundThreadFinished;
		std::string _backgroundThreadMessage;

		E_ControlMode m_eControlMode;
		bool m_bHasPin;
		Vector3 m_vPinPosition;
		bool m_b3DControl;
		bool m_bAudioEnabled;
		Handle<Entity> m_LockedEntity;
		bool m_bGameMouse;

		float m_fDeltaTime;
	};
}




