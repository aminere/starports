/* 

Amine Rehioui
Created: April 1st 2011

*/

#pragma once


namespace shoot
{
	//! Log class
	class BaseLog
	{
	public:

		//! Constructor
		BaseLog();

		//! Prints to available output streams
		void Print(const char* format, ...);

		//! Close
		void Close();

		//! Print operators
		BaseLog& operator << (const char*);
		BaseLog& operator << (const wchar_t*);
		BaseLog& operator << (size_t);
		BaseLog& operator << (float);
		BaseLog& operator << (const Vector3&);
		BaseLog& operator << (const Vector2&);

	private:

		//! Prints to available output streams
		void PrintInternal(const char* str);

		#if SHOOT_PLATFORM == SHOOT_PLATFORM_WIN32
		FILE* m_pLog;
		#endif
	};

	extern BaseLog Log;
}



