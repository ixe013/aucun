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

int DumpThisToken();
int DumpThisToken(HANDLE tok);



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


int _tmain(int argc, _TCHAR* argv[])
{
   int result = -1;

   HANDLE lsa = 0;
   //wchar_t unlock[MAX_GROUPNAME] = L"";

   TRACE(L"-------------------------\n");

   //EnablePrivilege(L"SeTcbPrivilege");
   if (!RegisterLogonProcess(LOGON_PROCESS_NAME, &lsa))
      TRACEMSG(GetLastError());

   if (IsWindowsServer())
   {
      TRACE(L"Windows Server\n");
   }
   else
   {
      TRACE(L"Windows pas Server\n");
   }

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
            result = ShouldUnlockForUser(lsa, current_user, L".", argv[i], passwd);

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
	   HMODULE msginadll = LoadLibraryEx(_T("msgina.dll"), 0, LOAD_LIBRARY_AS_DATAFILE);

	   if(msginadll)
	   {
			DialogBox(msginadll, MAKEINTRESOURCE(1500), 0, 0);

		   FreeLibrary(msginadll);
		   msginadll = 0;
	   }
   }

   if(lsa)
     LsaDeregisterLogonProcess(lsa);

   return result;
}
