/* 

Amine Rehioui
Created: February 21th 2010

*/

#pragma once


#include "InputManager.h"

#include "Context.h"
#include "ContextStack.h"

#include "Size.h"

#include "EntityOperation.h"
#include "FadeRenderer.h"
#include "Image.h"

namespace shoot
{
	// forwards
	class Thread;

	//! Shoot Engine base class
	class Engine
	{
	public:

		//! Object registration callback
		typedef void ObjectRegistrationCallback(void);
		
		//! Loading finished callback
		typedef void LoadingFinishedCallback(void);

		//! returns the unique instance of the engine
		inline static Engine* Instance() { return m_spInstance; }

		//! Constructor
		Engine(ObjectRegistrationCallback* pRegisterObjects = NULL,
			   InputManager::E_Type _eInputManagerType = InputManager::IMT_SDL);

		//! Destructor
		virtual ~Engine();

		//! Engine initialization
		virtual void Init();

		//! called during the update of the engine
		virtual bool Update(float fDeltaTime);

		//! called during the rendering the engine
		virtual void Render();

		//! resizes the screen surface
		virtual void ResizeScreen(const Vector2& vSize);

		//! called when the application is paused
		virtual void OnPause();

		//! called when the application is resumed
		virtual void OnResume();

		//! returns the loading status
		virtual bool IsLoading() const { return false; }
		
		//! Loads a new context stack after clearing the existing one
		void LoadContextStack(const char* strPath, bool bImmediate, std::function<void()> loadingFinished);

		//! SetPartitionsToLoad
		inline void SetPartitionsToLoad(const std::vector<std::string>& partitions) { m_PartitionsToLoad = partitions; }

		//! ClearContextStack
		void ClearContextStack();

		//! pushs a new context
		void PushContext(const char* strPath);

		//! returns the top context
		Context* GetTopContext() const;

		//! calculates FPS
		void CalculateFPS(float fDeltaTime);

		//! adds an entity operation
		void AddEntityOperation(const EntityOperationInfo& info);

		//! requests resource reload
		void RequestResourceReload(std::function<void()> reloadingFinished);

		//! requests an application exit
		void RequestExit();

		inline void PopContext() const { GetTopContext()->Pop(); }
		inline size_t GetNumContexts() const { return m_ContextStack->GetChildCount(); }
		inline Context* GetContext(size_t index) const { return (m_ContextStack->GetChildCount() ? static_cast<Context*>(m_ContextStack->GetChild(index)) : NULL); }
		inline ContextStack* GetContextStack() const { return m_ContextStack; }
		inline float GetFPS() const { return m_fps; }
		inline float GetTime() const { return m_Time; }
		inline bool IsRunning() const { return !m_bExitRequested; }
		inline bool IsPaused() const { return m_bPaused; }
		inline Shader* GetFullScreenShader() const { return m_FullScreenShader; }

	protected:

		Reference<ContextStack> m_ContextStack;
		Reference<FadeRenderer> m_FadeRenderer;
		Reference<Shader> m_FullScreenShader;
		std::string m_ContextStackToLoad;
		bool m_bRequestResourceReload;
		Thread* m_pLoadingThread;
		std::function<void()> m_LoadingFinishedCallback;
		bool m_bLoadingThreadFinished;
		std::function<void()> m_ResourceReloadFinishedCallback;
		bool m_bResourceReloadThreadFinished;
		Reference<Image> m_Cursor;

		bool m_bPaused;
		float m_Time;

		// fps calculation
		float m_fps;
		float m_frameTimer;
		uint m_numFrames;
		uint m_numFramesPerSecond;

		//! do entity operation
		void DoEntityOperations();

		std::vector<EntityOperationInfo> m_aEntityOperations;

		//! entity operation comparator
		static bool EntityOperationInfoComparator(const EntityOperationInfo& _1, const EntityOperationInfo& _2);

		//! loading thread
		static void* LoadingThread(void* pParam);

		//! resource reload thread
		static void* ResourceReloadThread(void* pParam);

		//! updates loading thread status
		void UpdateLoadingThreadStatus();

		//! called when the loading thread has started
		virtual void OnLoadingStarted(bool useLoadingTransition) { }

		//! Present
		void Present();

	private:

		static Engine* m_spInstance;
		std::vector<std::string> m_PartitionsToLoad;

		bool m_bInitialized;
		bool m_bExitRequested;
	};
}



