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

         TRACE(eERROR, L"Unlock notice will be displayed.\n");

         //Insert real \n caracters from the \n found in the string.
         wsprintf(message, InterpretCarriageReturn(text), unlock); //Will insert group name if there is a %s in the message

         result = ((PWLX_DISPATCH_VERSION_1_0) g_pWinlogon)->WlxMessageBox(hWlx, hDlg, message, caption, MB_YESNOCANCEL|MB_ICONEXCLAMATION);
      }
   }

   return result;

}

INT_PTR CALLBACK MyWlxWkstaLoggedOnSASDlgProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
   INT_PTR bResult = FALSE;

   // We hook a click on OK
   if ((uMsg == WM_COMMAND) && (wParam == 1800))
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
         switch (DisplayUnlockNotice(hwndDlg, pgAucunContext->Winlogon))
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

   if (!bResult)
      bResult = gDialogsProc[LOGGED_ON_SAS_dlg].originalproc(hwndDlg, uMsg, wParam, lParam);

   return bResult;
}


