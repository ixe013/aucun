// test.cpp : Defines the entry point for the console application.
//

#include <windows.h>
#include <wincred.h>
#include <tchar.h>
#include <stdio.h>
#include "settings.h"
#include "unlockpolicy.h"
#include "debug.h"


void ErrorExit(DWORD dw) 
{ 
	//wchar_t buf[2048];
	LPVOID lpMsgBuf;

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | 
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		dw,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR) &lpMsgBuf,
		0, NULL );

	wprintf(L"failed with error %d: %s", dw, lpMsgBuf); 
	//OutputDebugString(buf);

	LocalFree(lpMsgBuf);
}

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
	SetLastError(0);
	*((HWND *)lParam) = hwnd;
	return FALSE;
}

int _tmain(int argc, _TCHAR* argv[])
{
	{
		DWORD pid, tid;
		HANDLE process, token;
		LUID luid;
		HWND hWnd = 0;

		EnumDesktopWindows(GetThreadDesktop(GetCurrentThreadId()), EnumWindowsProc, (LPARAM)&hWnd);

		if(GetLastError() == 0)
		{

			tid = GetWindowThreadProcessId(hWnd, &pid);
			process = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pid);
			if(OpenProcessToken(process, TOKEN_QUERY, &token))
			{
				GetLUIDFromToken(token, &luid);
				OutputGetSessionUserName(&luid);

				CloseHandle(token);
			}

			CloseHandle(process);
		}
	}

	if(argc > 1) for(int i=1; i<argc; ++i)
	{
//		wchar_t user[MAX_USERNAME];
//		wchar_t domain[MAX_DOMAIN];
		wchar_t passwd[MAX_PASSWORD];

		HANDLE token = 0;

		wprintf(L"Enter password for %s : ", argv[i]);
		if (_getws_s(passwd, MAX_PASSWORD) == passwd)
		{
			//if(ShouldUnlockForUser(L"HYDRO-YN4PUBYPI", argv[i], passwd))
			//if(ShouldUnlockForUser(domain, user, passwd))
			switch(ShouldUnlockForUser(L"", argv[i], passwd))
			{
			case eLetMSGINAHandleIt: wprintf(L"eLetMSGINAHandleIt\n"); break;
			case eUnlock: wprintf(L"eUnlock\n"); break;
			case eForceLogoff: wprintf(L"eForceLogoff\n"); break;
			}
		}
		else
		{
			break;
		}
	}


	return 0;
}

