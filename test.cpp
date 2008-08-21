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
#include "trace.h"
#include "debug.h"
#include "SecurityHelper.h"

//BEGIN - Found on Google Code search, now 404 http://www.cybertech.net/~sh0ksh0k/projects_new/TRET-1.0-PUBSRC/Shared/utils/security.c
BOOL EnablePrivilegeInToken(HANDLE hToken, const wchar_t *PrivilegeName)
{
	LUID Privilege;
	TOKEN_PRIVILEGES TokenPrivileges;

	if (!LookupPrivilegeValue(NULL, PrivilegeName, &Privilege)) return FALSE;
	TokenPrivileges.PrivilegeCount = 1;
	TokenPrivileges.Privileges[0].Luid = Privilege;
	TokenPrivileges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	if (!AdjustTokenPrivileges(hToken, FALSE, &TokenPrivileges, sizeof(TokenPrivileges), NULL, NULL)) return FALSE;
	else if (GetLastError() == ERROR_NOT_ALL_ASSIGNED) return FALSE;
	return TRUE;
}



BOOL EnablePrivilege(const wchar_t *PrivilegeName)
{
	BOOL Result;
	HANDLE hToken;
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) return FALSE;
	Result = EnablePrivilegeInToken(hToken, PrivilegeName);
	CloseHandle(hToken);
	return Result;
}

BOOL DisablePrivilegeInToken(HANDLE hToken, const wchar_t *PrivilegeName)
{
	LUID Privilege;
	TOKEN_PRIVILEGES TokenPrivileges;

	if (!LookupPrivilegeValue(NULL, PrivilegeName, &Privilege)) return FALSE;
	TokenPrivileges.PrivilegeCount = 1;
	TokenPrivileges.Privileges[0].Luid = Privilege;
	TokenPrivileges.Privileges[0].Attributes = 0;
	if (!AdjustTokenPrivileges(hToken, FALSE, &TokenPrivileges, sizeof(TokenPrivileges), NULL, NULL)) return FALSE;
	else if (GetLastError() == ERROR_NOT_ALL_ASSIGNED) return FALSE;
	return TRUE;
}

BOOL DisablePrivilege(const wchar_t *PrivilegeName)
{
	BOOL Result;
	HANDLE hToken;
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) return FALSE;
	Result = DisablePrivilegeInToken(hToken, PrivilegeName);
	CloseHandle(hToken);
	return Result;
}


//END - Found on Google Code search, now 404 http://www.cybertech.net/~sh0ksh0k/projects_new/TRET-1.0-PUBSRC/Shared/utils/security.c


int _tmain(int argc, _TCHAR* argv[])
{
	int result = -1;

	//wchar_t unlock[MAX_GROUPNAME] = L"";
	//HANDLE lsa;

	TRACE(L"-------------------------\n");

	//EnablePrivilege(L"SeTcbPrivilege");
	//if(!RegisterLogonProcess(LOGON_PROCESS_NAME, &lsa))
	//	TRACEMSG(GetLastError());
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

		OpenProcessToken(GetCurrentProcess(), TOKEN_READ, &current_user);

		if(ShouldHookUnlockPasswordDialog(current_user))
		{
			TRACE(L"Should hook.\n");
		}

		if (_getws_s(passwd, MAX_PASSWORD) == passwd)
		{
			result = ShouldUnlockForUser(current_user, L"", argv[i], passwd);

			switch(result)
			{
			case eLetMSGINAHandleIt: 
				TRACE(L"TEST result is eLetMSGINAHandleIt\n"); 
				wprintf(L"Actual result   : eLetMSGINAHandleIt\n");
				break;
			case eUnlock: 
				TRACE(L"TEST result is eUnlock\n"); 
				wprintf(L"Actual result   : eUnlock\n"); 
				break;
			case eForceLogoff: 
				TRACE(L"TEST result is eForceLogoff\n"); 
				wprintf(L"Actual result   : eForceLogoff\n"); 
				break;
			}
		}
		else
		{
			TRACE(L"Unable to read password\n");
			break;
		}

		CloseHandle(current_user); 
	}

	//LsaDeregisterLogonProcess(lsa);
	//DisablePrivilege(L"SeTcbPrivilege");

	return result;
}

