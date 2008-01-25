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

//Taken from Keith Brown Full Ginal Sample on MSJ (or MSDN mag, whatever)
HRESULT IsSameUser(HANDLE hToken1, HANDLE hToken2) 
{
	HRESULT result = E_FAIL; 

    BYTE buf1[sizeof(TOKEN_USER) + SECURITY_MAX_SID_SIZE];
    BYTE buf2[sizeof(TOKEN_USER) + SECURITY_MAX_SID_SIZE];

    DWORD cb;
    if (GetTokenInformation(hToken1, TokenUser, buf1, sizeof buf1, &cb) &&
		GetTokenInformation(hToken2, TokenUser, buf2, sizeof buf2, &cb)) 
	{
        if(EqualSid(((TOKEN_USER*)buf1)->User.Sid, ((TOKEN_USER*)buf2)->User.Sid))
		{
			result = S_OK;
		}
		else
		{
			result = S_FALSE;
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

	//Get the groups early to ensure fail fast if GINA is not configured
	GetGroupName(gUnlockGroupName, unlock, sizeof unlock / sizeof *unlock);
	GetGroupName(gForceLogoffGroupName, logoff, sizeof logoff / sizeof *logoff);

	//Do we have anything to work with ?
	if(*unlock || *logoff)
	{
		//Let's see if we can authenticate the user (this will generate a event log entry if the policy requires it)
		if (LogonUser(username, domain, password, LOGON32_LOGON_UNLOCK, LOGON32_PROVIDER_DEFAULT, &token))
		{
			HRESULT is_same_user;
			HANDLE current_user;
			token = ConvertToImpersonationToken(token);

			current_user = GetCurrentLoggedOnUserToken();

			is_same_user = IsSameUser(current_user, token);

			if(is_same_user == S_OK)
			{
				result = eUnlock; 
			}
			else if(is_same_user == S_FALSE)
			{
				if(UsagerEstDansGroupe(token, unlock) == S_OK)
				{
					result = eUnlock;
				}
				else if(UsagerEstDansGroupe(token, logoff) == S_OK)
				{
					result = eForceLogoff;
				}
			}

			CloseHandle(token);
			CloseHandle(current_user);
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
#ifdef _DEBUG
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
#endif
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
				result = token;
			}
			CloseHandle(process);
		}
	}

	return result;
}

BOOLEAN ShouldHookUnlockPasswordDialog(HANDLE token)
{
	BOOLEAN result = FALSE;

	wchar_t unlock[MAX_GROUPNAME] = L"";
	wchar_t excluded[MAX_GROUPNAME] = L"";
	wchar_t forcelogoff[MAX_GROUPNAME] = L"";

	//If there is either an unlock or force logoff group, 
	if((GetGroupName(gUnlockGroupName, unlock, sizeof unlock / sizeof *unlock) == S_OK)
	|| (GetGroupName(gForceLogoffGroupName, forcelogoff, sizeof forcelogoff / sizeof *forcelogoff) == S_OK))
	{
		//User must not be in the excluded group
		if(GetGroupName(gExcludedGroupName, excluded, sizeof excluded / sizeof *excluded) == S_OK)
		{
			//If is not blacklisted, return TRUE (so the dialog will be hooked)
			result = UsagerEstDansGroupe(token, excluded) == S_FALSE;
		}
	}

	return result;
}
