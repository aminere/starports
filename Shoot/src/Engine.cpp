/* 

Amine Rehioui
Created: February 21th 2010

*/

#include "Shoot.h"

#include "EventManager.h"
#include "TimeManager.h"
#include "StatsManager.h"
#include "EditorEvents.h"
#include "MaterialProvider.h"
#include "GeometryProvider.h"
#include "FileSystem.h"
#include "MemoryManager.h"
#include "Thread.h"
#include "AudioDriver.h"
#include "NetworkManager.h"
#include "Gamepad.h"
#include "File.h"
#include "InputRecorder.h"
#include "FontManager.h"

#ifdef SHOOT_EDITOR
#include "FBXMeshLoader.h"
#endif

#include "RenderTarget.h"

#ifdef DX11
#include "DirectX11Driver.h"
#endif // DX11

namespace shoot
{
	// static variable initialization
	Engine* Engine::m_spInstance = NULL;
	float g_fDeltaTime = 1.0f/60.0f;
	uint g_CurrentFrame = 0;

	//! Constructor
	Engine::Engine(ObjectRegistrationCallback* pRegisterObjects /*= NULL*/,
				   InputManager::E_Type _eInputManagerType /*= InputManagre::IMT_SDL*/)
	: m_fps(0.0f)
	, m_frameTimer(0.0f)
	, m_Time(0.0f)
	, m_numFrames(0)
	, m_numFramesPerSecond(0)
	, m_bInitialized(false)
	, m_pLoadingThread(NULL)
	, m_bLoadingThreadFinished(false)
	, m_bResourceReloadThreadFinished(false)
	, m_bRequestResourceReload(false)
	, m_bPaused(false)
	, m_bExitRequested(false)
	{
		SHOOT_ASSERT(!m_spInstance, "Cannot create multiple Engine instances");
		
		MemoryManager::Init();
		ProfileManager::Init();
		
		m_spInstance = this;
		
		// initializes the random number generator
		Random::Initialize();	

		// create ObjectFactory and register objects
		ObjectFactory::CreateInstance();
		ObjectManager::CreateInstance();
		if(pRegisterObjects)
			pRegisterObjects();		

#ifdef SHOOT_EDITOR
		EditorEvents::Register();
		File::SetUseDataFolder(true);
#endif // SHOOT_EDITOR

		// create core singletons	
		FileSystem::CreateInstance();
		EventManager::CreateInstance();
		GraphicsDriver::CreateInstance();
		AudioDriver::CreateInstance();
		EntityRenderer::CreateInstance();
		ResourceManager::CreateInstance();
		TimeManager::CreateInstance();
		MaterialProvider::CreateInstance();
		GeometryProvider::CreateInstance();
		InputManager::CreateInstance(_eInputManagerType);
		InputRecorder::CreateInstance();
		FontManager::CreateInstance();

#ifndef SHOOT_NO_NETWORK
		NetworkManager::CreateInstance();
		NetworkManager::Instance()->Init();
#endif // SHOOT_NO_NETWORK

		Gamepad::CreateInstance();
		
		m_ContextStack = snew ContextStack();
	}

	Engine::~Engine()
	{
		m_bExitRequested = true;
		m_aEntityOperations.clear();
		m_ContextStack.Clear();
		m_FadeRenderer.Clear();
		m_FullScreenShader.Clear();
		m_Cursor.Clear();

#ifdef DX11
		ID3D11Debug* pDebug = NULL;
		DirectX11Driver::Instance()->GetDevice()->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void**>(&pDebug));
#endif

#ifdef SHOOT_EDITOR
		FBXMeshLoader::Uninit();
#endif
		InputRecorder::DestroyInstance();
		InputManager::DestroyInstance();
		if(StatsManager::IsCreated())		
			StatsManager::DestroyInstance();		

		FontManager::DestroyInstance();
		Gamepad::DestroyInstance();
		MaterialProvider::DestroyInstance();
		GeometryProvider::DestroyInstance();
		EntityRenderer::DestroyInstance();
		ResourceManager::DestroyInstance();
		GraphicsDriver::DestroyInstance();
		AudioDriver::DestroyInstance();
		ObjectFactory::DestroyInstance();
		ObjectManager::DestroyInstance();
		EventManager::DestroyInstance();
		TimeManager::DestroyInstance();
		FileSystem::DestroyInstance();

		if(NetworkManager::IsCreated())		
			NetworkManager::DestroyInstance();		
			
		m_spInstance = NULL;
		
		ProfileManager::Uninit();
		MemoryManager::Uninit();

#ifdef DX11
		if(pDebug)
		{
			pDebug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
			pDebug->Release();
		}
#endif

		Log.Close();
	}

	//! Engine initialization
	void Engine::Init()
	{
		GraphicsDriver::Instance()->Init();
		AudioDriver::Instance()->Init();
		EntityRenderer::Instance()->Init();
		StatsManager::CreateInstance();
		m_FadeRenderer = snew FadeRenderer();
		m_FullScreenShader = static_cast<Shader*>(ObjectManager::Instance()->FindOrLoad("common/shaders/sceneRT.xml"));
		m_Cursor = static_cast<Image*>(ObjectManager::Instance()->Load("common/inputmarker.xml"));
		m_Cursor->Init();

		m_bInitialized = true;
	}

	//! Loads a new context stack after clearing the existing one
	void Engine::LoadContextStack(const char* strPath, bool bImmediate, std::function<void()> loadingFinished)
	{
		if(bImmediate)
		{
			SHOOT_ASSERT(m_aEntityOperations.empty(), "Called LoadContextStack() while EntityOperations are pending");			
			m_ContextStack = snew ContextStack();
			m_ContextStack->LoadFromXML(strPath);
			m_ContextStack->Init();
			m_ContextStackToLoad.clear();
		}
		else
		{
			m_ContextStackToLoad = strPath;
			m_LoadingFinishedCallback = loadingFinished;
		}
	}
	
	//! ClearContextStack
	void Engine::ClearContextStack()
	{
		m_aEntityOperations.clear();
		m_ContextStack = snew ContextStack();
	}

	//! pushs a new context
	void Engine::PushContext(const char* strPath)
	{
		auto object = ObjectManager::Instance()->Load(strPath);
		auto context = DYNAMIC_CAST(object, Context);
		context->Init();
		m_ContextStack->AddChild(context, true);
	}

	//! returns the top context
	Context* Engine::GetTopContext() const
	{ 
		SHOOT_ASSERT(GetNumContexts() >= 1, "TopContext not found");
		return GetContext(GetNumContexts()-1);
	}
	
	//! called during the update of the engine
	bool Engine::Update(float fDeltaTime)
	{
		if(m_bPaused)
		{
			AudioDriver::Instance()->Update();
			return true;
		}

		if(!m_ContextStackToLoad.empty())
		{
			OnLoadingStarted(true);
			ClearContextStack();
			m_pLoadingThread = Thread::CreateThread();
			m_pLoadingThread->Start(&LoadingThread);
			return true;
		}

		if(m_bRequestResourceReload)
		{
			shoot::GraphicsDriver::Instance()->UnloadObjects();
			OnLoadingStarted(false);
			m_pLoadingThread = Thread::CreateThread();
			m_pLoadingThread->Start(&ResourceReloadThread);
			m_bRequestResourceReload = false;
			return true;
		}

		CalculateFPS(fDeltaTime);

		TimeManager::Instance()->Update();
		AudioDriver::Instance()->Update();
		Gamepad::Instance()->Update();
		InputRecorder::Instance()->Update();
		m_FadeRenderer->Update();

		if(!m_aEntityOperations.empty())
			DoEntityOperations();

		m_ContextStack->Update();

		#if SHOOT_PLATFORM == SHOOT_PLATFORM_WIN32
			m_Cursor->SetPosition(InputManager::Instance()->GetRawTouchState().vPosition);
			m_Cursor->SetVisible(InputRecorder::Instance()->GetState() != InputRecorder::S_Replaying);
		#endif

#ifndef _RETAIL_
		if(StatsManager::IsCreated())
			StatsManager::Instance()->Update();		
#endif

		EventManager::Instance()->Update();

		ResourceManager::Instance()->Update();

		return !m_bExitRequested;
	}

	//! called during the rendering the engine
	void Engine::Render()
	{
		EntityRenderer::Instance()->Clear();

		m_ContextStack->RegisterForRendering();

		#if SHOOT_PLATFORM == SHOOT_PLATFORM_WIN32
			#ifndef SHOOT_EDITOR
			if (InputRecorder::Instance()->GetState() != InputRecorder::S_Replaying)
				EntityRenderer::Instance()->RegisterEntity(m_Cursor);
			#endif
		#endif

		#ifndef _RETAIL_
			if(StatsManager::IsCreated())
				StatsManager::Instance()->RegisterForRendering();
		#endif
		
		EntityRenderer::Instance()->Render();
	}

	//! resizes the screen surface
	void Engine::ResizeScreen(const Vector2& vSize)
	{
		Log.Print("Engine::ResizeScreen: %f, %f\n", vSize.X, vSize.Y);
		GraphicsDriver::Instance()->ResizeScreen(vSize);

		if (EntityRenderer::Instance()->GetFinalRenderTarget())
			EntityRenderer::Instance()->OnResize();
	}

	//! called when the application is paused
	void Engine::OnPause()
	{
		m_bPaused = true;
		Log << "Engine::OnPause()\n";
		// TODO: might need to queue and process in Update(), audio is not thread safe
		AudioDriver::Instance()->Pause();
	}

	//! called when the application is resumed
	void Engine::OnResume()
	{
		Log << "Engine::OnResume()\n";
		// TODO: might need to queue and process in Update(), audio is not thread safe		
		AudioDriver::Instance()->Resume();
	}

	//! calculates FPS
	void Engine::CalculateFPS(float fDeltaTime)
	{
#ifndef SHOOT_EDITOR
		g_fDeltaTime = Math::Min(fDeltaTime, 0.033333f);
#else
		g_fDeltaTime = fDeltaTime;
#endif
		
		m_Time += fDeltaTime;

		// calc FPS
		m_frameTimer += fDeltaTime;
		m_numFrames++;
		m_numFramesPerSecond++;
		if(m_frameTimer >= 1.0f)
		{
			m_fps = float(m_numFramesPerSecond) / m_frameTimer;
			m_numFramesPerSecond = 0;
			m_frameTimer = 0.0f;					
		}

		g_CurrentFrame = m_numFrames;
	}

	//! adds an entity operation
	void Engine::AddEntityOperation(const EntityOperationInfo& info)
	{
		// make sure there are no duplicate operations		
		if(std::find(m_aEntityOperations.begin(), m_aEntityOperations.end(), info) == m_aEntityOperations.end())		
			m_aEntityOperations.push_back(info);
	}

	//! do entity operation
	void Engine::DoEntityOperations()
	{
		// sort operations - always do add operations first
		std::sort(m_aEntityOperations.begin(), m_aEntityOperations.end(), EntityOperationInfoComparator);

		for (size_t i=0; i<m_aEntityOperations.size(); ++i)
		{
			EntityOperationInfo& info = m_aEntityOperations[i];
			switch(info.m_Operation)
			{
			case EO_AddChild:
				{
					SHOOT_ASSERT(info.m_Object->IsA(Entity::TypeID), "Invalid Entity Operation");
					Entity* pEntity = static_cast<Entity*>(info.m_Object.Get());
					info.m_Entity->AddChild(pEntity, true);
					if(!pEntity->IsInitialized())
						pEntity->Init();

					// notify the quadtree
					if(QuadTree* pQuadTree = pEntity->GetRootContext()->GetQuadTree())					
						pQuadTree->OnEntityAdded(pEntity);
				}
				break;

			case EO_RemoveChild:
				SHOOT_ASSERT(info.m_Object->IsA(Entity::TypeID), "Invalid Entity Operation");				
				info.m_Entity->RemoveChild(static_cast<Entity*>(info.m_Object.Get()), true);
				break;

			case EO_AddComponent:
				{
					SHOOT_ASSERT(info.m_Object->IsA(Component::TypeID), "Invalid Entity Operation");
					Component* pComponent = static_cast<Component*>(info.m_Object.Get());
					info.m_Entity->AddComponent(pComponent, true);
					pComponent->Init();
				}
				break;

			case EO_RemoveComponent:
				SHOOT_ASSERT(info.m_Object->IsA(Component::TypeID), "Invalid Entity Operation");
				info.m_Entity->RemoveComponent(static_cast<Component*>(info.m_Object.Get()), true);
				break;

			default: SHOOT_ASSERT(0, "Invalid E_EntityOperation");
			}
		}

		m_aEntityOperations.clear();
	}

	//! entity operation comparator
	bool Engine::EntityOperationInfoComparator(const EntityOperationInfo& _1, const EntityOperationInfo& _2)
	{
		return _1.m_Operation < _2.m_Operation;
	}

	//! loading thread
	void* Engine::LoadingThread(void* pParam)
	{
		Engine* pEngine = Engine::Instance();
		
		// load new partitions
		for (auto& partition : pEngine->m_PartitionsToLoad)
			FileSystem::Instance()->LoadPartition(partition);

		pEngine->m_ContextStack->LoadFromXML(pEngine->m_ContextStackToLoad.c_str());
		pEngine->m_ContextStack->Init();
		pEngine->m_ContextStackToLoad.clear();
		pEngine->m_bLoadingThreadFinished = true;
		return NULL;
	}
	
	//! resource reload thread
	void* Engine::ResourceReloadThread(void* pParam)
	{
		ResourceManager::Instance()->ReloadResources();
		Engine::Instance()->m_bResourceReloadThreadFinished = true;
		return NULL;
	}

	//! updates loading thread status
	void Engine::UpdateLoadingThreadStatus()
	{
		AudioDriver::Instance()->Update();
		if(m_bLoadingThreadFinished)
		{
			sdelete(m_pLoadingThread);
			m_LoadingFinishedCallback();
			m_bLoadingThreadFinished = false;
		}
		else if(m_bResourceReloadThreadFinished)
		{
			sdelete(m_pLoadingThread);
			m_ResourceReloadFinishedCallback();
			m_bResourceReloadThreadFinished = false;
		}
	}

	//! requests resource reload
	void Engine::RequestResourceReload(std::function<void()> reloadingFinished) 
	{
		m_bRequestResourceReload = true;
		m_bPaused = false;
		m_ResourceReloadFinishedCallback = reloadingFinished;		
	}

	//! requests an application exit
	void Engine::RequestExit()
	{
		m_bExitRequested = true;
	}

	//! Present
	void Engine::Present()
	{
		auto finalRT = EntityRenderer::Instance()->GetFinalRenderTarget();
		auto fullScreenQuad = GeometryProvider::Instance()->GetFullScreenQuad();
		int finalRTIndex = 0;
		GraphicsDriver::Instance()->SetRenderTarget(NULL);
		GraphicsDriver::Instance()->SetRenderState(GraphicsDriver::RS_AlphaBlending, false);
		m_FullScreenShader->GetConstant("uTexture0")->Set(&finalRTIndex, 1);
		m_FullScreenShader->GetConstant("uAmbientColor")->Set(&Color::White, 1);
		finalRT->BindTexture(finalRTIndex, GraphicsDriver::BF_ColorBuffer);
		fullScreenQuad->Draw(m_FullScreenShader);
	}
}

