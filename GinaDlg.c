#include <windows.h>
#include <winwlx.h>
#include <assert.h>
#include <lm.h>

#include "Ginahook.h"
#include "Settings.h"
#include "UnlockPolicy.h"

#include "global.h"
#include "debug.h"


typedef struct
{
	int dlgid;
	int IDC_USERNAME;
	int IDC_PASSWORD;
	int IDC_DOMAIN;
}
DialogAndControlsID;

static const DialogAndControlsID gDialogsAndControls[] =
{
	//Windows 2000 (never tested, taken from MS HookGina Sample)
	{ 1850, 1453, 1454, 1455 },
	//Windows Server 2003 (and probably XP SP2, never tested)
	{ 1950, 1953, 1954, 1956 },
};

static const int nbDialogsAndControlsID = sizeof gDialogsAndControls / sizeof *gDialogsAndControls;

//
// Pointers to redirected functions.
//

static PWLX_MESSAGE_BOX pfWlxMessageBox = NULL;
static PWLX_DIALOG_BOX_PARAM pfWlxDialogBoxParam = NULL;

static int gCurrentDlgIndex = -1;

//
// Pointers to redirected dialog box.
//

static DLGPROC pfWlxWkstaLockedSASDlgProc = NULL;

//
// Local functions.
//

int WINAPI MyWlxMessageBox(HANDLE, HWND, LPWSTR, LPWSTR, UINT);
int WINAPI MyWlxDialogBoxParam(HANDLE, HANDLE, LPWSTR, HWND, DLGPROC, LPARAM);


//
// Hook WlxDialogBoxParam() dispatch function.
//
void HookWlxDialogBoxParam(PVOID pWinlogonFunctions, DWORD dwWlxVersion)
{
	//WlxDialogBoxParam
	pfWlxDialogBoxParam = ((PWLX_DISPATCH_VERSION_1_0) pWinlogonFunctions)->WlxDialogBoxParam;
	((PWLX_DISPATCH_VERSION_1_0) pWinlogonFunctions)->WlxDialogBoxParam = MyWlxDialogBoxParam;

	//WlxMessageBox
	pfWlxMessageBox = ((PWLX_DISPATCH_VERSION_1_0) pWinlogonFunctions)->WlxMessageBox;
	((PWLX_DISPATCH_VERSION_1_0) pWinlogonFunctions)->WlxMessageBox = MyWlxMessageBox;
}

BOOLEAN GetDomainUsernamePassword(HWND hwndDlg, wchar_t *domain, int nbdomain, wchar_t *username, int nbusername, wchar_t *password, int nbpassword)
{
	BOOLEAN result = FALSE;

	if((gCurrentDlgIndex >= 0) && (gCurrentDlgIndex < nbDialogsAndControlsID))
	{
		if((GetDlgItemText(hwndDlg, gDialogsAndControls[gCurrentDlgIndex].IDC_PASSWORD, password, nbpassword) > 0)
			&& (GetDlgItemText(hwndDlg, gDialogsAndControls[gCurrentDlgIndex].IDC_USERNAME, username, nbusername) > 0))
		{
			result = TRUE; //That's enough to keep going. Let's try the domain nonetheless

			GetDlgItemText(hwndDlg, gDialogsAndControls[gCurrentDlgIndex].IDC_DOMAIN, domain, nbdomain);
		}
	}

	return result;
}

//
// Redirected WlxWkstaLockedSASDlgProc().
//
INT_PTR CALLBACK MyWlxWkstaLockedSASDlgProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	INT_PTR bResult = FALSE;
	static int level = 0;

	/*
	{
		wchar_t buf[512];
		int i;

		for(i=0; i<level; ++i)
		{
			buf[(i*3)+0] = ' ';
			buf[(i*3)+1] = ' ';
			buf[(i*3)+2] = ' ';
		}

		wsprintf(buf+(i*3), L"%s (0x%08X) wParam=0x%08X lParam=0x%08X\n", GetWindowsMessageName(uMsg), uMsg, wParam, lParam);
		OutputDebugString(buf);

	}
	//*/

	++level;

	// We hook a click on OK
	if ((uMsg == WM_COMMAND) && (wParam == IDOK))
	{
		wchar_t rawdomain[MAX_DOMAIN];
		wchar_t rawusername[MAX_USERNAME];
		wchar_t password[MAX_PASSWORD];

		//Get the username and password for this particular Dialog template
		if(GetDomainUsernamePassword(hwndDlg, rawdomain, sizeof rawdomain / sizeof *rawdomain, 
											  rawusername, sizeof rawusername / sizeof *rawusername, 
											  password, sizeof password / sizeof *password))
		{
			wchar_t *username = 0;
			wchar_t *domain = 0;

			//Replace this hack with CredUIParseUserName
			username = wcsstr(rawusername, L"\\");

			//For some reason, LogonUser does not accept the UPN notation. I parse it.
			//if(!username)
			//{
			//	username = wcsstr(rawusername, L"@");
			//}

			if(username)
			{
				domain = rawusername;
				*username++ = 0; //Null terminate the domain name and skip the separator
			}
			else
			{
				username = rawusername; //No domain entered, so point directly to the supplied buffer
				if(*rawdomain)
					domain = rawdomain;
			}

			if (*username && *password)
			{
				switch(ShouldUnlockForUser(domain, username, password))
				{
				case eForceLogoff:
					//Might help with house keeping, instead of EndDialog
					PostMessage(hwndDlg, WLX_WM_SAS, WLX_SAS_TYPE_USER_LOGOFF, 0);
					bResult = TRUE;
					break;
				case eUnlock:
					EndDialog(hwndDlg, IDOK);
					bResult = TRUE;
					break;
				case eLetMSGINAHandleIt: 
					//Most of the time, we end up here with nothing to do
					break;
				}

				SecureZeroMemory(password, sizeof password);
			}
		}
	}

	if (!bResult)
		bResult = pfWlxWkstaLockedSASDlgProc(hwndDlg, uMsg, wParam, lParam);

	--level;

	return bResult;
}

//Stupid helper function to get the first window sent to us
BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
	//Any window will do.
	*((HWND *)lParam) = hwnd;

	//We fail simply because we don't need to iterate over every single window. 
	SetLastError(0); 

	return FALSE;
}


//
// Redirected WlxDialogBoxParam() function.
//
int WINAPI MyWlxDialogBoxParam(HANDLE hWlx, HANDLE hInst, LPWSTR lpszTemplate, HWND hwndOwner, DLGPROC dlgprc, LPARAM dwInitParam)
{
	DLGPROC proc2use = dlgprc;

	//
	// We only know MSGINA dialogs by identifiers.
	//
	if (!HIWORD(lpszTemplate))
	{
		// Hook appropriate dialog boxes as necessary.
		DWORD dlgid = LOWORD(lpszTemplate);	   //Cast to remove warning C4311
		int i;

		//Try to find the dialog
		for(i=0; i<nbDialogsAndControlsID; ++i)
		{
			//Is it one of the ID we know ?
			if(gDialogsAndControls[i].dlgid == dlgid)
			{
				//Yes, found it ! But is the user blacklisted ?
				HWND hWnd = 0;
				PWLX_DESKTOP wlxdesktop = 0;

				((PWLX_DISPATCH_VERSION_1_3) g_pWinlogon)->WlxGetSourceDesktop(hWlx, &wlxdesktop);

				EnumDesktopWindows(wlxdesktop->hDesktop, EnumWindowsProc, (LPARAM)&hWnd);

				LocalFree(wlxdesktop);

				if(!GetLastError())
				{
					DWORD pid, tid;
					HANDLE process;

					tid = GetWindowThreadProcessId(hWnd, &pid);
					process = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pid);

					if(process)
					{
						HANDLE token;
						if(OpenProcessToken(process, TOKEN_QUERY, &token))
						{
							wchar_t excluded[MAX_GROUPNAME] = L"";

							LUID luid = {0};
							GetLUIDFromToken(token, &luid);
							OutputGetSessionUserName(&luid);

							GetGroupName(gExcludedGroupName, excluded, sizeof *excluded);
							if(UsagerEstDansGroupe(token, excluded) != S_OK)
							{
								//User is not blacklisted, let's hook this
								gCurrentDlgIndex = i;
								pfWlxWkstaLockedSASDlgProc = dlgprc;
								proc2use = MyWlxWkstaLockedSASDlgProc; //Use our proc instead
								OutputDebugString(L"Hooked!\n");
								break;
							}

							CloseHandle(token);
						}
						CloseHandle(process);
					}
				}
			}
		}
	}

	return pfWlxDialogBoxParam(hWlx, hInst, lpszTemplate, hwndOwner, proc2use, dwInitParam);
}

int WINAPI MyWlxMessageBox(HANDLE hWlx, HWND hwndOwner, LPWSTR lpszText, LPWSTR lpszTitle, UINT fuStyle)
{
	OutputDebugString(L"MyWlxMessageBox: ");
	OutputDebugString(lpszText);
	OutputDebugString(L"\n");

	{
		wchar_t buf[512];
		DWORD nbuf = sizeof buf / sizeof *buf;

		GetUserName(buf, &nbuf);
		OutputDebugString(L"Thread user name is ");
		if(nbuf > 0)
			OutputDebugString(buf);
		else
			OutputDebugString(L"unknown !!!");

		OutputDebugString(L"\n");
	}

	return pfWlxMessageBox(hWlx, hwndOwner, lpszText, lpszTitle, fuStyle);
}
