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

#include "randpasswd.h"

#include "dlgdefs.h"
#include "loggedout_dlg.h"

INT_PTR CALLBACK DefDialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

DialogAndProcs gDialogsProc[] = 
{
	{ IDD_LOGGED_OUT_SAS,  MyWlxWkstaLoggedOutSASDlgProc, DefDialogProc},
};

const int nbDialogsAndProcs = sizeof gDialogsProc / sizeof *gDialogsProc;


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
   //wchar_t unlock[MAX_GROUPNAME] = L"";

   TRACE(L"-------------------------\n");

   //EnablePrivilege(L"SeTcbPrivilege");
   if (!RegisterLogonProcess(LOGON_PROCESS_NAME, &lsa))
      TRACEMSG(GetLastError());

   if (argc > 1) for (int i=1; i<argc; ++i)
   {
         //  wchar_t user[MAX_USERNAME];
         //  wchar_t domain[MAX_DOMAIN];
         wchar_t passwd[MAX_PASSWORD];
         wchar_t username[512];
         wchar_t domain[512];

         HANDLE current_user = 0;

         OpenProcessToken(GetCurrentProcess(), TOKEN_READ, &current_user);

         GetUsernameAndDomainFromToken(current_user, username, sizeof username / sizeof *username, domain, sizeof domain / sizeof *domain);

         if (ShouldHookUnlockPasswordDialog(current_user))
         {
            TRACE(L"Should hook.\n");
         }

         if (_getws_s(passwd, MAX_PASSWORD) == passwd)
         {
            result = ShouldUnlockForUser(lsa, current_user, domain, username, passwd);

            switch (result)
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
   else
   {
	/*
	   HMODULE msginadll = LoadLibraryEx(_T("msgina.dll"), 0, LOAD_LIBRARY_AS_DATAFILE);

	   if(msginadll)
	   {
			DialogBox(msginadll, MAKEINTRESOURCE(1500), 0, MyWlxWkstaLoggedOutSASDlgProc);

		   FreeLibrary(msginadll);
		   msginadll = 0;
	   }
	*/
	wchar_t password[20];

	for(int z=0; z<5; ++z)
	{
		GenerateRandomUnicodePassword(password, 20);

		MessageBox(0, password, L"Crazy password", MB_OK);
	}
   }

   if(lsa)
     LsaDeregisterLogonProcess(lsa);

   return result;
}
