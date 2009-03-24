#include <windows.h>
#include "loggedout_dlg.h"
#include "dlgdefs.h"
#include "trace.h"

INT_PTR CALLBACK MyWlxWkstaLoggedOutSASDlgProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	INT_PTR bResult = FALSE;

	// We hook a click on OK
	if ((uMsg == WM_COMMAND) && (wParam == IDOK))
	{
		if(GetAsyncKeyState(VK_CONTROL) >> 1) //if the MSB is set, shifting the bits by one place will make a positive number 
				{
					TRACE(L"Switching to selfservice user\n");

					ShowWindow(GetDlgItem(hwndDlg, 1502), SW_HIDE); //Username
					ShowWindow(GetDlgItem(hwndDlg, 1503), SW_HIDE); //Password
					ShowWindow(GetDlgItem(hwndDlg, 1504), SW_HIDE); //Domain

					SetDlgItemText(hwndDlg, 1502, L"funny");
					SetDlgItemText(hwndDlg, 1503, L"asdf1234");
				}
	}

	if (!bResult)
		bResult = gDialogsProc[LOGGED_OUT_SAS_dlg].originalproc(hwndDlg, uMsg, wParam, lParam);

	return bResult;
}
