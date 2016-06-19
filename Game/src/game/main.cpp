/* 

Amine Rehioui
Created: February 21th 2010

*/

#include "Game.h"

#include "GameEngine.h"
#include "GameApp.h"

#include "APIHook.h"

#include <Dbghelp.h>
#include <iostream>
#include <tchar.h>
#include <strsafe.h>
#include <shlobj.h>

#pragma comment(lib, "DbgHelp")
#define MAX_BUFF_SIZE 1024

//! Entity registration function
extern void RegisterObjects();

LONG WINAPI RedirectedSetUnhandledExceptionFilter(EXCEPTION_POINTERS* ExceptionInfo)
{
	// When the CRT calls SetUnhandledExceptionFilter with NULL parameter
	// our handler will not get removed.
	return 0;
}

void make_minidump(EXCEPTION_POINTERS* e)
{
	HANDLE hFile = CreateFile("minidump.dmp", GENERIC_WRITE, FILE_SHARE_READ, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	if (hFile == INVALID_HANDLE_VALUE)
		return;

	MINIDUMP_EXCEPTION_INFORMATION exceptionInfo;
	exceptionInfo.ThreadId = GetCurrentThreadId();
	exceptionInfo.ExceptionPointers = e;
	exceptionInfo.ClientPointers = FALSE;

	MiniDumpWriteDump(
		GetCurrentProcess(),
		GetCurrentProcessId(),
		hFile,
		MINIDUMP_TYPE(MiniDumpNormal),
		e ? &exceptionInfo : NULL,
		NULL,
		NULL);

	if (hFile)
	{
		CloseHandle(hFile);
		hFile = NULL;
	}
	return;
}

LONG WINAPI OurSetUnhandledExceptionFilter(EXCEPTION_POINTERS* ExceptionInfo)
{
	shoot::Log.Print("GOTCHA!\n");
	make_minidump(ExceptionInfo);
	return EXCEPTION_CONTINUE_SEARCH;
}

int main(int argc, char** argv)
{
	::SetUnhandledExceptionFilter(OurSetUnhandledExceptionFilter);
	CAPIHook apiHook("kernel32.dll", "SetUnhandledExceptionFilter", (PROC)RedirectedSetUnhandledExceptionFilter);

	{
		shoot::GameEngine* pEngine = new shoot::GameEngine(&RegisterObjects);
		shoot::GameApp app(shoot::Size(1024, 768), true, true);

		pEngine->Init();

		app.Run();
	}

	return 0;
}

