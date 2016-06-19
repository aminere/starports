/* 

Amine Rehioui
Created: March 4th 2015

*/

#include "Shoot.h"

#include "InputRecorder.h"
#include "Thread.h"
#include "FileSystem.h"

//#if SHOOT_PLATFORM == SHOOT_PLATFORM_WIN32
//#include "FreeImage.h"
//#endif

namespace shoot
{
	DEFINE_OBJECT(InputRecorder::DataObject);

	//! constructor
	InputRecorder::InputRecorder()
		: m_State(S_Inactive)
		, m_ScreenshotData(NULL)
	{
	}

	//! destructor
	InputRecorder::~InputRecorder()
	{
		sdelete_array(m_ScreenshotData);
	}

	//! Record
	void InputRecorder::Record(const std::string& contextStack)
	{
		m_DataObject = snew DataObject();
		m_DataObject->m_ContextStack = contextStack;
		auto screenSize = GraphicsDriver::Instance()->GetViewPort().Size();
		m_DataObject->m_ScreenSize = screenSize;
		m_bPreviousPressed = false;
		m_Time = 0.0f;
		m_State = S_Recording;
	}

	//! Record
	void InputRecorder::Replay(const std::string& data, bool immediate, std::function<void()> loadingFinished, std::function<void()> replayFinished)
	{
		m_DataObject = static_cast<DataObject*>(ObjectManager::Instance()->Load(data));
		Engine::Instance()->LoadContextStack(m_DataObject->m_ContextStack, immediate, loadingFinished);
		m_ReplayFinished = replayFinished;
		m_Time = 0.0f;
		m_ReplayIndex = 0;
		m_CurrentScreenshot = 0;
		m_State = S_Replaying;

		#if SHOOT_PLATFORM == SHOOT_PLATFORM_WIN32
			CreateDirectory(L"replay", NULL);
			auto screenSize = GraphicsDriver::Instance()->GetViewPort().Size();
			m_ScreenshotSize = Size((int)screenSize.X, (int)screenSize.Y);
			m_ScreenshotSize.Width = Math::GetNextPow2(m_ScreenshotSize.Width);
			m_ScreenshotSize.Height = Math::GetNextPow2(m_ScreenshotSize.Height);
			sdelete_array(m_ScreenshotData);
			m_ScreenshotData = new uchar[3 * m_ScreenshotSize.Width * m_ScreenshotSize.Height];
		#endif
	}

	//! StopRecording
	void InputRecorder::StopRecording(const std::string& outPath)
	{
		m_DataObject->SaveToXML(outPath.c_str());
		m_DataObject.Clear();
		m_State = S_Inactive;
	}

	//! StopReplay
	void InputRecorder::StopReplay()
	{
		m_DataObject.Clear();
		m_State = S_Inactive;
		m_ReplayFinished();
		sdelete_array(m_ScreenshotData);
	}

	//! Update
	void InputRecorder::Update()
	{
		if (m_State == S_Inactive)
			return;

		switch (m_State)
		{
		case S_Recording:
		{
			const auto& touchState = InputManager::Instance()->GetRawTouchState();
			auto data = snew DataObject::Data();
			data->bTouched = touchState.bPressed;
			data->pos = touchState.vPosition;
			data->time = m_Time;
			data->leftPressed = InputManager::Instance()->IsKeyPressed(InputManager::KT_Left);
			data->rightPressed = InputManager::Instance()->IsKeyPressed(InputManager::KT_Right);
			m_DataObject->m_Datas.Add(data);			
		}
		break;

		case S_Replaying:
		{
			g_fDeltaTime = 1.0f / 60.0f;
			auto& datas = m_DataObject->m_Datas;
			if (m_ReplayIndex < datas.GetSize())
			{
				if (m_Time >= datas[m_ReplayIndex].time)
				{
					InputManager::TouchState state;
					state.bPressed = datas[m_ReplayIndex].bTouched;
					state.vPosition = datas[m_ReplayIndex].pos;

					// transform to current screen size
					auto screenSize = GraphicsDriver::Instance()->GetViewPort().Size();
					auto localSize = state.vPosition * (screenSize / m_DataObject->m_ScreenSize);
					state.vPosition = localSize;

					InputManager::Instance()->SetTouchState(state);
					InputManager::Instance()->SetKeyPressed(InputManager::KT_Left, datas[m_ReplayIndex].leftPressed);
					InputManager::Instance()->SetKeyPressed(InputManager::KT_Right, datas[m_ReplayIndex].rightPressed);
					++m_ReplayIndex;
				}
			}
			else
			{
				StopReplay();
			}
		}
		break;
		}

		m_Time += g_fDeltaTime;
	}

	//! TakeScreenshot
	void InputRecorder::TakeScreenshot()
	{
		#if SHOOT_PLATFORM == SHOOT_PLATFORM_WIN32
			//auto path = Utils::StringFormat("replay/%03d.png", m_CurrentScreenshot);
			//glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			//glReadPixels(0, 0, m_ScreenshotSize.Width, m_ScreenshotSize.Height, GL_RGB, GL_UNSIGNED_BYTE, m_ScreenshotData);
			//FIBITMAP* image = FreeImage_ConvertFromRawBits(m_ScreenshotData, m_ScreenshotSize.Width, m_ScreenshotSize.Height, 3 * m_ScreenshotSize.Width, 24, 0x0000FF, 0xFF0000, 0x00FF00, false);
			//FreeImage_Save(FIF_BMP, image, path.c_str(), 0);
			//FreeImage_Unload(image);
			//++m_CurrentScreenshot;
		#endif		
	}
}

