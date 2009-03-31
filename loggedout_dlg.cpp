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
#include <lm.h>
#include "loggedout_dlg.h"
#include "dlgdefs.h"
#include "trace.h"
#include "settings.h"

#define _SECURE_ATL 1

#include <atlbase.h>
#include <atlapp.h>
#include <atlctrls.h>
#include "StaticPrompt.h"

static CStaticPromptCtrl gStaticPrompt;

enum
{
	IDC_SELFSERVE_FIRST = 1000,
	IDC_SELFSERVEPROMPT
};


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
	static HWND hwndPrompt = 0;

	switch(uMsg)
	{
	case WM_INITDIALOG :
		{
			wchar_t prompt[2048];
			if(GetSelfServeSetting(L"Text", prompt, sizeof prompt / sizeof *prompt) == S_OK)
			{
				RECT rect;
				POINT p;
				int increase = 0;
				int margin;

				//TODO : Make a function that creates the STATIC control
				//TODO : Do no display all the time, detect a bad password attempt
				//Create a static prompt control
				//As far left as the static controls
				GetWindowRect(GetDlgItem(hwndDlg, 1506), &rect);
				margin = rect.left;

				//On the same line as the ok buttons (they will be moved)
				GetWindowRect(GetDlgItem(hwndDlg, IDOK), &rect);
				p.x = rect.left;
				p.y = rect.top;

				//As far right as the Option button
				GetWindowRect(GetDlgItem(hwndDlg, 1514), &rect);
				//As far left as the Ok button
				rect.left = p.x;
				
				//Here, rect contains the outer rectangle including all buttons from the bottom row
				//rect = left, top and bottom of IDOK,   right of 1514 (options);

				//Now convert the top-left coordinate in client for CreateWindow
				ScreenToClient(hwndDlg, &p);

				//GetClientRect(hwndDlg, &rect); //Make it as wide as the dialog box, minus margins

				hwndPrompt = CreateWindowEx(0, L"STATIC", prompt, SS_LEFT|SS_NOTIFY|WS_CHILD|WS_VISIBLE, 
					//p.x, p.y, rect.right-rect.left-p.x-margin, 100, hwndDlg, (HMENU)IDC_SELFSERVEPROMPT, 0, 0);
					p.x, p.y, rect.right-rect.left, 100, hwndDlg, (HMENU)IDC_SELFSERVEPROMPT, 0, 0);

				//TODO : Subclass the STATIC control
				gStaticPrompt.SubclassWindow(hwndPrompt);

				increase = gStaticPrompt.ComputeRequiredHeight();

				//gStaticPrompt.MoveWindow(p.x, p.y, rect.right-rect.left-p.x-margin, increase); 
				gStaticPrompt.MoveWindow(p.x, p.y, rect.right-rect.left, increase); 

				//Get the original window's dimension
				GetWindowRect(hwndDlg, &rect);
				//Make it bigger
				SetWindowPos(hwndDlg, 0, 0, 0, rect.right-rect.left, rect.bottom-rect.top+increase+margin,SWP_NOMOVE|SWP_NOZORDER);

				//Move the bottom row controls (Ok, Cancel, Options, etc.)
				MoveControl(hwndDlg, 24064, 0, increase+margin); //Bitmap (shows current keyboard layout)
				//MoveControl(hwndDlg, IDOK, 0, increase+margin);  //OK button
				MoveControl(hwndDlg, IDCANCEL, 0, increase+margin); //Cancel button
				MoveControl(hwndDlg, 1501, 0, increase+margin);  //Shutdown button
				MoveControl(hwndDlg, 1514, 0, increase+margin);  //Option button
			}
		}
		break;

	case WM_CTLCOLORSTATIC :
		//TODO : Move this to a C++ class that subclasses the STATIC control
		/*
		if(hwndPrompt == (HWND)lParam)
		{
			return (INT_PTR)GetSysColorBrush(COLOR_INFOBK);
		}
		*/
		break;
	case WM_COMMAND :
		if((HIWORD(wParam) == STN_CLICKED) && (LOWORD(wParam == IDC_SELFSERVEPROMPT)))
		{
				wchar_t username[255];

				GetSelfServeSetting(L"Username", username, sizeof username / sizeof *username);
				TRACE(L"Switching to selfservice user %s\n", username);

				ShowWindow(GetDlgItem(hwndDlg, 1502), SW_HIDE); //Username
				ShowWindow(GetDlgItem(hwndDlg, 1503), SW_HIDE); //Password
				ShowWindow(GetDlgItem(hwndDlg, 1504), SW_HIDE); //Domain

				//TODO : Change selfservice password here
				USER_INFO_1003 ui;
				ui.usri1003_password = L"asdf1243";

				NetUserSetInfo(0, username, 1003, (LPBYTE)&ui, 0);
				SetDlgItemText(hwndDlg, 1502, username);
				SetDlgItemText(hwndDlg, 1503, ui.usri1003_password);
		}
		break;
	}

	return gDialogsProc[LOGGED_OUT_SAS_dlg].originalproc(hwndDlg, uMsg, wParam, lParam);;
}
