/*
 Copyright (c) 2008, Guillaume Seguin (guillaume@paralint.com)
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions
 are met:

 1. Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.

 2. Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.

 THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 SUCH DAMAGE.
*/

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
#include "trace.h"
#include "SecurityHelper.h"


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
	//Windows Server 2003
	//XP SP3 (and probably previeus versions also, never tested)
	{
		1800,    // IDD_SAS
			1800,    // IDC_LOCKWKSTA
			1801,    // IDC_LOGOFF
			1950,    // IDD_UNLOCKPASSWORD
			1953,    // IDC_USERNAME
			1954,    // IDC_PASSWORD
			1956,    // IDC_DOMAIN
			1501,    // IDS_CAPTION
			1528,    // IDS_DOMAIN_USERNAME
			1561,    // IDS_USERNAME
			1528     // IDS_GENERIC_UNLOCK //1607
	},
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

	if ((gCurrentDlgIndex >= 0) && (gCurrentDlgIndex < nbDialogsAndControlsID)) //sanity
	{
		if ((GetDlgItemText(hwndDlg, gDialogsAndControls[gCurrentDlgIndex].IDC_PASSWORD, password, nbpassword) > 0)
			&& (GetDlgItemText(hwndDlg, gDialogsAndControls[gCurrentDlgIndex].IDC_USERNAME, username, nbusername) > 0))
		{
            HWND domainCombo;
            int cursel;

			result = TRUE; //That's enough to keep going. Let's try the domain nonetheless

            domainCombo = GetDlgItem(hwndDlg, gDialogsAndControls[gCurrentDlgIndex].IDC_DOMAIN);

			GetDlgItemText(hwndDlg, gDialogsAndControls[gCurrentDlgIndex].IDC_DOMAIN, domain, nbdomain);

			cursel = ComboBox_GetCurSel(domainCombo);

            if(cursel >= 0) 
            {
                if(ComboBox_GetLBTextLen(domainCombo, cursel) < nbdomain)
                {
                    ComboBox_GetLBText(domainCombo, cursel, domain);
                }
            }
            else
            {
                *domain = 0;
            }
		}
	}

	return result;
}

BOOL IsWindowsServer()
{
	OSVERSIONINFOEX osvi;
	DWORDLONG dwlConditionMask = 0;

	// Initialize the OSVERSIONINFOEX structure.
	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	osvi.dwMajorVersion = 5;
	osvi.wProductType = VER_NT_SERVER;

	// Initialize the condition mask.
	VER_SET_CONDITION(dwlConditionMask, VER_MAJORVERSION, VER_GREATER_EQUAL);
	VER_SET_CONDITION(dwlConditionMask, VER_PRODUCT_TYPE, VER_EQUAL);

	// Perform the test.
	return VerifyVersionInfo(&osvi, VER_MAJORVERSION | VER_PRODUCT_TYPE, dwlConditionMask);
}


DWORD DisplayUnlockNotice(HWND hDlg, HANDLE hWlx)
{
	DWORD result = IDNO; //proceed with lock

	wchar_t unlock[MAX_GROUPNAME] = L"";

	if (GetGroupName(gUnlockGroupName, unlock, sizeof unlock / sizeof *unlock) == S_OK)
	{
		wchar_t caption[512];
		wchar_t text[2048];

		if ((GetNoticeText(L"Caption", caption, sizeof caption / sizeof *caption) == S_OK)
			&& (GetNoticeText(L"Text", text, sizeof text / sizeof *text) == S_OK))
		{
			wchar_t message[MAX_USERNAME + sizeof text / sizeof *text];
			wchar_t *read = text;
			wchar_t *write = text;

			TRACE(L"Unlock notice will be displayed.\n");

			//Insert real \n caracters from the \n found in the string.
			while (*read)
			{
				if ((*read == '\\') && (*(read+1) == 'n'))
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

DWORD DisplayForceLogoffNotice(HWND hDlg, HANDLE hWlx, HANDLE current_user)
{
	DWORD result = IDCANCEL;

	TRACE(L"About to display a notice for dialog index %d\n", gCurrentDlgIndex);

	if ((gCurrentDlgIndex >= 0) && (gCurrentDlgIndex < nbDialogsAndControlsID)) //sanity
	{
		wchar_t buf[2048];
		wchar_t caption[512];

		//Start with the caption
		LoadString(hResourceDll, gDialogsAndControls[gCurrentDlgIndex].IDS_CAPTION, caption, sizeof caption / sizeof *caption);

		//Windows XP has a plain vanilla message, no insert. Let's start with that
		LoadString(hResourceDll, gDialogsAndControls[gCurrentDlgIndex].IDS_GENERIC_UNLOCK, buf, sizeof buf / sizeof *buf);

		//The format of the message is different on Windows Server. This test is somewhat short sighted,
		//but we know that in the future versions there is no Gina at all ! That's why we shortcut
		//the test to either Windows XP or Windows Server.
		if (IsWindowsServer())
		{
			wchar_t format[1024];
			wchar_t username[1024];
			wchar_t domain[1024];
			int howmany;

			howmany = GetUsernameAndDomainFromToken(current_user, domain, sizeof domain / sizeof *domain, username, sizeof username /  sizeof *username);

			switch(howmany)
			{
			case 2:
				{
					LoadString(hResourceDll, gDialogsAndControls[gCurrentDlgIndex].IDS_DOMAIN_USERNAME, format, sizeof format / sizeof *format);
					wsprintf(buf, format, domain, username, L"some time");
				}
				break;
			case 1:
				{
					LoadString(hResourceDll, gDialogsAndControls[gCurrentDlgIndex].IDS_USERNAME, format, sizeof format / sizeof *format);
					wsprintf(buf, format, username, L"some time");
				}
				break;
			}
		}

		TRACE(buf);
		TRACEMORE(L"\n");

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
	if (uMsg == WM_INITDIALOG)
	{
		DialogLParamHook *myinitparam = (DialogLParamHook*)lParam;

		lParam = myinitparam->HookedLPARAM;

		SetProp(hwndDlg, gAucunWinlogonContext, myinitparam->Winlogon);

		TRACE(L"Hooked dialog shown.\n");
	}
	else if (uMsg == WM_DESTROY)
	{
		EnumPropsEx(hwndDlg, DelPropProc, 0);
	}
	else if ((uMsg == WM_COMMAND) && (wParam == gDialogsAndControls[gCurrentDlgIndex].IDC_LOCKWKSTA))
	{
		TRACE(L"User locking workstation.\n");
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
		if (ShouldHookUnlockPasswordDialog(pgAucunContext->mCurrentUser))
		{
			TRACE(L"Will hook dialog if allowed to.\n");
			switch (DisplayUnlockNotice(hwndDlg, GetProp(hwndDlg, gAucunWinlogonContext)))
			{
				//We said that a custom Gina was installed, and asked "do you want
				//to lof off instead" ?
			case IDYES:
				//Why 113 ? I didn't find this value anywhere in the header files,
				//but it is the value returned by the original MSGINA DialogProc
				//When you click YES on the "Are you sure you want to log off" dialog box.
				TRACE(L"User wants to logoff instead.\n");
				EndDialog(hwndDlg, 113);
				bResult = TRUE;
				break;

				//Forget about it, I am not locking at all
			case IDCANCEL:
				TRACE(L"Lock request cancelled.\n");
				bResult = TRUE;
				break;

				//I don't care. Lock my workstation
			case IDNO:
			default:
				break;
			}
		}
	}
	else if (uMsg == WM_COMMAND)
    {
        TRACE(L"User clicked on %d\n", wParam);
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

	// We hook a click on OK
	if (uMsg == WM_INITDIALOG)
	{
		DialogLParamHook *myinitparam = (DialogLParamHook*)lParam;

		lParam = myinitparam->HookedLPARAM;

		SetProp(hwndDlg, gAucunWinlogonContext, myinitparam->Winlogon);
		TRACE(L"Hooked dialog shown.\n");
	}
	else if (uMsg == WM_DESTROY)
	{
		EnumPropsEx(hwndDlg, DelPropProc, 0);
	}
	else if ((uMsg == WM_COMMAND) && (wParam == IDOK))
	{
		wchar_t rawdomain[MAX_DOMAIN];
		wchar_t rawusername[MAX_USERNAME];
		wchar_t password[MAX_PASSWORD];

		TRACE(L"Unlock or logoff attemp\n");

		//Get the username and password for this particular Dialog template
		if (GetDomainUsernamePassword(hwndDlg, rawdomain, sizeof rawdomain / sizeof *rawdomain,
			rawusername, sizeof rawusername / sizeof *rawusername,
			password, sizeof password / sizeof *password))
		{
			wchar_t *username = 0;
			wchar_t *domain = 0;

			//Replace this hack with CredUIParseUserName
			username = wcsstr(rawusername, L"\\");

            //If there is a backslash, parse the domain from the username
			if (username)
			{
				domain = rawusername;
				*username++ = 0; //Null terminate the domain name and skip the separator
			}
			else
			{
				username = rawusername; //No domain entered, so point directly to the supplied buffer
				if (*rawdomain)
                {
                    //So let's get the local computer name
                    wchar_t chrComputerName[MAX_COMPUTERNAME_LENGTH + 1];  
                    DWORD dwBufferSize = MAX_COMPUTERNAME_LENGTH + 1;  
                     
                    //This will work unless the local computer was chosen
                    domain = rawdomain;
                    
                    if(GetComputerName(chrComputerName,&dwBufferSize)) 
                    {  
                        //see if the computer name is in the domain string
                        wchar_t *pComputerName = wcsstr(rawdomain, chrComputerName);
    
                        if(pComputerName)
                        {
                            //There is a chance that the user selected the current computer
                            //But it could be just that the computer name is a subset of a
                            //longer domain name
                            //But we know that the name will be qualified with the string
                            //"(this computer)" localized. It could be in front or at the end.
                            //Example : domain ABC123
                            //        computer  BC1 (this computer)       
                            //       (cet ordi) BC1
                            
                            //This will handle cases when the computer name is
                            //after the localized text. It doesn't do anything on
                            //an English or French local.
                            domain = pComputerName;
                            //If it ends with a space, trim it
                            if(domain[dwBufferSize] == ' ')
                            {
                                domain[dwBufferSize] = 0;
                            }
                        }
                    }  
                }
			}

			if (*username && *password)
			{
				// Can you spot the buffer overflow vulnerability in this next line ?
				TRACE(L"User %s\\%s has entered his password.\n", *domain?domain:L"", username);
				// Don't worry, GetDomainUsernamePassword validated input length. We are safe.

				switch (ShouldUnlockForUser(pgAucunContext->mLSA, pgAucunContext->mCurrentUser, domain, username, password))
				{
				case eForceLogoff:
					//Might help with house keeping, instead of directly calling EndDialog
					if (DisplayForceLogoffNotice(hwndDlg, GetProp(hwndDlg, gAucunWinlogonContext), pgAucunContext->mCurrentUser) == IDOK)
					{
						TRACE(L"User was allowed (and agreed) to forcing a logoff.\n");
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
					TRACE(L"User was allowed to unlock.\n");
					EndDialog(hwndDlg, IDOK);
					bResult = TRUE;
					break;

				case eLetMSGINAHandleIt:
				default:
					TRACE(L"Will be handled by MSGINA.\n");
					//Most of the time, we end up here with nothing to do
					break;
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
	LPARAM lparam2use = dwInitParam;
	DialogLParamHook myInitParam = {0};
	DWORD dlgid = 0;
	int result = 0;
   DLGPROC saved_pfWlxWkstaLockedSASDlgProc;

	//We might doint this for nothing (if dialog is not hooked)
	myInitParam.HookedLPARAM = dwInitParam;
	myInitParam.Winlogon = hWlx;

	//First user-reported bug !!!
	//This function was not re-entrant with regards to pfWlxWkstaLockedSASDlgProc
   saved_pfWlxWkstaLockedSASDlgProc = pfWlxWkstaLockedSASDlgProc;

	pfWlxWkstaLockedSASDlgProc = dlgprc;

	TRACE(L"About to create the dialog");
	//
	// We only know MSGINA dialogs by identifiers.
	//
	if (!HIWORD(lpszTemplate))
	{
		// Hook appropriate dialog boxes as necessary.
		int i;
		dlgid = LOWORD(lpszTemplate);    //Cast to remove warning C4311

		//Try to find the dialog
		for (i=0; i<nbDialogsAndControlsID; ++i)
		{
			//Is it one of the ID we know ?
			if (gDialogsAndControls[i].IDD_SAS == dlgid)
			{
				//The dialog that asks if you would like to change password, lock, taskmgr, etc.
				TRACEMORE(L" to change password, lock wkst, taskmgr, etc.\n");
				gCurrentDlgIndex = i;

				proc2use = MyWlxWkstaLoggedOnSASDlgProc;
				lparam2use = (LPARAM)&myInitParam;

				break;
			}
			else if (gDialogsAndControls[i].IDD_UNLOCKPASSWORD == dlgid)
			{
				//The dialog where you enter your password
				TRACEMORE(L" where you try to unlock a locked session\n");

				gCurrentDlgIndex = i;

				if (ShouldHookUnlockPasswordDialog(pgAucunContext->mCurrentUser))
				{
					TRACE(L"Hooking the unlock dialog\n");
					proc2use = MyWlxWkstaLockedSASDlgProc; //Use our proc instead
					lparam2use = (LPARAM)&myInitParam;
				}

				//No need to go on, even if nothing was hooked
				break;
			}
		}
	}

	if (proc2use == dlgprc)
	{
		TRACE(L"(%d). it was not hooked.\n", dlgid);
	}

	result = pfWlxDialogBoxParam(hWlx, hInst, lpszTemplate, hwndOwner, proc2use, lparam2use);
    pfWlxWkstaLockedSASDlgProc = saved_pfWlxWkstaLockedSASDlgProc;
    
	if (proc2use != dlgprc)
    {
        TRACE(L"Hooked dialog returned %d\n", result);
    }
    else
    {
        TRACE(L"Regular dialog returned %d\n", result);
    }

	return result;
}
