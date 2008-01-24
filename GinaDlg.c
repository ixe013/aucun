#include <windows.h>
#include <winwlx.h>
#include <assert.h>
#include <lm.h>
#include <string.h>
#include <security.h>
#include <windowsx.h>

#include "Ginahook.h"
#include "Settings.h"
#include "UnlockPolicy.h"

#include "global.h"
#include "debug.h"


typedef struct
{
	int IDD_SAS;
	int IDC_LOCKWKSTA;
	int IDC_LOGOFF;
	int IDD_UNLOCKPASSWORD;
	int IDC_USERNAME;
	int IDC_PASSWORD;
	int IDC_DOMAIN;
	int IDS_CAPTION;
	int IDS_DOMAIN_USERNAME;
	int IDS_USERNAME;
	int IDS_GENERIC_UNLOCK;
}
DialogAndControlsID;

static const DialogAndControlsID gDialogsAndControls[] =
{
	//Windows 2000 (never tested, taken from MS HookGina Sample) 
	//0 values means I don't know the number
	{ 0, 0, 0, 1850, 1453, 1454, 1455, 0, 0, 0, 0 },
	//Windows Server 2003 (and probably XP SP2, never tested)
	{ 1800, 1800, 1801, 1950, 1953, 1954, 1956, 1501, 1528, 1561, 1620 },
};

static const int nbDialogsAndControlsID = sizeof gDialogsAndControls / sizeof *gDialogsAndControls;
static int gCurrentDlgIndex = -1;

//
// Pointers to redirected functions.
//

static PWLX_DIALOG_BOX_PARAM pfWlxDialogBoxParam = NULL;
typedef struct
{
	HANDLE CurrentUser;
	HANDLE Winlogon;
	LPARAM HookedLPARAM;
} DialogLParamHook;

const wchar_t gAucunWinlogonContext[] = L"Paralint.com_Aucun_WinlogonContext";


//
// Pointers to redirected dialog box.
//

static DLGPROC pfWlxWkstaLockedSASDlgProc = NULL;

//
// Local functions.
//

int WINAPI MyWlxDialogBoxParam(HANDLE, HANDLE, LPWSTR, HWND, DLGPROC, LPARAM);

BOOLEAN ShouldHookUnlockPasswordDialog();


//
// Hook WlxDialogBoxParam() dispatch function.
//
void HookWlxDialogBoxParam(PVOID pWinlogonFunctions, DWORD dwWlxVersion)
{
	//WlxDialogBoxParam
	pfWlxDialogBoxParam = ((PWLX_DISPATCH_VERSION_1_0) pWinlogonFunctions)->WlxDialogBoxParam;
	((PWLX_DISPATCH_VERSION_1_0) pWinlogonFunctions)->WlxDialogBoxParam = MyWlxDialogBoxParam;
}

BOOLEAN GetDomainUsernamePassword(HWND hwndDlg, wchar_t *domain, int nbdomain, wchar_t *username, int nbusername, wchar_t *password, int nbpassword)
{
	BOOLEAN result = FALSE;

	if((gCurrentDlgIndex >= 0) && (gCurrentDlgIndex < nbDialogsAndControlsID)) //sanity
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

DWORD DisplayUnlockNotice(HWND hDlg, HANDLE hWlx)
{
	DWORD result = IDNO; //proceed with lock

	wchar_t unlock[MAX_GROUPNAME] = L"";

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
			result = ((PWLX_DISPATCH_VERSION_1_0) g_pWinlogon)->WlxMessageBox(hWlx, hDlg, message, caption, MB_YESNOCANCEL|MB_ICONEXCLAMATION);
		}
	}

	return result;

}

DWORD DisplayForceLogoffNotice(HWND hDlg, HANDLE hWlx)
{
	DWORD result = IDCANCEL;

	if((gCurrentDlgIndex >= 0) && (gCurrentDlgIndex < nbDialogsAndControlsID)) //sanity
	{
		TCHAR szFormat[1024];
		TCHAR buf[2048];
		wchar_t caption[512];
		wchar_t *a, *b;
		WKSTA_USER_INFO_1 *userinfo = 0;

		UINT ids = gDialogsAndControls[gCurrentDlgIndex].IDS_GENERIC_UNLOCK;

		HANDLE token = GetCurrentLoggedOnUserToken();

		a = b = 0;

		if(ImpersonateLoggedOnUser(token))
		{
			NetWkstaUserGetInfo(0, 1, (LPBYTE*)&userinfo);		

			RevertToSelf(); //We are done with this

			//What information do we have ?
			if(*userinfo->wkui1_logon_domain && *userinfo->wkui1_username)
			{
				ids = gDialogsAndControls[gCurrentDlgIndex].IDS_DOMAIN_USERNAME;										
				a = userinfo->wkui1_logon_domain;
				b = userinfo->wkui1_username;
			}
			else if(*userinfo->wkui1_username)
			{
				ids = gDialogsAndControls[gCurrentDlgIndex].IDC_USERNAME;
				a = userinfo->wkui1_username;
				b = 0;
			}
		}

		CloseHandle(token);

		LoadString(hDll, gDialogsAndControls[gCurrentDlgIndex].IDS_CAPTION, caption, sizeof caption / sizeof *caption);
		LoadString(hDll, ids, szFormat, sizeof szFormat / sizeof *szFormat);

		wsprintf(buf, szFormat, a, b, L"some time");

		if(userinfo)
			NetApiBufferFree(userinfo);
	
		result = ((PWLX_DISPATCH_VERSION_1_0) g_pWinlogon)->WlxMessageBox(hWlx, hDlg, buf, caption, MB_OKCANCEL|MB_ICONEXCLAMATION);
	}

	return result;
}

// DelPropProc is an application-defined callback function 
// that deletes a window property. 
BOOL CALLBACK DelPropProc(HWND hwndSubclass, LPTSTR lpszString, HANDLE hData, ULONG_PTR x)       // data handle 
{ 
    RemoveProp(hwndSubclass, lpszString); 
    return TRUE; 
} 

INT_PTR CALLBACK MyWlxWkstaLoggedOnSASDlgProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	INT_PTR bResult = FALSE;

	// We hook a click on OK
	if(uMsg == WM_INITDIALOG)
	{
		DialogLParamHook *myinitparam = (DialogLParamHook*)lParam;

		lParam = myinitparam->HookedLPARAM;

		SetProp(hwndDlg, gAucunWinlogonContext, myinitparam->Winlogon);
	}
	else if(uMsg == WM_DESTROY)
	{
	    EnumPropsEx(hwndDlg, DelPropProc, 0); 
	}
	else if ((uMsg == WM_COMMAND) && (wParam == gDialogsAndControls[gCurrentDlgIndex].IDC_LOCKWKSTA))
	{
		/*
		There is a race condition here (time of check, time of use).
		We check for a certain condition and display a warning. Then we let go 
		and make the same test again to hook the dialog or not. An administrator
		with a good sense of timing could manage set the registry just after the 
		test of ShouldHookUnlockPasswordDialog is made but before the actual 
		dialog would be hooked. 

		In other words: with good timing, an administrator with access to the 
		registry can prevent the unlock notice from showing.

		Spotting the flaw is 80% of the fun... I will probably never fix it.
		*/
		if(ShouldHookUnlockPasswordDialog(pgAucunContext->mCurrentUser))
		{
			switch(DisplayUnlockNotice(hwndDlg, GetProp(hwndDlg, gAucunWinlogonContext)))
			{
				case IDYES: 
					//wParam = gDialogsAndControls[gCurrentDlgIndex].IDC_LOGOFF; 
					//PostMessage(hwndDlg, WLX_WM_SAS, WLX_SAS_TYPE_USER_LOGOFF, 0);
					EndDialog(hwndDlg, WLX_DLG_USER_LOGOFF);
					bResult = TRUE; 

					break;
				case IDCANCEL: 
					bResult = TRUE; 
					break;
				case IDNO: 
					break;
			}
		}
	}

	if (!bResult)
		bResult = pfWlxWkstaLockedSASDlgProc(hwndDlg, uMsg, wParam, lParam);

	return bResult;
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
	if(uMsg == WM_INITDIALOG)
	{
		DialogLParamHook *myinitparam = (DialogLParamHook*)lParam;

		lParam = myinitparam->HookedLPARAM;

		SetProp(hwndDlg, gAucunWinlogonContext, myinitparam->Winlogon);
	}
	else if(uMsg == WM_DESTROY)
	{
	    EnumPropsEx(hwndDlg, DelPropProc, 0); 
	}
	else if ((uMsg == WM_COMMAND) && (wParam == IDOK))
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
					//Might help with house keeping, instead of directly calling EndDialog
					if(DisplayForceLogoffNotice(hwndDlg, GetProp(hwndDlg, gAucunWinlogonContext)) == IDOK)
					{
						PostMessage(hwndDlg, WLX_WM_SAS, WLX_SAS_TYPE_USER_LOGOFF, 0);
					}
					else
					{
						//mimic MSGINA behavior
						SetDlgItemText(hwndDlg, gDialogsAndControls[gCurrentDlgIndex].IDC_PASSWORD, L"");
					}
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

//
// Redirected WlxDialogBoxParam() function.
//
int WINAPI MyWlxDialogBoxParam(HANDLE hWlx, HANDLE hInst, LPWSTR lpszTemplate, HWND hwndOwner, DLGPROC dlgprc, LPARAM dwInitParam)
{
	DLGPROC proc2use = dlgprc;
	LPARAM lparam2use = dwInitParam;
	DialogLParamHook myInitParam = {0};

	//We might doint this for nothing (if dialog is not hooked)
	myInitParam.HookedLPARAM = dwInitParam;
	myInitParam.Winlogon = hWlx;

	pfWlxWkstaLockedSASDlgProc = dlgprc;

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
			if(gDialogsAndControls[i].IDD_SAS == dlgid)
			{
				gCurrentDlgIndex = i;

				proc2use = MyWlxWkstaLoggedOnSASDlgProc;
				lparam2use = (LPARAM)&myInitParam;
				OutputDebugString(L"Hooked SAS!\n");

				break;
			}
			else if(gDialogsAndControls[i].IDD_UNLOCKPASSWORD == dlgid)
			{
				gCurrentDlgIndex = i;

				if(ShouldHookUnlockPasswordDialog(pgAucunContext->mCurrentUser))
				{
					proc2use = MyWlxWkstaLockedSASDlgProc; //Use our proc instead
					lparam2use = (LPARAM)&myInitParam;
					OutputDebugString(L"Hooked unlock\n");
				}

				//No need to go on, even if nothing was hooked
				break;
			}
		}
	}

	return pfWlxDialogBoxParam(hWlx, hInst, lpszTemplate, hwndOwner, proc2use, lparam2use);
}

