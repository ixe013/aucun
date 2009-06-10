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

#define _WIN32_WINNT 0x0501

#include <windows.h>
#include <winwlx.h>

#include "Ginahook.h"
#include "GinaDlg.h"
#include "global.h"
#include "trace.h"
#include "debug.h"
#include "SecurityHelper.h"
#include "Settings.h"

#include "loggedout_dlg.h"
#include "resource.h"
#include "selfserve_sas_dlg.h"

//
// Location of the real MSGINA.
//

#define REALGINA_PATH      TEXT("MSGINA.DLL")
#define GINASTUB_VERSION   (WLX_VERSION_1_4)

//Hooked instance of MSGINA
HINSTANCE hDll;

//
// Winlogon function dispatch table.
//
PVOID g_pWinlogon = NULL;
static DWORD g_dwVersion = WLX_VERSION_1_4;
static HANDLE WinlogonHandle = 0;

static MyGinaContext gAucunContext = {0};
MyGinaContext *pgAucunContext = &gAucunContext;

//
// Pointers to the real MSGINA functions.
//

static PFWLXNEGOTIATE            pfWlxNegotiate;
static PFWLXINITIALIZE           pfWlxInitialize;
static PFWLXDISPLAYSASNOTICE     pfWlxDisplaySASNotice;
static PFWLXLOGGEDOUTSAS         pfWlxLoggedOutSAS;
static PFWLXACTIVATEUSERSHELL    pfWlxActivateUserShell;
static PFWLXLOGGEDONSAS          pfWlxLoggedOnSAS;
static PFWLXDISPLAYLOCKEDNOTICE  pfWlxDisplayLockedNotice;
static PFWLXWKSTALOCKEDSAS       pfWlxWkstaLockedSAS;
static PFWLXISLOCKOK             pfWlxIsLockOk;
static PFWLXISLOGOFFOK           pfWlxIsLogoffOk;
static PFWLXLOGOFF               pfWlxLogoff;
static PFWLXSHUTDOWN             pfWlxShutdown;

//
// New for version 1.1
//
static PFWLXSTARTAPPLICATION     pfWlxStartApplication  = NULL;
static PFWLXSCREENSAVERNOTIFY    pfWlxScreenSaverNotify = NULL;

//
// New for version 1.2 - No new GINA interface was added, except
//                       a new function in the dispatch table.
//

//
// New for version 1.3
//
static PFWLXNETWORKPROVIDERLOAD   pfWlxNetworkProviderLoad  = NULL;
static PFWLXDISPLAYSTATUSMESSAGE  pfWlxDisplayStatusMessage = NULL;
static PFWLXGETSTATUSMESSAGE      pfWlxGetStatusMessage     = NULL;
static PFWLXREMOVESTATUSMESSAGE   pfWlxRemoveStatusMessage  = NULL;

//
// New for version 1.4
//
static PWLXGETCONSOLESWITCHCREDENTIALS pfWlxGetConsoleSwitchCredentials = NULL;
static PWLXRECONNECTNOTIFY pfWlxReconnectNotify  = NULL;
static PWLXDISCONNECTNOTIFY pfWlxDisconnectNotify = NULL;

PVOID GetHookedContext(PVOID pWlxContext)
{
   return ((MyGinaContext*)pWlxContext)->mHookedContext;
}

//
// Hook into the real MSGINA.
//
BOOL MyInitialize(HINSTANCE hDll, DWORD dwWlxVersion)
{
   //
   // Get pointers to all of the WLX functions in the real MSGINA.
   //
   pfWlxInitialize = (PFWLXINITIALIZE) GetProcAddress(hDll, "WlxInitialize");

   if (!pfWlxInitialize)
   {
      return FALSE;
   }

   pfWlxDisplaySASNotice = (PFWLXDISPLAYSASNOTICE) GetProcAddress(hDll, "WlxDisplaySASNotice");

   if (!pfWlxDisplaySASNotice)
   {
      return FALSE;
   }

   pfWlxLoggedOutSAS = (PFWLXLOGGEDOUTSAS) GetProcAddress(hDll, "WlxLoggedOutSAS");

   if (!pfWlxLoggedOutSAS)
   {
      return FALSE;
   }

   pfWlxActivateUserShell = (PFWLXACTIVATEUSERSHELL) GetProcAddress(hDll, "WlxActivateUserShell");

   if (!pfWlxActivateUserShell)
   {
      return FALSE;
   }

   pfWlxLoggedOnSAS = (PFWLXLOGGEDONSAS) GetProcAddress(hDll, "WlxLoggedOnSAS");

   if (!pfWlxLoggedOnSAS)
   {
      return FALSE;
   }

   pfWlxDisplayLockedNotice = (PFWLXDISPLAYLOCKEDNOTICE) GetProcAddress(hDll, "WlxDisplayLockedNotice");

   if (!pfWlxDisplayLockedNotice)
   {
      return FALSE;
   }

   pfWlxIsLockOk = (PFWLXISLOCKOK) GetProcAddress(hDll, "WlxIsLockOk");

   if (!pfWlxIsLockOk)
   {
      return FALSE;
   }

   pfWlxWkstaLockedSAS = (PFWLXWKSTALOCKEDSAS) GetProcAddress(hDll, "WlxWkstaLockedSAS");

   if (!pfWlxWkstaLockedSAS)
   {
      return FALSE;
   }

   pfWlxIsLogoffOk = (PFWLXISLOGOFFOK) GetProcAddress(hDll, "WlxIsLogoffOk");

   if (!pfWlxIsLogoffOk)
   {
      return FALSE;
   }

   pfWlxLogoff = (PFWLXLOGOFF) GetProcAddress(hDll, "WlxLogoff");

   if (!pfWlxLogoff)
   {
      return FALSE;
   }

   pfWlxShutdown = (PFWLXSHUTDOWN) GetProcAddress(hDll, "WlxShutdown");

   if (!pfWlxShutdown)
   {
      return FALSE;
   }

   //
   // Load functions for version 1.1 as necessary.
   //
   if (dwWlxVersion > WLX_VERSION_1_0)
   {
      pfWlxStartApplication = (PFWLXSTARTAPPLICATION) GetProcAddress(hDll, "WlxStartApplication");

      if (!pfWlxStartApplication)
      {
         return FALSE;
      }

      pfWlxScreenSaverNotify = (PFWLXSCREENSAVERNOTIFY) GetProcAddress(hDll, "WlxScreenSaverNotify");

      if (!pfWlxScreenSaverNotify)
      {
         return FALSE;
      }
   }

   //
   // Load functions for version 1.3 as necessary.
   //
   if (dwWlxVersion > WLX_VERSION_1_2)
   {
      pfWlxNetworkProviderLoad = (PFWLXNETWORKPROVIDERLOAD)GetProcAddress(hDll, "WlxNetworkProviderLoad");

      if (!pfWlxNetworkProviderLoad)
      {
         return FALSE;
      }

      pfWlxDisplayStatusMessage = (PFWLXDISPLAYSTATUSMESSAGE)GetProcAddress(hDll, "WlxDisplayStatusMessage");

      if (!pfWlxDisplayStatusMessage)
      {
         return FALSE;
      }

      pfWlxGetStatusMessage =(PFWLXGETSTATUSMESSAGE)GetProcAddress(hDll, "WlxGetStatusMessage");
      if (!pfWlxGetStatusMessage)
      {
         return FALSE;
      }

      pfWlxRemoveStatusMessage =
         (PFWLXREMOVESTATUSMESSAGE)
         GetProcAddress(hDll, "WlxRemoveStatusMessage");
      if (!pfWlxRemoveStatusMessage)
      {
         return FALSE;
      }
   }

   //
   // Load functions for version 1.3 as necessary.
   //
   if (dwWlxVersion > WLX_VERSION_1_3)
   {
      pfWlxGetConsoleSwitchCredentials = (PWLXGETCONSOLESWITCHCREDENTIALS) GetProcAddress(hDll, "WlxGetConsoleSwitchCredentials");
      if (!pfWlxGetConsoleSwitchCredentials) return FALSE;

      pfWlxReconnectNotify = (PWLXRECONNECTNOTIFY) GetProcAddress(hDll, "WlxReconnectNotify");
      if (!pfWlxReconnectNotify) return FALSE;

      pfWlxDisconnectNotify = (PWLXDISCONNECTNOTIFY) GetProcAddress(hDll, "WlxDisconnectNotify");
      if (!pfWlxDisconnectNotify) return FALSE;

   }

   //
   // Everything loaded OK.
   //
   return TRUE;
}


BOOL WINAPI WlxNegotiate(DWORD dwWinlogonVersion, DWORD *pdwDllVersion)
{
   DWORD dwWlxVersion = GINASTUB_VERSION;

   TRACE(eDEBUG, L"WlxNegotiate\n");
   //
   // Load MSGINA.DLL.
   //
   if (!(hDll = LoadLibrary(REALGINA_PATH)))
   {
      return FALSE;
   }

   //
   // Get pointers to WlxNegotiate function in the real MSGINA.
   //
   pfWlxNegotiate = (PFWLXNEGOTIATE) GetProcAddress(hDll, "WlxNegotiate");
   if (!pfWlxNegotiate)
   {
      return FALSE;
   }

   //
   // Handle older version of Winlogon.
   //
   if (dwWinlogonVersion < dwWlxVersion)
   {
      dwWlxVersion = dwWinlogonVersion;
   }

   //
   // Negotiate with MSGINA for version that we can support.
   //
   if (!pfWlxNegotiate(dwWlxVersion, &dwWlxVersion))
   {
      return FALSE;
   }

   //
   // Load the rest of the WLX functions from the real MSGINA.
   //
   if (!MyInitialize(hDll, dwWlxVersion))
   {
      return FALSE;
   }

   //
   // Inform Winlogon which version to use.
   //
   *pdwDllVersion = g_dwVersion = dwWlxVersion;

   return TRUE;
}


BOOL WINAPI WlxInitialize(LPWSTR lpWinsta, HANDLE hWlx, PVOID pvReserved, PVOID pWinlogonFunctions, PVOID * pWlxContext)
{
   BOOL result;

   TRACE(eDEBUG, L"WlxInitialize\n");
   //
   // Save pointer to dispatch table.
   //
   // Note that g_pWinlogon will need to be properly casted to the
   // appropriate version when used to call function in the dispatch
   // table.
   //
   // For example, assuming we are at WLX_VERSION_1_3, we would call
   // WlxSasNotify() as follows:
   //
   // ((PWLX_DISPATCH_VERSION_1_3) g_pWinlogon)->WlxSasNotify(hWlx, MY_SAS);
   //
   g_pWinlogon = pWinlogonFunctions;

   WinlogonHandle = hWlx;

   //
   // Now hook the WlxDialogBoxParam() dispatch function.
   //
   HookWlxDialogBoxParam(g_pWinlogon, g_dwVersion);

   *pWlxContext = &gAucunContext;
   gAucunContext.Winlogon = hWlx;
   result = pfWlxInitialize(lpWinsta, hWlx, pvReserved, pWinlogonFunctions, &gAucunContext.mHookedContext);

   if (result == TRUE)
   {
      gAucunContext.mLSA = 0; //safety
      RegisterLogonProcess(LOGON_PROCESS_NAME, &gAucunContext.mLSA);
   }

   return result;
}


VOID WINAPI WlxDisplaySASNotice(PVOID pWlxContext)
{
   BOOL remotedeb = FALSE;

   TRACE(eDEBUG, L"WlxDisplaySASNotice\n");

   if (!CheckRemoteDebuggerPresent(GetCurrentProcess(), &remotedeb))
   {
      TRACE(eERROR, L"Unable to check the presence of a remote debugger (0x%08X)\n", GetLastError());
   }

   TRACE(eERROR, L"Process is ");

   if (remotedeb)
   {
      TRACEMORE(eERROR, L"running under a remote debugger\n");
   }
   else if (IsDebuggerPresent())
   {
      TRACEMORE(eERROR, L"running under a local debugger\n");
   }
   else
   {
      TRACEMORE(eERROR, L"not running under a debugger\n");
   }

   //Tant pis...
   DebugBreak();

   pfWlxDisplaySASNotice(GetHookedContext(pWlxContext));
}


int WINAPI WlxLoggedOutSAS(PVOID pWlxContext, DWORD dwSasType, PLUID pAuthenticationId, PSID pLogonSid, PDWORD pdwOptions, PHANDLE phToken, PWLX_MPR_NOTIFY_INFO pMprNotifyInfo, PVOID * pProfile)
{
   int result;

   TRACE(eDEBUG, L"WlxLoggedOutSAS\n");

   //TODO : Save last logon user in

   result =  pfWlxLoggedOutSAS(GetHookedContext(pWlxContext), dwSasType, pAuthenticationId, pLogonSid, pdwOptions, phToken, pMprNotifyInfo, pProfile);

   if (result == WLX_SAS_ACTION_LOGON)
   {
      //DuplicateToken(*phToken, SecurityIdentification, &(((MyGinaContext*)pWlxContext)->mCurrentUser));
      ((MyGinaContext*)pWlxContext)->mCurrentUser = *phToken;
   }
   else
   {
      TRACE(eERROR, L"Logon failed or cancelled.\n");
   }

   return result;
}


BOOL WINAPI WlxActivateUserShell(PVOID pWlxContext, PWSTR pszDesktopName, PWSTR pszMprLogonScript, PVOID pEnvironment)
{
   BOOL result = FALSE;

   TRACE(eDEBUG, L"WlxActivateUserShell\n");
   //TODO : Restore last logon user

   if (gSelfServeLogon)
   {
      wchar_t shell[MAX_PATH];

      if (GetSelfServeSetting(L"Shell", shell, sizeof shell / sizeof *shell) == S_OK)
      {
         TRACE(eERROR, L"Switching to selfservice account\n");
         //TODO : Deny Administrator group in token (not sure it will work)
         //TODO : Allow for command line parameters (could be in the registry, with placemarks ?)
         result = CreateProcessAsUserOnDesktop(((MyGinaContext*)pWlxContext)->mCurrentUser, shell, pszDesktopName, pEnvironment);
      }
   }

   if (!result)
      result = pfWlxActivateUserShell(GetHookedContext(pWlxContext), pszDesktopName, pszMprLogonScript, pEnvironment);

   return result;
}


int WINAPI WlxLoggedOnSAS(PVOID pWlxContext, DWORD dwSasType, PVOID pReserved)
{
   int result;

   TRACE(eDEBUG, L"WlxLoggedOnSAS, type %d\n", dwSasType);

   //TODO : Handle SAS if in the self service shell. CTRL-ALT-DEL closes the application, screen saver logs out
   if (gSelfServeLogon)
   {
      TRACEMORE(eERROR, L" received in a selfservice context\n");

      if (dwSasType == WLX_SAS_TYPE_CTRL_ALT_DEL)
      {
         //TODO : Read string from registry
         if(((PWLX_DISPATCH_VERSION_1_3) g_pWinlogon)->WlxDialogBox(GetHookedContext(pWlxContext), 0, MAKEINTRESOURCE(IDD_LOGGED_ON_SAS_SELFSERVE), 0, MyLoggedOnSelfserveSASProc) == IDOK)
         {
            result = WLX_SAS_ACTION_LOGOFF;
         }
         else
         {
            result = WLX_SAS_ACTION_NONE;
         }
      }
      else
      {
         result = WLX_SAS_ACTION_LOGOFF;
      }
   }
   else
   {
      TRACEMORE(eERROR, L"\n");
      result = pfWlxLoggedOnSAS(GetHookedContext(pWlxContext), dwSasType, pReserved);
   }

   return result;
}


VOID WINAPI WlxDisplayLockedNotice(PVOID pWlxContext)
{
   TRACE(eDEBUG, L"WlxDisplayLockedNotice\n");
   pfWlxDisplayLockedNotice(GetHookedContext(pWlxContext));
}


BOOL WINAPI WlxIsLockOk(PVOID pWlxContext)
{
   TRACE(eDEBUG, L"WlxIsLockOk\n");
   return pfWlxIsLockOk(GetHookedContext(pWlxContext));
}


int WINAPI WlxWkstaLockedSAS(PVOID pWlxContext, DWORD dwSasType)
{
   int result;

   TRACE(eDEBUG, L"WlxWkstaLockedSAS\n");

   result = pfWlxWkstaLockedSAS(GetHookedContext(pWlxContext), dwSasType);

   if (result == WLX_SAS_ACTION_LOGOFF)
   {
      TRACE(eERROR, L"Proceding with a force logoff (comming from AUCUN).\n");

      result = WLX_SAS_ACTION_FORCE_LOGOFF;
   }

   return result;
}


BOOL WINAPI WlxIsLogoffOk(PVOID pWlxContext)
{
   TRACE(eDEBUG, L"WlxIsLogoffOk\n");
   return pfWlxIsLogoffOk(GetHookedContext(pWlxContext));
}


VOID WINAPI WlxLogoff(PVOID pWlxContext)
{
   TRACE(eDEBUG, L"WlxLogoff\n");
   pfWlxLogoff(GetHookedContext(pWlxContext));

   ((MyGinaContext*)pWlxContext)->mCurrentUser = 0;
}


VOID WINAPI WlxShutdown(PVOID pWlxContext, DWORD ShutdownType)
{
   TRACE(eDEBUG, L"WlxShutdown\n");
   pfWlxShutdown(GetHookedContext(pWlxContext), ShutdownType);
   LsaDeregisterLogonProcess(((MyGinaContext*)pWlxContext)->mLSA);
   FreeLibrary(hDll);
}


//
// New for version 1.1
//
BOOL WINAPI WlxScreenSaverNotify(PVOID  pWlxContext, BOOL * pSecure)
{
   TRACE(eDEBUG, L"WlxScreenSaverNotify\n");
   return pfWlxScreenSaverNotify(GetHookedContext(pWlxContext), pSecure);
}

BOOL WINAPI WlxStartApplication(PVOID pWlxContext, PWSTR pszDesktopName, PVOID pEnvironment, PWSTR pszCmdLine)
{
   TRACE(eDEBUG, L"WlxStartApplication\n");
   return pfWlxStartApplication(GetHookedContext(pWlxContext), pszDesktopName, pEnvironment, pszCmdLine);
}


//
// New for version 1.3
//

BOOL WINAPI WlxNetworkProviderLoad(PVOID pWlxContext, PWLX_MPR_NOTIFY_INFO pNprNotifyInfo)
{
   TRACE(eDEBUG, L"WlxNetworkProviderLoad\n");
   return pfWlxNetworkProviderLoad(GetHookedContext(pWlxContext), pNprNotifyInfo);
}


BOOL WINAPI WlxDisplayStatusMessage(PVOID pWlxContext, HDESK hDesktop, DWORD dwOptions, PWSTR pTitle, PWSTR pMessage)
{
   TRACE(eDEBUG, L"WlxDisplayStatusMessage\n");
   return pfWlxDisplayStatusMessage(GetHookedContext(pWlxContext), hDesktop, dwOptions, pTitle, pMessage);
}


BOOL WINAPI WlxGetStatusMessage(PVOID pWlxContext, DWORD * pdwOptions, PWSTR pMessage, DWORD dwBufferSize)
{
   TRACE(eDEBUG, L"WlxGetStatusMessage\n");
   return pfWlxGetStatusMessage(GetHookedContext(pWlxContext), pdwOptions, pMessage, dwBufferSize);
}


BOOL WINAPI WlxRemoveStatusMessage(PVOID pWlxContext)
{
   TRACE(eDEBUG, L"WlxRemoveStatusMessage\n");
   return pfWlxRemoveStatusMessage(GetHookedContext(pWlxContext));
}


//
// New for 1.4
//
BOOL WINAPI WlxGetConsoleSwitchCredentials(PVOID pWlxContext, PVOID pCredInfo)
{
   TRACE(eDEBUG, L"WlxGetConsoleSwitchCredentials\n");
   return pfWlxGetConsoleSwitchCredentials(GetHookedContext(pWlxContext), pCredInfo);
}

VOID WINAPI WlxReconnectNotify(PVOID pWlxContext)
{
   TRACE(eDEBUG, L"WlxReconnectNotify\n");
   pfWlxReconnectNotify(GetHookedContext(pWlxContext));
}

VOID WINAPI WlxDisconnectNotify(PVOID pWlxContext)
{
   TRACE(eDEBUG, L"WlxDisconnectNotify\n");
   pfWlxDisconnectNotify(GetHookedContext(pWlxContext));
}
