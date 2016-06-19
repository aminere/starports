/*

Amine Rehioui
Created: March 4th 2015

*/

#pragma once


namespace shoot
{	
	//! InputRecorder
	class InputRecorder : public Singleton<InputRecorder>
	{
		DECLARE_SINGLETON(InputRecorder);
	public:

		//! State
		enum E_State
		{
			S_Inactive,
			S_Recording,
			S_Replaying
		};

		//! DataObject
		class DataObject : public Object
		{
			DECLARE_OBJECT(DataObject, Object);

		public:

			//! Serialize
			void Serialize(PropertyStream& stream)
			{
				stream.Serialize("ContextStack", &m_ContextStack);
				stream.Serialize("ScreenSize", &m_ScreenSize);
				stream.Serialize("Datas", &m_Datas);
			}

			//! Data
			struct Data : ISerializableStruct
			{
				float time;
				bool bTouched;
				Vector2 pos;
				bool leftPressed;
				bool rightPressed;

				void Serialize(PropertyStream& stream)
				{
					stream.Serialize("Time", &time);
					stream.Serialize("Touched", &bTouched);
					stream.Serialize("Pos", &pos);
					stream.Serialize("leftPressed", &leftPressed);
					stream.Serialize("rightPressed", &rightPressed);
				}
			};

			IPath m_ContextStack;
			Vector2 m_ScreenSize;
			Array<Data> m_Datas;
		};

		//! Record
		void Record(const std::string&);

		//! Record
		void Replay(const std::string&, bool, std::function<void()>, std::function<void()>);

		//! StopRecording
		void StopRecording(const std::string&);

		//! StopReplay
		void StopReplay();

		//! Update
		void Update();

		//! TakeScreenshot
		void TakeScreenshot();

		//! GetState
		inline E_State GetState() const { return m_State; }

	private:

		E_State m_State;
		bool m_bPreviousPressed;
		float m_Time;
		uint m_ReplayIndex;
		std::function<void()> m_ReplayFinished;
		Reference<DataObject> m_DataObject;
		uint m_CurrentScreenshot;
		uchar* m_ScreenshotData;
		Size m_ScreenshotSize;
	};
}




