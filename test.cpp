// test.cpp : Defines the entry point for the console application.
//

#include <windows.h>
#include <wincred.h>
#include <tchar.h>
#include <stdio.h>
#include <security.h>
#include <lm.h>
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


int _tmain(int argc, _TCHAR* argv[])
{
	wchar_t unlock[MAX_GROUPNAME] = L"";

	OutputDebugString(L"TEST starting...\n");
/*
	if(GetGroupName(gUnlockGroupName, unlock, sizeof unlock / sizeof *unlock) == S_OK)
	{
		wchar_t caption[512];
		wchar_t text[2048];

		OutputDebugString(L"Group name ");
		OutputDebugString(unlock);
		OutputDebugString(L"\n");

		if((GetNoticeText(L"Caption", caption, sizeof caption / sizeof *caption) == S_OK)
		&& (GetNoticeText(L"Text", text, sizeof text / sizeof *text) == S_OK))
		{
			wchar_t message[MAX_USERNAME + sizeof text / sizeof *text];
			wchar_t *read = text;
			wchar_t *write = text;

			while(*read)
			{
				if((*read == '\\') && (*(read+1) == 'n'))
				{
					*write++ = '\n';
					read += 2;
				}
				else
				{
					*write++ = *read++;
				}
			}

			*write = 0;

			wsprintf(message, text, unlock); //Will insert group name if there is a %s in the message
			MessageBox(0, message, caption, MB_YESNOCANCEL|MB_ICONEXCLAMATION);
		}
	}
*/

	if(argc > 1) for(int i=1; i<argc; ++i)
	{
//		wchar_t user[MAX_USERNAME];
//		wchar_t domain[MAX_DOMAIN];
		wchar_t passwd[MAX_PASSWORD];

		HANDLE current_user = 0;

		//OpenThreadToken(GetCurrentThread(), TOKEN_READ, TRUE, &current_user);
		OpenProcessToken(GetCurrentProcess(), TOKEN_READ, &current_user);

		ExtractTokenOwner(current_user, passwd, 512);

		if(ShouldHookUnlockPasswordDialog(current_user))
		{
			wprintf(L"Should hook.\n");
		}

		wprintf(L"Enter password for %s : ", argv[i]);
		if (_getws_s(passwd, MAX_PASSWORD) == passwd)
		{
			//if(ShouldUnlockForUser(L"HYDRO-YN4PUBYPI", argv[i], passwd))
			//if(ShouldUnlockForUser(domain, user, passwd))
			switch(ShouldUnlockForUser(current_user, L"", argv[i], passwd))
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

		//CloseDesktop(desktop); //Not needed says MSDN
		CloseHandle(current_user); 
	}


	return 0;
}

