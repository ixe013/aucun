#include "selfserve_sas_dlg.h"
#include "dlgdefs.h"
#include "trace.h"
#include "global.h"
//#include "unlockpolicy.h"
#include "settings.h"
#include <winwlx.h>
#include <winwlx.h>

#include "resource.h"

INT_PTR CALLBACK MyLoggedOnSelfserveSASProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
   INT_PTR bResult = FALSE;

   // We hook a click on OK
   if (!bResult)
      bResult = gDialogsProc[LOGGED_ON_SAS_dlg].originalproc(hwndDlg, uMsg, wParam, lParam);

   return bResult;
}


