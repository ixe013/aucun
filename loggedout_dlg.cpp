#include <windows.h>
#include "loggedout_dlg.h"
#include "dlgdefs.h"
#include "trace.h"

void MoveControl(HWND hwndDlg, int id, int xoffset, int yoffset)
{
	HWND control = GetDlgItem(hwndDlg, id);

	RECT rect;

	//Move the bottom row controls (Ok, Cancel, Options, etc.)
	GetWindowRect(control, &rect);
	POINT p;
	p.x = rect.left;
	p.y = rect.top;

	ScreenToClient(hwndDlg, &p);

	MoveWindow(control, p.x+xoffset, p.y+yoffset, rect.right-rect.left, rect.bottom-rect.top, TRUE);
}

INT_PTR CALLBACK MyWlxWkstaLoggedOutSASDlgProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_INITDIALOG :
		{
			RECT rect;

			//Get the original window's dimension
			GetWindowRect(hwndDlg, &rect);

			//Make it bigger
			SetWindowPos(hwndDlg, 0, 0, 0, rect.right-rect.left, rect.bottom-rect.top+150,SWP_NOMOVE|SWP_NOZORDER);

			//Move the bottom row controls (Ok, Cancel, Options, etc.)
			MoveControl(hwndDlg, 24064, 0, 150);
			MoveControl(hwndDlg, IDOK, 0, 150);
			MoveControl(hwndDlg, IDCANCEL, 0, 150);
			MoveControl(hwndDlg, 1501, 0, 150);
			MoveControl(hwndDlg, 1514, 0, 150);
		}
		break;

	case WM_COMMAND :
		// We hook a click on OK
		if(wParam == IDOK)
		{
			if(GetAsyncKeyState(VK_CONTROL)>>1)
			{
				TRACE(L"Switching to selfservice user\n");

				ShowWindow(GetDlgItem(hwndDlg, 1502), SW_HIDE); //Username
				ShowWindow(GetDlgItem(hwndDlg, 1503), SW_HIDE); //Password
				ShowWindow(GetDlgItem(hwndDlg, 1504), SW_HIDE); //Domain

				SetDlgItemText(hwndDlg, 1502, L"funny");
				SetDlgItemText(hwndDlg, 1503, L"asdf1234");
			}
		}
		break;
	}

	return gDialogsProc[LOGGED_OUT_SAS_dlg].originalproc(hwndDlg, uMsg, wParam, lParam);;
}
