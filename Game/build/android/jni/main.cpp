
#include <jni.h>
#include <errno.h>
#include <sys/stat.h>
#include <android/asset_manager_jni.h>

#include "../../../src/game/Game.h"
#include "../../../src/game/GameEngine.h"
#include "FileAPK.h"
#include "InputManager.h"
#include "FileSystem.h"

extern void RegisterObjects();
static double g_PreviousTime = 0;
static bool g_bContextReady = false;

// Initialize Audio
#include "ck/ck.h"
#include "ck/config.h"

/* return current time in seconds */
static double now_seconds(void)
{
    struct timespec res;
    clock_gettime(CLOCK_REALTIME, &res);
    return res.tv_sec + (double)res.tv_nsec/1e9;
}

extern "C"
{
    JNIEXPORT void JNICALL Java_com_amine_JNILib_init(JNIEnv * env, jobject obj, jobject activityObject, jobject assetManager, jstring internalPath, jstring externalPath);
    JNIEXPORT void JNICALL Java_com_amine_JNILib_step(JNIEnv * env, jobject obj);
	JNIEXPORT void JNICALL Java_com_amine_JNILib_setTouchState(JNIEnv * env, jobject obj, jfloat x, jfloat y, jboolean bPressed);
	JNIEXPORT void JNICALL Java_com_amine_JNILib_setBackPressed(JNIEnv * env, jobject obj, jboolean bPressed);
	JNIEXPORT void JNICALL Java_com_amine_JNILib_createContext(JNIEnv * env, jobject obj);
	JNIEXPORT void JNICALL Java_com_amine_JNILib_resizeScreen(JNIEnv * env, jobject obj, jint width, jint height);
	JNIEXPORT void JNICALL Java_com_amine_JNILib_pause(JNIEnv * env, jobject obj);
	JNIEXPORT void JNICALL Java_com_amine_JNILib_resume(JNIEnv * env, jobject obj);
	JNIEXPORT void JNICALL Java_com_amine_JNILib_destroy(JNIEnv * env, jobject obj);
	JNIEXPORT void JNICALL Java_com_amine_JNILib_stop(JNIEnv * env, jobject obj);
	JNIEXPORT bool JNICALL Java_com_amine_JNILib_isExitRequested(JNIEnv * env, jobject obj);
	JNIEXPORT void JNICALL Java_com_amine_JNILib_onFPSModeAcquired(JNIEnv * env, jobject obj);	
	JNIEXPORT bool JNICALL Java_com_amine_JNILib_isBuyFPSModeRequested(JNIEnv * env, jobject obj);
	JNIEXPORT void JNICALL Java_com_amine_JNILib_resetBuyFPSModeRequest(JNIEnv * env, jobject obj);
	JNIEXPORT void JNICALL Java_com_amine_JNILib_zoom(JNIEnv * env, jobject obj, jboolean zoom);	
	JNIEXPORT void JNICALL Java_com_amine_JNILib_zoomStop(JNIEnv * env, jobject obj);	
};

JNIEXPORT void JNICALL Java_com_amine_JNILib_init(JNIEnv *env, jobject obj, jobject activityObject, jobject assetManager, jstring internalPath, jstring externalPath)
{
	shoot::Log << "Init";
	AAssetManager* pAssetMgr = AAssetManager_fromJava(env, assetManager);	
	const char* strInternalPath = env->GetStringUTFChars(internalPath, NULL);
	const char* strExternalPath = env->GetStringUTFChars(externalPath, NULL);	
	shoot::FileAPK::SetAssetManager(pAssetMgr);	
	shoot::File::SetWritablePath(strInternalPath);
	shoot::File::SetExternalWritablePath(strExternalPath);
	
	shoot::Log.Print("Initializing audio..\n");
	CkConfig config(env, activityObject);
	CkInit(&config);
	
#ifndef _RETAIL_
	// create external directory if it doesn't exist
	struct stat _stat;
	if(stat(strExternalPath, &_stat) == -1 && (errno == ENOENT))
	{
		std::string path = strExternalPath;
		std::string rootDir = path.substr(0, path.rfind("/"));
		shoot::Log.Print("Creating directory: %s", rootDir.c_str());
		bool bError = mkdir(rootDir.c_str(), 0770) != 0;
		if(!bError)
		{
			shoot::Log.Print("Creating directory: %s", strExternalPath);
			bError = mkdir(strExternalPath, 0770) != 0;
		}		
		if(bError)
		{
			shoot::Log.Print("mkdir error: %s", strerror(errno));
		}
	}
#endif // _RETAIL_
}

JNIEXPORT void JNICALL Java_com_amine_JNILib_step(JNIEnv * env, jobject obj)
{	
	if(!g_bContextReady)
	{
		return;
	}
	
	double currentTime = now_seconds();
	float fDeltaTime = currentTime-g_PreviousTime;
	g_PreviousTime = currentTime;
		
	shoot::InputManager::Instance()->Update();
	shoot::Engine::Instance()->Update(fDeltaTime);
	shoot::Engine::Instance()->Render();
}

JNIEXPORT void JNICALL Java_com_amine_JNILib_setTouchState(JNIEnv * env, jobject obj, jfloat x, jfloat y, jboolean bPressed)
{
	shoot::InputManager::TouchState state;
	state.vPosition = shoot::Vector2::Create(x, y);
	state.bPressed = bPressed;
	shoot::InputManager::Instance()->SetTouchState(state);	
}

JNIEXPORT void JNICALL Java_com_amine_JNILib_setBackPressed(JNIEnv * env, jobject obj, jboolean bPressed)
{
	shoot::InputManager::Instance()->SetKeyPressed(shoot::InputManager::KT_Back, bPressed);
}

JNIEXPORT void JNICALL Java_com_amine_JNILib_zoom(JNIEnv * env, jobject obj, jboolean zoom)
{	
	shoot::InputManager::Instance()->SetKeyPressed(shoot::InputManager::KT_Left, !zoom);
	shoot::InputManager::Instance()->SetKeyPressed(shoot::InputManager::KT_Right, zoom);	
}

JNIEXPORT void JNICALL Java_com_amine_JNILib_zoomStop(JNIEnv * env, jobject obj)
{
	shoot::InputManager::Instance()->SetKeyPressed(shoot::InputManager::KT_Left, false);
	shoot::InputManager::Instance()->SetKeyPressed(shoot::InputManager::KT_Right, false);
	shoot::InputManager::TouchState state;
	state.vPosition = shoot::Vector2::Zero;
	state.bPressed = false;
	shoot::InputManager::Instance()->SetTouchState(state);	
}

JNIEXPORT void JNICALL Java_com_amine_JNILib_createContext(JNIEnv * env, jobject obj)
{
	shoot::Log << "createContext";
	if(shoot::Engine::Instance())
	{
		shoot::Engine::Instance()->RequestResourceReload([]()
		{
			shoot::GraphicsDriver::Instance()->LoadObjects();
			shoot::FileSystem::Instance()->UnloadAllPartitions("common");
			shoot::Engine::Instance()->OnResume();
		});
	}
	g_bContextReady = true;
}

static void printGLString(const char *name, GLenum s)
{
    const char *v = (const char *) glGetString(s);
    shoot::Log.Print("GL %s = %s", name, v);
}

JNIEXPORT void JNICALL Java_com_amine_JNILib_resizeScreen(JNIEnv * env, jobject obj, jint width, jint height)
{
	shoot::Log << "resizeScreen";
	// TODO: should start a native thread and do this in it.
	if(!shoot::Engine::Instance())
	{
		printGLString("Version", GL_VERSION);
		printGLString("Vendor", GL_VENDOR);
		printGLString("Renderer", GL_RENDERER);
		
		shoot::Engine* pEngine = new shoot::GameEngine(&RegisterObjects, shoot::InputManager::IMT_Touch);
		pEngine->Init();
	}	
	shoot::Engine::Instance()->ResizeScreen(shoot::Vector2::Create(float(width), float(height)));	
}

JNIEXPORT void JNICALL Java_com_amine_JNILib_pause(JNIEnv * env, jobject obj)
{
	g_bContextReady = false;
	if(shoot::Engine::Instance())
	{
		shoot::Engine::Instance()->OnPause();
	}
}

JNIEXPORT void JNICALL Java_com_amine_JNILib_resume(JNIEnv * env, jobject obj)
{
	shoot::Log << "resume";
}

JNIEXPORT void JNICALL Java_com_amine_JNILib_destroy(JNIEnv * env, jobject obj)
{
	shoot::Log << "destroy";
	if(shoot::Engine::Instance())
	{
		delete shoot::Engine::Instance();
	}
}

JNIEXPORT void JNICALL Java_com_amine_JNILib_stop(JNIEnv * env, jobject obj)
{
	shoot::Log << "stop";	
}

JNIEXPORT bool JNICALL Java_com_amine_JNILib_isExitRequested(JNIEnv * env, jobject obj)
{
	if(shoot::Engine::Instance())
	{
		return !shoot::Engine::Instance()->IsRunning();
	}
	return false;
}

JNIEXPORT void JNICALL Java_com_amine_JNILib_onFPSModeAcquired(JNIEnv * env, jobject obj)
{
	if(auto engine = shoot::GameEngine::Instance())
    {		
		engine->GetGameMgr()->GetGameData()->OnProductPurchased(L"com.frantic.starports.extramode1");
		engine->GetGameMgr()->OnTransactionCompleted(true);
    }
}

JNIEXPORT bool JNICALL Java_com_amine_JNILib_isBuyFPSModeRequested(JNIEnv * env, jobject obj)
{
	auto engine = shoot::GameEngine::Instance();
	return engine->GetGameMgr()->m_HasBuyFPSModeRequest;
}

JNIEXPORT void JNICALL Java_com_amine_JNILib_resetBuyFPSModeRequest(JNIEnv * env, jobject obj)
{
	auto engine = shoot::GameEngine::Instance();
	engine->GetGameMgr()->m_HasBuyFPSModeRequest = false;
}


