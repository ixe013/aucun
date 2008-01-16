#include <windows.h>
#include "Settings.h"
#include "UnlockPolicy.h"

HANDLE ConvertToImpersonationToken(HANDLE token)
{
	HANDLE result = token;

	SECURITY_IMPERSONATION_LEVEL sil;
	DWORD cbsil = sizeof sil;

	//If we are not impersonating
	if(GetTokenInformation(token, TokenImpersonationLevel, (LPVOID)&sil, sizeof sil, &cbsil) == 0)
	{
		HANDLE imptoken = 0;

		//Change to an impersonation token
		if(DuplicateToken(token, SecurityIdentification, &imptoken))
		{
			result = imptoken;
			CloseHandle(token);
		}
	}

	return result;
}

EXTERN int ShouldUnlockForUser(const wchar_t *domain, const wchar_t *username, const wchar_t *password)
{
	int result = eLetMSGINAHandleIt; //secure by default
	HANDLE token = 0;

	wchar_t unlock[MAX_GROUPNAME] = L"";
	wchar_t logoff[MAX_GROUPNAME] = L"";

	GetGroupName(gUnlockGroupName, unlock, sizeof unlock / sizeof *unlock);
	GetGroupName(gForceLogoffGroupName, logoff, sizeof logoff / sizeof *logoff);

	//Do we have anything to work with ?
	if(*unlock || *logoff)
	{
		//Let's see if we can authenticate the user (this will generate a event log entry if the policy requires it)
		if (LogonUser(username, domain, password, LOGON32_LOGON_UNLOCK, LOGON32_PROVIDER_DEFAULT, &token))
		{
			PSID current_sid, token_sid;
			HANDLE current_user;
			token = ConvertToImpersonationToken(token);

			current_user = GetCurrentLoggedOnUserToken();

			current_sid = token_sid = 0; //Fix me

			if(EqualSid(current_sid, token_sid))
			{
				result = eUnlock;
			}
			else if(UsagerEstDansGroupe(token, logoff) == S_OK)
			{
				result = eForceLogoff;
			}
			else if(UsagerEstDansGroupe(token, unlock) == S_OK)
			{
				result = eUnlock;
			}

			CloseHandle(token);
		}
	}

	return result;
}


//----------------------------------------------------------------------
// Name        : UsagerEstDansGroupe
// Description : Genere un SID a partir du nom de groupe puis verifie
//               l'appartenance au groupe de l'usager. Si on nom d'usager
//               est passé au lieu du nom de groupe, cette fonction le
//               traite comme un groupe ne contenant que cet usager.
//               Attention au leaks si vous jouez la dedans... les SID
//               c'est pas joli
// Parametre   : [in] HANDLE usager
// Parametre   : [in] BSTR groupe
// Returns     : S_OK si l'usager est dans le groupe
//               S_FALSE si l'usager n'est pas dans le groupe
//               E_FAIL s'il y a un bobo (on ne trouve pas le groupe)
//----------------------------------------------------------------------
HRESULT UsagerEstDansGroupe(HANDLE usager, const wchar_t *groupe)
{
	HRESULT result = E_FAIL;
	SID_NAME_USE snu;
	WCHAR szDomain[256];
	DWORD dwSidSize =0;

	DWORD dwSize = sizeof szDomain / sizeof *szDomain;

	if ((LookupAccountNameW(NULL, groupe, 0, &dwSidSize, szDomain, &dwSize, &snu) == 0)
		&& (ERROR_INSUFFICIENT_BUFFER == GetLastError()))
	{
		SID *pSid = (SID*)malloc(dwSidSize);

		if (LookupAccountNameW(NULL, groupe, pSid, &dwSidSize, szDomain, &dwSize, &snu))
		{
			BOOL b;

			if (CheckTokenMembership(usager, pSid, &b) && (b == TRUE))
			{
				result = S_OK;
			}
			else
			{
				result = S_FALSE;
			}
		}

		//Si tout vas bien (la presque totalitée des cas), on delete notre pointeur
		//avec le bon operateur.
		free(pSid);
	}

	return result;
}

//Stupid helper function to get the first window sent to us
BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
	BOOL result = TRUE;
	wchar_t buf[512];
	wsprintf(buf, L"Enun Window 0x%08X ", hwnd);

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



BOOL CALLBACK EnumWindowsProcFindVisible(HWND hwnd, LPARAM lParam)
{
	static int nbhidden = 0;
	BOOL result = TRUE;
	wchar_t buf[512];
	wsprintf(buf, L"    Enun Window 0x%08p ", hwnd);
	OutputDebugString(buf);

	//Any visible window will do.
	if(IsWindowVisible(hwnd))
	{
		//We fail simply because we don't need to iterate over every single window. 
		OutputDebugString(L"is visible\n");

		*((HWND*)lParam) = hwnd;

		SetLastError(0); 

		result = FALSE;
	}
	else 
	{
		OutputDebugString(L"is not visible\n");
	}

	return result;
}


BOOL CALLBACK EnumDesktopProc(LPTSTR lpszDesktop, LPARAM lParam)
{
	HDESK desktop;
	wchar_t buf[512];

	wsprintf(buf, L"  Desktop %s\n", lpszDesktop);
	OutputDebugString(buf);

	//No need to enumerate Winlogon desktop windows
	if(_wcsicmp(L"Winlogon", lpszDesktop))
	{
		//A real desktop, usually "Default"
		desktop = OpenDesktop(lpszDesktop, 0, FALSE, GENERIC_READ);

		if(desktop)
		{
			EnumDesktopWindows(desktop, EnumWindowsProcFindVisible, lParam);
			CloseDesktop(desktop);
		}
	}
	
	return TRUE;
}



HANDLE GetCurrentLoggedOnUserToken()
{
	HANDLE result = 0;
	HWND hWnd = 0;
	HWINSTA winsta;

	winsta = GetProcessWindowStation();

	if(winsta)
	{
		{
			wchar_t buf[512];
			DWORD l = sizeof buf / sizeof *buf;
			if(GetUserObjectInformation(winsta, UOI_NAME, buf, l, &l))
			{
				OutputDebugString(L"Current window station ");
				OutputDebugString(buf);
				OutputDebugString(L"\n");
			}
		}
		EnumDesktops(winsta, EnumDesktopProc, (LPARAM)&hWnd);
		CloseWindowStation(winsta);
	}

	if(hWnd)
	{
		DWORD pid, tid;
		HANDLE process;

		tid = GetWindowThreadProcessId(hWnd, &pid);
		process = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pid);

		if(process)
		{
			HANDLE token;
			if(OpenProcessToken(process, TOKEN_QUERY|TOKEN_DUPLICATE, &token))
			{
				/*
				wchar_t excluded[MAX_GROUPNAME] = L"";

				LUID luid = {0};
				GetLUIDFromToken(token, &luid);
				OutputGetSessionUserName(&luid);

				GetGroupName(gExcludedGroupName, excluded, sizeof *excluded);
				if(UsagerEstDansGroupe(token, excluded) != S_OK)
				{
					//User is not blacklisted, let's hook the dialog
					gCurrentDlgIndex = i;
					pfWlxWkstaLockedSASDlgProc = dlgprc;
					proc2use = MyWlxWkstaLockedSASDlgProc; //Use our proc instead
					OutputDebugString(L"Hooked!\n");
					break;
				}
				else OutputDebugString(L"Pas dans groupe \n");
				//*/

				result = token;
			}
			CloseHandle(process);
		}
	}

	return result;
}