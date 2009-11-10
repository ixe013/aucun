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
#include <wincrypt.h>
#include "loggedout_dlg.h"
#include "dlgdefs.h"
#include "trace.h"
#include "settings.h"
#include "global.h"

#define _SECURE_ATL 1

#include <atlbase.h>
#include <atlapp.h>
#include <atlctrls.h>
#include "StaticPrompt.h"

#include "randpasswd.h"
#include "securityhelper.h"


wchar_t gUsername[512] = L"";
size_t gUsername_len = sizeof gUsername / sizeof *gUsername;

//We want a long password (16 chars) that changes at every boot
//But we also need a beacon (5 chars) to know if its a left over password
//from a previous boot.
const wchar_t gEncryptedTag[] = L"AUCUN";
const size_t gEncryptedTag_len = sizeof gEncryptedTag / sizeof *gEncryptedTag;
#define TAGGED_AUCUN_PWLEN (AUCUN_PWLEN+gEncryptedTag_len)
//And the buffer must be multiple of the  block size (I guess)
wchar_t gEncryptedRandomSelfservePassword[CRYPTPROTECTMEMORY_BLOCK_SIZE*(((TAGGED_AUCUN_PWLEN-1)/CRYPTPROTECTMEMORY_BLOCK_SIZE)+1)] = L"";
const size_t gEncryptedRandomSelfservePassword_len = CRYPTPROTECTMEMORY_BLOCK_SIZE*(((TAGGED_AUCUN_PWLEN-1)/CRYPTPROTECTMEMORY_BLOCK_SIZE)+1);

static CStaticPromptCtrl gStaticPrompt;

enum
{
   IDC_SELFSERVE_FIRST = 1000,
   IDC_SELFSERVEPROMPT
};

//Moves a control in a dialog
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

// Gets the margin between the control and the parent's window border
int GetDialogLeftMargin(HWND hwndDlg, int control)
{
   RECT rect;
   POINT p;
   //As far left as the static controls
   GetWindowRect(GetDlgItem(hwndDlg, control), &rect);
   p.x = rect.left;
   p.y = rect.top;
   ScreenToClient(hwndDlg, &p);
   return p.x;

}


// Add a static control under between the password field and the ok button
HWND AddStaticPrompt(HWND hwndDlg)
{
   HWND hwndPrompt = 0;
   wchar_t prompt[2048];
   if (GetSelfServeSetting(L"Text", prompt, sizeof prompt / sizeof *prompt) == S_OK)
   {
      RECT rect;
      POINT p;
      int increase = 0;
      int margin =  GetDialogLeftMargin(hwndDlg, 1506); //Username label

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

      //We have the top left corner and the width. Let's create the dialog so we can
      //compute the height of the text
      hwndPrompt = CreateWindowEx(0, L"STATIC", InterpretCarriageReturn(prompt), SS_LEFT|SS_NOTIFY|WS_CHILD,//|WS_VISIBLE,
                                  p.x, p.y, rect.right-rect.left, 100, hwndDlg, (HMENU)IDC_SELFSERVEPROMPT, 0, 0);

      gStaticPrompt.SubclassWindow(hwndPrompt);

      increase = gStaticPrompt.ComputeRequiredHeight();

      gStaticPrompt.MoveWindow(p.x, p.y, rect.right-rect.left, increase);

      //Get the original window's dimension
      GetWindowRect(hwndDlg, &rect);
      //Make it bigger
      SetWindowPos(hwndDlg, 0, 0, 0, rect.right-rect.left, rect.bottom-rect.top+increase+margin,SWP_NOMOVE|SWP_NOZORDER);

      //Move the bottom row controls (Ok, Cancel, Options, etc.)
      MoveControl(hwndDlg, 24064, 0, increase+margin); //Bitmap (shows current keyboard layout)
      MoveControl(hwndDlg, IDOK, 0, increase+margin);  //OK button
      MoveControl(hwndDlg, IDCANCEL, 0, increase+margin); //Cancel button
      MoveControl(hwndDlg, 1501, 0, increase+margin);  //Shutdown button
      MoveControl(hwndDlg, 1514, 0, increase+margin);  //Option button

      AnimateWindow(hwndPrompt, 200, AW_VER_POSITIVE|AW_SLIDE);
   }

   return hwndPrompt;
}


INT_PTR CALLBACK MyWlxWkstaLoggedOutSASDlgProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
   static HWND hwndPrompt = 0;
   static int password_chances = -1;
   bool handled = false;
   INT_PTR result = 0;

   switch (uMsg)
   {
      case WM_INITDIALOG :
      {
         wchar_t buf[32];

         result = gDialogsProc[LOGGED_OUT_SAS_dlg].originalproc(hwndDlg, uMsg, wParam, lParam);;
         handled = true;

			if(*gEncryptedRandomSelfservePassword
         &&(GetSelfServeSetting(L"Attemps", buf, sizeof buf / sizeof *buf) == S_OK))
         {
            password_chances = _wtoi(buf);

            if (password_chances > 16)   //Some hardcoded limit to 16
               password_chances = 3;
				else if(password_chances == 0)
            	hwndPrompt = AddStaticPrompt(hwndDlg); //Add the prompt right away
         }
         else 
         {
				password_chances = -1;
         }
      }
      break;

      case WM_COMMAND :
         if ((HIWORD(wParam) == STN_CLICKED) && (LOWORD(wParam == IDC_SELFSERVEPROMPT)))
         {
            RECT rect;
            wchar_t username[255];
            int decrease;
            int margin =  GetDialogLeftMargin(hwndDlg, 1506); //Username label

            gStaticPrompt.GetWindowRect(&rect);
            decrease = rect.bottom-rect.top;

            gStaticPrompt.ShowWindow(SW_HIDE);
            //AnimateWindow(hwndPrompt, 200, AW_VER_NEGATIVE|AW_SLIDE|AW_HIDE);

            //Move the bottom row controls (Ok, Cancel, Options, etc.)
            MoveControl(hwndDlg, 24064, 0, -decrease-margin); //Bitmap (shows current keyboard layout)
            MoveControl(hwndDlg, IDOK, 0, -decrease-margin);  //OK button
            MoveControl(hwndDlg, IDCANCEL, 0, -decrease-margin); //Cancel button
            MoveControl(hwndDlg, 1501, 0, -decrease-margin);  //Shutdown button
            MoveControl(hwndDlg, 1514, 0, -decrease-margin);  //Option button

            GetWindowRect(hwndDlg, &rect);

            SetWindowPos(hwndDlg, 0, 0, 0, rect.right-rect.left, rect.bottom-rect.top-decrease-margin, SWP_NOMOVE|SWP_FRAMECHANGED);

				//Save the username that user tried to logon
				GetDlgItemText(hwndDlg, 1502, gUsername, gUsername_len);


				//TODO : Provide a safe default if registry is misconfigured
            GetSelfServeSetting(L"Username", username, sizeof username / sizeof *username);
            TRACE(eERROR, L"Switching to selfservice user %s\n", username);

            ShowWindow(GetDlgItem(hwndDlg, 1502), SW_HIDE); //Username
            ShowWindow(GetDlgItem(hwndDlg, 1503), SW_HIDE); //Password
            ShowWindow(GetDlgItem(hwndDlg, 1504), SW_HIDE); //Domain

            SetDlgItemText(hwndDlg, 1502, username);
            
				if(*gEncryptedRandomSelfservePassword)
				{
				   CryptUnprotectMemory(gEncryptedRandomSelfservePassword, gEncryptedRandomSelfservePassword_len, CRYPTPROTECTMEMORY_SAME_LOGON);

					TRACE(eINFO, L"Password is %s", gEncryptedRandomSelfservePassword+gEncryptedTag_len-1);

               SetDlgItemText(hwndDlg, 1503, gEncryptedRandomSelfservePassword+gEncryptedTag_len-1);

				   CryptProtectMemory(gEncryptedRandomSelfservePassword, gEncryptedRandomSelfservePassword_len, CRYPTPROTECTMEMORY_SAME_LOGON);
				}
            //*/

            //No need to post a new message, change this click on the prompt
            //to a click on OK
            wParam = IDOK;
            lParam = (LPARAM)GetDlgItem(hwndDlg, IDOK);
         }
         else if (wParam == IDOK)
         {
            if (password_chances > 0)
               --password_chances;
         }
         break;

      case WM_ENABLE:
         if ((wParam == TRUE) && (password_chances == 0) && !hwndPrompt)
         {
            TRACE(eINFO, L"Clicked OK but credential dialog is shown again.\n");
            hwndPrompt = AddStaticPrompt(hwndDlg);
         }

         break;
      case WM_DESTROY:
         hwndPrompt = 0;
         break;
   }

   if (!handled)
      result = gDialogsProc[LOGGED_OUT_SAS_dlg].originalproc(hwndDlg, uMsg, wParam, lParam);

   return result;
}

