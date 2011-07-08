// test.cpp : Defines the entry point for the console application.
//

#include <windows.h>
#include <wincred.h>
#include <tchar.h>
#include <stdio.h>
#include <security.h>
#include <lm.h>
#include <sddl.h>
#include "settings.h"
#include "unlockpolicy.h"
#include "trace.h"
#include "debug.h"
#include "SecurityHelper.h"
#include "global.h"

#include "randpasswd.h"

#include "dlgdefs.h"
#include "loggedout_dlg.h"

INT_PTR CALLBACK DefDialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

DialogAndProcs gDialogsProc[] =
{
    { IDD_LOGGED_OUT_SAS,  MyWlxWkstaLoggedOutSASDlgProc, DefDialogProc},
};

const int nbDialogsAndProcs = sizeof gDialogsProc / sizeof * gDialogsProc;

static MyGinaContext gAucunContext = {0};
MyGinaContext* pgAucunContext = &gAucunContext;


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


// Message handler for about box.
INT_PTR CALLBACK DefDialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);

    switch (message)
    {
        case WM_COMMAND:

            if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
            {
                EndDialog(hDlg, LOWORD(wParam));
                return (INT_PTR)TRUE;
            }

            break;
    }

    return (INT_PTR)FALSE;
}


int _tmain(int argc, _TCHAR* argv[])
{
    int result = -1;
    HANDLE lsa = 0;

    //EnablePrivilege(L"SeTcbPrivilege");
    if (!RegisterLogonProcess(LOGON_PROCESS_NAME, &lsa))
    {
        TRACE(eERROR, L"Process not registered 0x%0X", GetLastError());
    }

    if (argc > 1) for (int i = 1; i < argc; ++i)
        {
            //  wchar_t user[MAX_USERNAME];
            //  wchar_t domain[MAX_DOMAIN];
            wchar_t passwd[MAX_PASSWORD];
            wchar_t username[512];
            wchar_t domain[512];
            HANDLE current_user = 0;
            OpenProcessToken(GetCurrentProcess(), TOKEN_READ, &current_user);
            GetUsernameAndDomainFromToken(current_user, domain, sizeof domain / sizeof * domain, username, sizeof username / sizeof * username);
            TOKEN_USER *tu = 0;
            PSID sid1 = 0;
            PSID sid2 = 0;
            GetSIDFromToken(current_user, &tu);
            sid1 = tu->User.Sid;
            GetSIDFromUsername(argv[i], &sid2);

            if(sid1 && sid2 && EqualSid(sid1, sid2))
            {
                TRACE(eINFO, L"Same SID\n");
            }

            LPTSTR tsid = 0;
            ConvertSidToStringSid(sid1, &tsid);
            wprintf(L"LogonSID : %s (%s)\n", tsid, username);
            LocalFree(tsid);
            ConvertSidToStringSid(sid2, &tsid);
            wprintf(L"User SID : %s (%s)\n", tsid, argv[i]);
            LocalFree(tsid);
            HeapFree(GetProcessHeap(), 0, tu);
            HeapFree(GetProcessHeap(), 0, sid2);

            if (ShouldHookUnlockPasswordDialog(current_user))
            {
                TRACE(eERROR, L"Should hook.\n");
            }

            if (_getws_s(passwd, MAX_PASSWORD) == passwd)
            {
                result = ShouldUnlockForUser(lsa, current_user, domain, username, passwd);

                switch (result)
                {
                    case eLetMSGINAHandleIt:
                        TRACE(eERROR, L"TEST result is eLetMSGINAHandleIt\n");
                        wprintf(L"Actual result   : eLetMSGINAHandleIt\n");
                        break;
                    case eUnlock:
                        TRACE(eERROR, L"TEST result is eUnlock\n");
                        wprintf(L"Actual result   : eUnlock\n");
                        break;
                    case eForceLogoff:
                        TRACE(eERROR, L"TEST result is eForceLogoff\n");
                        wprintf(L"Actual result   : eForceLogoff\n");
                        break;
                }
            }
            else
            {
                TRACE(eERROR, L"Unable to read password\n");
                break;
            }

            CloseHandle(current_user);
        }
    else
    {
        /*
        HMODULE msginadll = LoadLibraryEx(_T("msgina.dll"), 0, LOAD_LIBRARY_AS_DATAFILE);

        if (msginadll)
        {
          DialogBox(msginadll, MAKEINTRESOURCE(1500), 0, MyWlxWkstaLoggedOutSASDlgProc);

          FreeLibrary(msginadll);
          msginadll = 0;
        }
        /*/
        for(int i = 0; i < 125; ++i)
        {
            wchar_t buf[62];
            GenerateRandomUnicodePassword(buf, sizeof buf / sizeof * buf);
            wprintf(L"%s\n", buf);
        }

        //*/
    }

    if (lsa)
    {
        LsaDeregisterLogonProcess(lsa);
    }

    return result;
}
