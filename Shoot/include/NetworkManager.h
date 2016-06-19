/* 

Amine Rehioui
Created: September 21st 2013

*/

#pragma once


#include "NetResult.h"

namespace shoot
{	
	//! NetworkManager
	class NetworkManager : public Singleton<NetworkManager>
	{
		DECLARE_SINGLETON(NetworkManager);

	public:

		//! initializes the network manager
		void Init();

		static const int ReadBufferSize = 65536;

		//! sends data to a host
		void SendData(const char* strHostName, int port, uchar* pData, int size, NetResultCallback* pResultCallback);
			
	private:

		Thread* m_pNetworkThread;
		bool m_bActive;
		bool m_bNetworkSupported;

		//! state
		enum E_State
		{
			S_Idle,
			S_Connecting,
			S_SendingData,
			S_ReceivingData
		};

		//! operation
		struct Operation
		{
			//! constructor
			Operation();
			
			//! releases operation resources
			void Release();

			size_t m_SocketID;
			std::string m_HostName;
			int m_Port;
			uchar* m_pData;
			int m_DataSize;
			NetResultCallback* m_pResultCallback;
		};

		E_State m_eState;
		std::list<Operation> m_lOperations;
		Mutex* m_pMutex;
		uchar m_ReadBuffer[ReadBufferSize+1];

		//! remove HTTP markers
		void RemoveHTTPMarkers(const std::vector<uchar>& dataIn, std::vector<uchar>& dataOut);

		//! returns hexadecimal status
		bool IsHexadecimal(char c) const;

		//! called on a networkd error
		void OnError(const char* strError);

		//! Network thread
		static void* NetworkThread(void* pParam);
	};
}




