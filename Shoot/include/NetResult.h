/* 

Amine Rehioui
Created: September 21st 2013

*/

#pragma once


namespace shoot
{	
	//! NetResult
	struct NetResult
	{
		//! Status
		enum E_Status
		{
			S_Undefined,
			S_ConnectionError,			
			S_DataReceived,
		};

		NetResult()
			: m_Status(S_Undefined)
		{
		}

		E_Status m_Status;
		std::vector<uchar> m_Data;
	};

	//! net result callback
	typedef void NetResultCallback(NetResult& result);
}




