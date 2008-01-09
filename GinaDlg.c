
#define _WIN32_WINNT 0x0501

#include <windows.h>
#include <winwlx.h>
#include <assert.h>
#include <lm.h>

#include "Ginahook.h"
#include "Settings.h"
#include "UnlockPolicy.h"


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

static PWLX_DIALOG_BOX_PARAM pfWlxDialogBoxParam = NULL;
static int gCurrentDlgIndex = -1;

//
// Pointers to redirected dialog box.
//

static DLGPROC pfWlxWkstaLockedSASDlgProc = NULL;

//
// Local functions.
//

int WINAPI MyWlxDialogBoxParam(HANDLE, HANDLE, LPWSTR, HWND, DLGPROC, LPARAM);


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

	assert(pfWlxWkstaLockedSASDlgProc != NULL); //sanity

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

			username = wcsstr(rawusername, L"\\");

			//C'est peut-etre une notation avec une arobas
			//if(!username)
			//	username = wcsstr(rawusername, L"@");

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
				if(ShouldUnlockForUser(domain, username, password))
				{
					//EndDialog(hwndDlg, IDOK);
					EndDialog(hwndDlg, WLX_DLG_USER_LOGOFF);
					bResult = TRUE;
				}

				SecureZeroMemory(password, sizeof password);
			}
		}
	}

	if (!bResult)
		bResult = pfWlxWkstaLockedSASDlgProc(hwndDlg, uMsg, wParam, lParam);

	return bResult;
}



//
// Redirected WlxDialogBoxParam() function.
//
int WINAPI MyWlxDialogBoxParam(HANDLE hWlx, HANDLE hInst, LPWSTR lpszTemplate, HWND hwndOwner, DLGPROC dlgprc, LPARAM dwInitParam)
{
	DLGPROC proc2use = dlgprc;

	//
	// Sanity check.
	//
	assert(pfWlxDialogBoxParam != NULL);

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
				gCurrentDlgIndex = i;
				//Yes, found it !
				pfWlxWkstaLockedSASDlgProc = dlgprc;
				proc2use = MyWlxWkstaLockedSASDlgProc; //Use our proc instead
				break;
			}
		}
	}

	return pfWlxDialogBoxParam(hWlx, hInst, lpszTemplate, hwndOwner, proc2use, dwInitParam);
}

