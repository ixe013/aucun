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
#include "loggedon_dlg.h"
#include "dlgdefs.h"
#include "trace.h"
#include "global.h"
#include "unlockpolicy.h"
#include "settings.h"
#include <winwlx.h>

DWORD DisplayUnlockNotice(HWND hDlg, HANDLE hWlx)
{
    DWORD result = IDNO; //proceed with lock
    wchar_t unlock[MAX_GROUPNAME] = L"";

    if (GetGroupName(gUnlockGroupName, unlock, sizeof unlock / sizeof * unlock) == S_OK)
    {
        wchar_t caption[512];
        wchar_t text[2048];

        if ((GetNoticeText(L"Caption", caption, sizeof caption / sizeof * caption) == S_OK)
                && (GetNoticeText(L"Text", text, sizeof text / sizeof * text) == S_OK))
        {
            wchar_t message[MAX_USERNAME + sizeof text / sizeof * text];
            wchar_t* read = text;
            wchar_t* write = text;
            TRACE(eDEBUG, L"Unlock notice will be displayed.\n");
            //Insert real \n caracters from the \n found in the string.
            wsprintf(message, InterpretCarriageReturn(text), unlock); //Will insert group name if there is a %s in the message
            result = ((PWLX_DISPATCH_VERSION_1_0) g_pWinlogon)->WlxMessageBox(hWlx, hDlg, message, caption, MB_YESNOCANCEL | MB_ICONEXCLAMATION);
        }
    }

    return result;
}

INT_PTR CALLBACK MyWlxWkstaLoggedOnSASDlgProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    INT_PTR bResult = FALSE;

    // We hook a click on OK
    if (uMsg == WM_INITDIALOG)
    {
        DialogLParamHook* myinitparam = (DialogLParamHook*)lParam;
        lParam = myinitparam->HookedLPARAM;
        SetProp(hwndDlg, gAucunWinlogonContext, myinitparam->Winlogon);
        TRACE(eDEBUG, L"Hooked dialog shown.\n");
    }
    else if (uMsg == WM_DESTROY)
    {
        EnumPropsEx(hwndDlg, DelPropProc, 0);
    }
    else if ((uMsg == WM_COMMAND) && (wParam == gDialogsAndControls[gCurrentDlgIndex].IDC_LOCKWKSTA))
    {
        TRACE(eERROR, L"User locking workstation.\n");

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
            TRACE(eERROR, L"Will hook dialog if allowed to.\n");

            switch (DisplayUnlockNotice(hwndDlg, GetProp(hwndDlg, gAucunWinlogonContext)))
            {
                    //We said that a custom Gina was installed, and asked "do you want
                    //to lof off instead" ?
                case IDYES:
                    //Why 113 ? I didn't find this value anywhere in the header files,
                    //but it is the value returned by the original MSGINA DialogProc
                    //When you click YES on the "Are you sure you want to log off" dialog box.
                    TRACE(eERROR, L"User wants to logoff instead.\n");
                    EndDialog(hwndDlg, 113);
                    bResult = TRUE;
                    break;
                    //Forget about it, I am not locking at all
                case IDCANCEL:
                    TRACE(eERROR, L"Lock request cancelled.\n");
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
        TRACE(eDEBUG, L"User clicked on %d\n", wParam);
    }

    if (!bResult)
    {
        bResult = gDialogsProc[LOGGED_ON_SAS_dlg].originalproc(hwndDlg, uMsg, wParam, lParam);
    }

    return bResult;
}


