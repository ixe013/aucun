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
	HMODULE msgina;

	msgina = LoadLibraryEx(L"msgina.dll", 0, LOAD_LIBRARY_AS_DATAFILE);

	if(msgina)
	{
		TCHAR szFormat[1024];
		TCHAR buf[2048];
		wchar_t caption[512];
		LPSTR a, b, c;
		TCHAR realname[MAX_USERNAME] = L"";
		DWORD nb_realname = sizeof realname / sizeof *realname;
		WKSTA_USER_INFO_1 *userinfo = 0;

		UINT ids = 1620;

		HANDLE token = GetCurrentLoggedOnUserToken();

		a = b = c = 0;

		if(ImpersonateLoggedOnUser(token))
		{

			GetUserNameEx(NameDisplay, realname, &nb_realname);

			NetWkstaUserGetInfo(0, 1, (LPBYTE*)&userinfo);		

			RevertToSelf(); //We are done with this

			//What information do we have ?
			if(*userinfo->wkui1_logon_domain && *userinfo->wkui1_username && *realname)
			{
				//
				ids = 1500;
				a = userinfo->wkui1_logon_domain;
				b = userinfo->wkui1_username;
				c = (LPSTR)realname;
			}
			else if(*userinfo->wkui1_logon_domain && *userinfo->wkui1_username)
			{
				ids = 1550;
				a = userinfo->wkui1_logon_domain;
				b = userinfo->wkui1_username;
				c = 0;
			}
			else if(*userinfo->wkui1_username && *realname)
			{
				ids = 1610;
				a = userinfo->wkui1_username;
				b = (LPSTR)realname;
				c = 0;
			}
			else if(*userinfo->wkui1_username)
			{
				ids = 1611;
				a = userinfo->wkui1_username;
				b = 0;
				c = 0;
			}
		}

		CloseHandle(token);

		LoadString(msgina, 1518, caption, sizeof caption / sizeof *caption);
		LoadString(msgina, ids, szFormat, sizeof szFormat / sizeof *szFormat);

		wsprintf(buf, szFormat, a, b, c);

		if(userinfo)
			NetApiBufferFree(userinfo);


		FreeLibrary(msgina);
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

