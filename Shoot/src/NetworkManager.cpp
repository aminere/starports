/* 

Amine Rehioui
Created: September 21st 2013

*/

#include "Shoot.h"

#include "NetworkManager.h"

#include "Thread.h"

#include "Mutex.h"

#ifndef SHOOT_NO_NETWORK
#if SHOOT_PLATFORM == SHOOT_PLATFORM_WIN32 || SHOOT_PLATFORM == SHOOT_PLATFORM_WP8
#define USE_WINSOCK2
#include <WS2tcpip.h>
#else
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h> 
#include <netinet/in.h>
#include <netdb.h>
#endif

namespace shoot
{
	//! Constructor
	NetworkManager::NetworkManager()
		: m_bActive(false)
	{
		m_pMutex = Mutex::Create();

#ifdef USE_WINSOCK2
		WSADATA WsaDat;
		m_bNetworkSupported = (WSAStartup(MAKEWORD(2, 2), &WsaDat) == 0);
#else
		m_bNetworkSupported = true;
#endif

		SHOOT_WARNING(m_bNetworkSupported, "NetworkManager: Initialization error\n");		
	}

	//! Destructor
	NetworkManager::~NetworkManager()
	{
		SHOOT_WARNING(!m_bActive, "NetworkManager: Shutting down while NetworkThread still active");
		
		if(!m_bActive)
		{
			for(std::list<Operation>::iterator it = m_lOperations.begin(); it != m_lOperations.end(); ++it)
			{
				(*it).Release();
			}
			m_lOperations.clear();
			sdelete(m_pMutex);

#ifdef USE_WINSOCK2
			WSACleanup();
#endif
			sdelete(m_pNetworkThread);
		}
	}

	//! initializes the network manager
	void NetworkManager::Init()
	{
		if(m_bNetworkSupported)
		{
			m_pNetworkThread = Thread::CreateThread();
			m_pNetworkThread->Start(&NetworkThread);
		}
	}

	//! sends data to a host
	void NetworkManager::SendData(const char* strHostName, int port, uchar* pData, int size, NetResultCallback* pResultCallback)
	{
		Operation operation;
		operation.m_SocketID = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
#ifdef USE_WINSOCK2
		DWORD msec = 5000;
		int rcvTimeOut = setsockopt(operation.m_SocketID, SOL_SOCKET, SO_RCVTIMEO, (const char*)&msec, sizeof(int));
		int sendTimeOut = setsockopt(operation.m_SocketID, SOL_SOCKET, SO_SNDTIMEO, (const char*)&msec, sizeof(int));
#else
		struct timeval timeout = { 5, 0 };
		int rcvTimeOut = setsockopt(operation.m_SocketID, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(struct timeval));
		int sendTimeOut = setsockopt(operation.m_SocketID, SOL_SOCKET, SO_SNDTIMEO, (char*)&timeout, sizeof(struct timeval));
#endif		
		if((operation.m_SocketID >= 0) && (rcvTimeOut == 0) && (sendTimeOut == 0))
		{
			operation.m_HostName = strHostName;
			operation.m_Port = port;
			operation.m_pData = snew uchar[size];
			memcpy(operation.m_pData, pData, size);
			operation.m_DataSize = size;
			operation.m_pResultCallback = pResultCallback;

			m_pMutex->Lock();
			m_lOperations.push_back(operation);
			m_pMutex->Unlock();
		}
		else
		{
			Log.Print("NetworkManager: error - socket creation failed, m_SocketID %d, rcvTimeOut %d, sendTimeOut %d\n", operation.m_SocketID, rcvTimeOut, sendTimeOut);
			NetResult result;
			result.m_Status = NetResult::S_ConnectionError;
			pResultCallback(result);
		}
	}

	//! called on a network error
	void NetworkManager::OnError(const char* strError)
	{
		Log.Print("NetworkManager: error - %s\n", strError);
		m_pMutex->Lock();
		Operation operation = m_lOperations.front();
		m_pMutex->Unlock();

		NetResult result;
		result.m_Status = NetResult::S_ConnectionError;
		if(operation.m_pResultCallback)
		{
			operation.m_pResultCallback(result);
		}

		operation.Release();
		m_pMutex->Lock();
		m_lOperations.pop_front();
		m_pMutex->Unlock();
	}

	//! Operation constructor
	NetworkManager::Operation::Operation()
		: m_SocketID(-1)
		, m_Port(0)
		, m_pResultCallback(NULL)
		, m_pData(NULL)
		, m_DataSize(0)
	{
	}

	//! releases operation resources
	void NetworkManager::Operation::Release()
	{
		if(m_SocketID >= 0)
		{
			shutdown(m_SocketID, 2);
#ifdef USE_WINSOCK2
			closesocket(m_SocketID);
#else
			close(m_SocketID);
#endif
		}
		sdelete_array(m_pData);
	}

	//! Network thread
	void* NetworkManager::NetworkThread(void* pParam)
	{
		Log << "NetworkManager: NetworkThread started\n";
		NetworkManager* pMgr = NetworkManager::Instance();
		pMgr->m_bActive = true;
		while(Engine::Instance()->IsRunning())
		{
			if(pMgr->m_lOperations.empty())
			{
				Thread::YieldThread();
				continue;
			}

			pMgr->m_pMutex->Lock();
			Operation operation = pMgr->m_lOperations.front();
			pMgr->m_pMutex->Unlock();

			struct hostent* server = gethostbyname(operation.m_HostName.c_str());
			if(!server)
			{
				pMgr->OnError("couldn't resolve host");
				continue;
			}

			struct sockaddr_in serv_addr;
			memset(&serv_addr, 0, sizeof(struct sockaddr_in));
			serv_addr.sin_family = AF_INET;
			serv_addr.sin_port = htons(operation.m_Port);
			memcpy((char *)&serv_addr.sin_addr.s_addr, (char*)server->h_addr, server->h_length);
			Log.Print("NetworkThread connect: m_SocketID(%d)\n", operation.m_SocketID);
			if(connect(operation.m_SocketID, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) != 0)
			{
				pMgr->OnError("couldn't connect to host");
				continue;
			}

			Log.Print("NetworkThread send: m_SocketID(%d)\n", operation.m_SocketID);
			if(send(operation.m_SocketID, (const char*)operation.m_pData,operation.m_DataSize, 0) != operation.m_DataSize)
			{
				pMgr->OnError("data send error");
				continue;
			}

			{
				std::vector<uchar> readData;
				while(Engine::Instance()->IsRunning())
				{
					Log.Print("NetworkThread recv: m_SocketID(%d)\n", operation.m_SocketID);
					int received = recv(operation.m_SocketID, (char*)pMgr->m_ReadBuffer, ReadBufferSize, 0);
					if(received < 0)
					{
						pMgr->OnError("data receive error");
						break;
					}

					if(received > 0)
					{
						auto currentSize = readData.size();
						readData.reserve(currentSize+received);
						std::copy(pMgr->m_ReadBuffer, pMgr->m_ReadBuffer+received, std::back_inserter(readData));
					}

					// HTTP hack to check if we received the end of the response.
					// TODO: make this check on the application side.
					int end = received-1;
					bool bHTPPEnd = false;
					char* buf = (char*)pMgr->m_ReadBuffer;
					if(end >= 3)
					{
						bHTPPEnd = (buf[end-3] == '\r' && buf[end-2] == '\n' && buf[end-1] == '\r' && buf[end] == '\n');
					}

					if(bHTPPEnd || received == 0)
					{
						NetResult result;
						pMgr->RemoveHTTPMarkers(readData, result.m_Data);							
						result.m_Status = NetResult::S_DataReceived;							
						if(operation.m_pResultCallback)
						{
							operation.m_pResultCallback(result);
						}

						operation.Release();
						pMgr->m_pMutex->Lock();
						pMgr->m_lOperations.pop_front();
						pMgr->m_pMutex->Unlock();
						break;
					}
				}
			}
		}
		pMgr->m_bActive = false;
		Log << "NetworkManager: NetworkThread finished\n";
		return NULL;
	}

	//! remove HTTP markers
	void NetworkManager::RemoveHTTPMarkers(const std::vector<uchar>& dataIn, std::vector<uchar>& dataOut)
	{
		bool bFoundCRLF = false;
		const int maxHeaderChars = 10;
		int i = 0;
		int copyOffset = 0;
		while(1 && i<int(dataIn.size())-1)
		{
			if(!bFoundCRLF)
			{
				if(dataIn[i] == '\r' && dataIn[i+1] == '\n')
				{
					bFoundCRLF = true;
					continue;
				}
				++i;
				continue;
			}
			else
			{
				int j=0;
				int afterCRLF = i+2;
				for(; j<maxHeaderChars && IsHexadecimal(dataIn[afterCRLF+j]); ++j);
				if(dataIn[afterCRLF+j] == '\r' && dataIn[afterCRLF+j+1] == '\n')
				{
					auto currentSize = dataOut.size();
					dataOut.reserve(currentSize+(i-copyOffset));
					std::copy(&dataIn[copyOffset], &dataIn[i], std::back_inserter(dataOut));

					i = afterCRLF + j + 2;
					copyOffset = i;
					bFoundCRLF = false;
					continue;
				}
				else
				{
					int beforeCRLF = i-1;
					for(int l=0; l<maxHeaderChars && IsHexadecimal(dataIn[beforeCRLF]); ++l, --beforeCRLF);					

					if(beforeCRLF > copyOffset)
					{
						auto currentSize = dataOut.size();
						dataOut.reserve(currentSize+(beforeCRLF-copyOffset));
						std::copy(&dataIn[copyOffset], &dataIn[beforeCRLF], std::back_inserter(dataOut));
					}

					i = i+2;
					copyOffset = i;
					bFoundCRLF = false;
					continue;
				}
			}
		}

		dataOut.push_back(0); // add string terminator
	}

	//! returns hexadecimal status
	bool NetworkManager::IsHexadecimal(char c) const
	{
		return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
	}
}

#endif // SHOOT_NO_NETWORK


