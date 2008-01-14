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

BOOL CALLBACK EnumAnyWindowsProc(HWND hwnd, LPARAM lParam)
{
	*((HWND *)lParam) = hwnd;

	//We fail simply because we don't need to iterate over every single window. 
	SetLastError(0); 

	return FALSE;
}

BOOL CALLBACK EnumWindowsProcFindVisible(HWND hwnd, LPARAM lParam)
{
	BOOL result = TRUE;
	wchar_t buf[512];
	wsprintf(buf, L"    Enun Window 0x%08X ", hwnd);

	OutputDebugString(buf);

	*((HWND *)lParam) = hwnd;

	//Any visible window will do.
	if(IsWindowVisible(hwnd))
	{

		//We fail simply because we don't need to iterate over every single window. 
		OutputDebugString(L"is visible\n");

		SetLastError(0); 


		result = FALSE;
	}
	else OutputDebugString(L"is not visible\n");

	return result;
}

void FindVisibleWindowFromDesktop(HDESK desktop)
{
	DWORD pid, tid;
	HANDLE process, token;
	LUID luid;
	HWND hWnd = 0;
	wchar_t buf[512];

	GetUserObjectInformation(desktop, UOI_NAME, buf, sizeof buf / sizeof *buf, &pid);

	OutputDebugString(buf);
	OutputDebugString(L"\n");

	EnumDesktopWindows(desktop, EnumAnyWindowsProc, (LPARAM)&hWnd);

	if(hWnd)
	{
		HDESK current_desktop;
		HWINSTA winsta;

		current_desktop = GetThreadDesktop(GetCurrentThreadId());

		tid = GetWindowThreadProcessId(hWnd, &pid);
		process = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pid);

		SwitchDes

		winsta = GetProcessWindowStation();
		
		if(OpenProcessToken(process, TOKEN_QUERY, &token))
		{
			GetLUIDFromToken(token, &luid);
			OutputGetSessionUserName(&luid);

			CloseHandle(token);
		}

		CloseHandle(process);
	}
}

int _tmain(int argc, _TCHAR* argv[])
{
	FindVisibleWindowFromDesktop(GetThreadDesktop(GetCurrentThreadId()));

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

