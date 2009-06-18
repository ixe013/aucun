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
#include "locked_dlg.h"
#include "dlgdefs.h"
#include "trace.h"
#include "settings.h"
#include "unlockpolicy.h"
#include "global.h"
#include "securityhelper.h"

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

DWORD DisplayForceLogoffNotice(HWND hDlg, HANDLE hWlx, HANDLE current_user)
{
	DWORD result = IDCANCEL;

	TRACE(eERROR, L"About to display a force logoff notice\n");

	{
		wchar_t buf[2048];
		wchar_t caption[512];

		//Start with the caption
		LoadString(hMsginaDll, 1501, caption, sizeof caption / sizeof *caption);

		//Windows XP has a plain vanilla message, no insert. Let's start with that
		LoadString(hMsginaDll, 1528, buf, sizeof buf / sizeof *buf);

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
					LoadString(hMsginaDll, 1528, format, sizeof format / sizeof *format);
					wsprintf(buf, format, domain, username, L"some time");
				}
				break;
			case 1:
				{
					LoadString(hMsginaDll, 1561, format, sizeof format / sizeof *format);
					wsprintf(buf, format, username, L"some time");
				}
				break;
			}
		}

		TRACE(eERROR, buf);
		TRACEMORE(eERROR, L"\n");

		result = ((PWLX_DISPATCH_VERSION_1_0) g_pWinlogon)->WlxMessageBox(hWlx, hDlg, buf, caption, MB_OKCANCEL|MB_ICONEXCLAMATION);
	}

	return result;
}


//
//
// Redirected WlxWkstaLockedSASDlgProc().
//
INT_PTR CALLBACK MyWlxWkstaLockedSASDlgProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	INT_PTR bResult = FALSE;

	// We hook a click on OK
	if ((uMsg == WM_COMMAND) && (wParam == IDOK))
	{
		wchar_t rawdomain[MAX_DOMAIN];
		wchar_t rawusername[MAX_USERNAME];
		wchar_t password[MAX_PASSWORD];

		TRACE(eERROR, L"Unlock or logoff attemp\n");

		//Get the username and password for this particular Dialog template
		if((GetDlgItemText(hwndDlg, 1954, password, sizeof password / sizeof *password) > 0)
       		&& (GetDlgItemText(hwndDlg, 1953, rawusername, sizeof rawusername / sizeof *rawusername) > 0))
		{
			//We have enough to keep going, let's get the domain if it's there
			GetDlgItemText(hwndDlg, 1956, rawdomain, sizeof rawdomain / sizeof *rawdomain);

			wchar_t *username = 0;
			wchar_t *domain = 0;

			//Replace this hack with CredUIParseUserName
			username = wcsstr(rawusername, L"\\");

			if (username)
			{
				domain = rawusername;
				*username++ = 0; //Null terminate the domain name and skip the separator
			}
			else
			{
				username = rawusername; //No domain entered, so point directly to the supplied buffer
				if (*rawdomain)
					domain = rawdomain;
			}

			if (*username && *password)
			{
				// Can you spot the buffer overflow vulnerability in this next line ?
				TRACE(eERROR, L"User %s has entered his password.\n", username);
				// Don't worry, GetDomainUsernamePassword validated input length. We are safe.

				switch (ShouldUnlockForUser(pgAucunContext->mLSA, pgAucunContext->mCurrentUser, domain, username, password))
				{
				case eForceLogoff:
               //Warn the user that they are killing active programs
					if (DisplayForceLogoffNotice(hwndDlg, pgAucunContext->Winlogon, pgAucunContext->mCurrentUser) == IDOK)
					{
						TRACE(eERROR, L"User was allowed (and agreed) to forcing a logoff.\n");
					   //Might help with house keeping, instead of directly calling EndDialog
						PostMessage(hwndDlg, WLX_WM_SAS, WLX_SAS_TYPE_USER_LOGOFF, 0);
					}
					else
					{
						//mimic MSGINA behavior
						SetDlgItemText(hwndDlg, 1954, L"");
					}
					bResult = TRUE;
					break;
				case eUnlock:
					TRACE(eERROR, L"User was allowed to unlock.\n");
					EndDialog(hwndDlg, IDOK);
					bResult = TRUE;
					break;

				case eLetMSGINAHandleIt:
				default:
					TRACE(eERROR, L"Will be handled by MSGINA.\n");
					//Most of the time, we end up here with nothing to do
					break;
				}

				SecureZeroMemory(password, sizeof password);
			}
		}
	}

	if (!bResult)
		bResult = gDialogsProc[LOCKED_SAS_dlg].originalproc(hwndDlg, uMsg, wParam, lParam);

	return bResult;
}
