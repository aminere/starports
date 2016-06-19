#include "pch.h"
#include "PhoneDirect3DApp1.h"
#include "BasicTimer.h"

#include "InputManager.h"
#include "GameEngine.h"
#include "DirectX11Driver.h"
#include "File.h"
#include "GameManager.h"
#include "tinyxml/tinyxml2.h"

#include <ppltasks.h>
using namespace concurrency;

using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::UI::Core;
using namespace Windows::System;
using namespace Windows::Foundation;
using namespace Windows::Graphics::Display;
using namespace Windows::Phone::UI::Input;
using namespace Windows::ApplicationModel::Store;
using namespace Windows::System::Threading;

bool g_bWP8TrialStatusUpdated = false;

// Shoot engine hooks
HRESULT ShootCreateSwapChainForCoreWindow(IDXGIFactory2* pFactory, ID3D11Device1* pDevice, DXGI_SWAP_CHAIN_DESC1* pSwapChainDesc, IDXGISwapChain1** ppSwapChain)
{
	return pFactory->CreateSwapChainForCoreWindow(pDevice, reinterpret_cast<IUnknown*>(CoreWindow::GetForCurrentThread()), pSwapChainDesc, NULL, ppSwapChain);
}

extern void RegisterObjects();

// Method to convert a length in device-independent pixels (DIPs) to a length in physical pixels.
float PhoneDirect3DApp1::ConvertDipsToPixels(float dips)
{
	static const float dipsPerInch = 96.0f;
	return floor(dips * DisplayProperties::LogicalDpi / dipsPerInch + 0.5f); // Round to nearest integer.
}

PhoneDirect3DApp1::PhoneDirect3DApp1() :
	m_windowClosed(false),
	m_windowVisible(true),
	m_bBackButtonPressed(false),
	m_PointerId(0),
	m_bTouchInProgress(false)
{
}

void PhoneDirect3DApp1::Initialize(CoreApplicationView^ applicationView)
{	
	shoot::Log << "PhoneDirect3DApp1::Initialize\n";

	applicationView->Activated +=
		ref new TypedEventHandler<CoreApplicationView^, IActivatedEventArgs^>(this, &PhoneDirect3DApp1::OnActivated);

	CoreApplication::Suspending +=
		ref new EventHandler<SuspendingEventArgs^>(this, &PhoneDirect3DApp1::OnSuspending);

	CoreApplication::Resuming +=
		ref new EventHandler<Platform::Object^>(this, &PhoneDirect3DApp1::OnResuming);

	Platform::String^ dataPath = Windows::ApplicationModel::Package::Current->InstalledLocation->Path;
	std::wstring dataPathW(dataPath->Begin());
	std::string dataPathMB(dataPathW.begin(), dataPathW.end());
	shoot::File::SetBasePath(dataPathMB.c_str());
	
	Platform::String^ writablePath = Windows::Storage::ApplicationData::Current->LocalFolder->Path;
	std::wstring writablePathW(writablePath->Begin());
	std::string writablePathMB(writablePathW.begin(), writablePathW.end());
	shoot::File::SetWritablePath(writablePathMB.c_str());

	// read product ID from WMAppManifest.xml
	{
		FILE* pManifest = NULL;
		fopen_s(&pManifest, "WMAppManifest.xml", "rb");
		if(pManifest)
		{
			int end = fseek(pManifest, 0, SEEK_END);
			int size = ftell(pManifest);
			fseek(pManifest, 0, SEEK_SET);
			char* pData = new char[size+1];
			fread(pData, 1, size, pManifest);
			pData[size] = 0;
			tinyxml2::XMLDocument manifest;
			if(manifest.Parse(pData) == tinyxml2::XML_SUCCESS)
			{
				tinyxml2::XMLElement* pRoot = manifest.FirstChildElement();
				tinyxml2::XMLElement* pDefaultLanguage = pRoot ? pRoot->FirstChildElement() : NULL;
				tinyxml2::XMLElement* pApp = pDefaultLanguage ? pDefaultLanguage->NextSiblingElement() : NULL;		
				std::string strProductId = pApp ? pApp->Attribute("ProductID") : "";
				m_ProductId = std::wstring(strProductId.begin(), strProductId.end());				
			}
			fclose(pManifest);
			delete[] pData;
		}
	}
}

void PhoneDirect3DApp1::SetWindow(CoreWindow^ window)
{
	shoot::Log << "PhoneDirect3DApp1::SetWindow\n";	

	window->VisibilityChanged +=
		ref new TypedEventHandler<CoreWindow^, VisibilityChangedEventArgs^>(this, &PhoneDirect3DApp1::OnVisibilityChanged);

	window->Closed += 
		ref new TypedEventHandler<CoreWindow^, CoreWindowEventArgs^>(this, &PhoneDirect3DApp1::OnWindowClosed);

	window->PointerPressed +=
		ref new TypedEventHandler<CoreWindow^, PointerEventArgs^>(this, &PhoneDirect3DApp1::OnPointerPressed);

	window->PointerMoved +=
		ref new TypedEventHandler<CoreWindow^, PointerEventArgs^>(this, &PhoneDirect3DApp1::OnPointerMoved);

	window->PointerReleased +=
		ref new TypedEventHandler<CoreWindow^, PointerEventArgs^>(this, &PhoneDirect3DApp1::OnPointerReleased);
	
	shoot::Engine* pEngine = shoot::Engine::Instance();
	if(!pEngine)
	{
		pEngine = new shoot::GameEngine(&RegisterObjects, shoot::GraphicsDriver::DT_DirectX11, shoot::InputManager::IMT_Touch);
		pEngine->Init();
	}

	ResizeWindow();
}

void PhoneDirect3DApp1::Load(Platform::String^ entryPoint)
{
	shoot::Log << "PhoneDirect3DApp1::Load\n";
}

void PhoneDirect3DApp1::Run()
{
	BasicTimer^ timer = ref new BasicTimer();
	bool bExitRequested = false;

	while (!m_windowClosed && !bExitRequested)
	{
		if (m_windowVisible)
		{
			timer->Update();
			CoreWindow::GetForCurrentThread()->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessAllIfPresent);

			if(m_bBackButtonPressed)
			{
				shoot::InputManager::Instance()->SetKeyPressed(shoot::InputManager::KT_Back, true);				
			}

			if(shoot::GameManager::Instance()->HasGetFullGameRequest())
			{
				shoot::Log.Print("launchUriOperation..\n");
				OutputDebugStringW(m_ProductId.c_str());
				std::wstring uriStdStr = std::wstring(L"zune:navigate?appid=") + m_ProductId;
				auto uriStr = ref new Platform::String(uriStdStr.c_str());
				auto uri = ref new Windows::Foundation::Uri(uriStr);
				concurrency::task<bool> launchUriOperation(Launcher::LaunchUriAsync(uri));
				launchUriOperation.then([](bool success)
				{
					shoot::Log.Print("LaunchURI status: %d\n", success);
				});
				shoot::GameManager::Instance()->ResetGetFullGameRequest();
			}

			shoot::InputManager::Instance()->Update();
			bExitRequested = !shoot::Engine::Instance()->Update(timer->Delta);
			shoot::Engine::Instance()->Render();			
			shoot::GraphicsDriver::Instance()->Present(); // This call is synchronized to the display frame rate.			

			if(m_bBackButtonPressed)
			{
				shoot::InputManager::Instance()->SetKeyPressed(shoot::InputManager::KT_Back, false);
				m_bBackButtonPressed = false;
			}
		}
		else
		{
			CoreWindow::GetForCurrentThread()->Dispatcher->ProcessEvents(CoreProcessEventsOption::ProcessOneAndAllPending);
		}
	}
}

void PhoneDirect3DApp1::Uninitialize()
{
	shoot::Log << "PhoneDirect3DApp1::Uninitialize()\n";

	if(shoot::Engine::Instance())
	{
		delete shoot::Engine::Instance();
	}
}

void PhoneDirect3DApp1::OnVisibilityChanged(CoreWindow^ sender, VisibilityChangedEventArgs^ args)
{
	shoot::Log << "PhoneDirect3DApp1::OnVisibilityChanged()\n";

	m_windowVisible = args->Visible;

	if(m_windowVisible)
	{
		shoot::Engine::Instance()->OnResume();
	}
	else
	{
		shoot::Engine::Instance()->OnPause();
	}
}

void PhoneDirect3DApp1::OnWindowClosed(CoreWindow^ sender, CoreWindowEventArgs^ args)
{
	shoot::Log << "PhoneDirect3DApp1::OnWindowClosed()\n";

	m_windowClosed = true;
}

void PhoneDirect3DApp1::OnPointerPressed(CoreWindow^ sender, PointerEventArgs^ args)
{
	if(shoot::InputManager::Instance() && !m_bTouchInProgress)
	{
		shoot::InputManager::TouchState state;
		float X = ConvertDipsToPixels(args->CurrentPoint->Position.X);
		float Y = ConvertDipsToPixels(args->CurrentPoint->Position.Y);
		state.vPosition = shoot::Vector2(X, Y);
		state.bPressed = true;
		shoot::InputManager::Instance()->SetTouchState(state);
		m_PointerId = args->CurrentPoint->PointerId;
		m_bTouchInProgress = true;
	}	
}

void PhoneDirect3DApp1::OnPointerMoved(CoreWindow^ sender, PointerEventArgs^ args)
{
	if(shoot::InputManager::Instance() && (args->CurrentPoint->PointerId == m_PointerId))
	{
		shoot::InputManager::TouchState state;
		float X = ConvertDipsToPixels(args->CurrentPoint->Position.X);
		float Y = ConvertDipsToPixels(args->CurrentPoint->Position.Y);
		state.vPosition = shoot::Vector2(X, Y);
		state.bPressed = true;
		shoot::InputManager::Instance()->SetTouchState(state);
	}
}

void PhoneDirect3DApp1::OnPointerReleased(CoreWindow^ sender, PointerEventArgs^ args)
{
	if(shoot::InputManager::Instance() && (args->CurrentPoint->PointerId == m_PointerId))
	{
		shoot::InputManager::TouchState state;
		float X = ConvertDipsToPixels(args->CurrentPoint->Position.X);
		float Y = ConvertDipsToPixels(args->CurrentPoint->Position.Y);
		state.vPosition = shoot::Vector2(X, Y);
		state.bPressed = false;
		shoot::InputManager::Instance()->SetTouchState(state);
		m_bTouchInProgress = false;
	}
}

void PhoneDirect3DApp1::OnBackButtonPressed(Platform::Object^ sender, Windows::Phone::UI::Input::BackPressedEventArgs^ args)
{
	if(m_windowVisible && shoot::InputManager::Instance())
	{
		m_bBackButtonPressed = true;
		args->Handled = true;
	}	
}

void PhoneDirect3DApp1::OnActivated(CoreApplicationView^ applicationView, IActivatedEventArgs^ args)
{
	shoot::Log << "PhoneDirect3DApp1::OnActivated()\n";

	if(shoot::GameManager* pMgr = shoot::GameManager::Instance())
	{
		pMgr->SetWP8TrialMode(false);

		ThreadPool::RunAsync(ref new WorkItemHandler([](IAsyncAction^ operation)
		{
			bool bIsTrial = CurrentApp::LicenseInformation->IsTrial;
			shoot::Log.Print("bIsTrial %d\n", bIsTrial);
			shoot::GameManager::Instance()->SetWP8TrialMode(bIsTrial);
		}));
	}

	CoreWindow::GetForCurrentThread()->Activate();

	HardwareButtons::BackPressed += ref new EventHandler<BackPressedEventArgs^>(this, &PhoneDirect3DApp1::OnBackButtonPressed);  
}

void PhoneDirect3DApp1::OnSuspending(Platform::Object^ sender, SuspendingEventArgs^ args)
{
	shoot::Log << "PhoneDirect3DApp1::OnSuspending()\n";

	shoot::GraphicsDriver::Instance()->UnloadObjects();

	// Save app state asynchronously after requesting a deferral. Holding a deferral
	// indicates that the application is busy performing suspending operations. Be
	// aware that a deferral may not be held indefinitely. After about five seconds,
	// the app will be forced to exit.
	SuspendingDeferral^ deferral = args->SuspendingOperation->GetDeferral();	
	
	create_task([this, deferral]()
	{
		// Insert your code here.

		deferral->Complete();
	});
}
 
void PhoneDirect3DApp1::OnResuming(Platform::Object^ sender, Platform::Object^ args)
{	
	shoot::Log << "PhoneDirect3DApp1::OnResuming()\n";

	if(shoot::GameManager* pMgr = shoot::GameManager::Instance())
	{
		pMgr->SetWP8TrialMode(false);
		
		ThreadPool::RunAsync(ref new WorkItemHandler([](IAsyncAction^ operation)
		{
			bool bIsTrial = CurrentApp::LicenseInformation->IsTrial;
			shoot::Log.Print("bIsTrial %d\n", bIsTrial);
			shoot::GameManager::Instance()->SetWP8TrialMode(bIsTrial);
			g_bWP8TrialStatusUpdated = true;
		}));
	}

	// Restore any data or state that was unloaded on suspend. By default, data
	// and state are persisted when resuming from suspend. Note that this event
	// does not occur if the app was previously terminated.
	shoot::Engine::Instance()->RequestResourceReload();	
	ResizeWindow();
}

void PhoneDirect3DApp1::ResizeWindow()
{
	Windows::Foundation::Rect windowBounds = CoreWindow::GetForCurrentThread()->Bounds;
	float width = ConvertDipsToPixels(windowBounds.Width);
	float height = ConvertDipsToPixels(windowBounds.Height);
	shoot::Engine::Instance()->ResizeScreen(shoot::Size(shoot::s32(width), shoot::s32(height)));
}

IFrameworkView^ Direct3DApplicationSource::CreateView()
{
	return ref new PhoneDirect3DApp1();
}

[Platform::MTAThread]
int main(Platform::Array<Platform::String^>^)
{
	shoot::Log << "entering main loop..\n";
	auto direct3DApplicationSource = ref new Direct3DApplicationSource();	
	CoreApplication::Run(direct3DApplicationSource);
	shoot::Log << "exiting main loop..\n";
	return 0;
}