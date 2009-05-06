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
#include <winwlx.h>

#include "Ginahook.h"
#include "trace.h"

#include "dlgdefs.h"
#include "loggedon_dlg.h"
#include "loggedout_dlg.h"
#include "locked_dlg.h"


//
// Pointers to redirected functions.
//
static PWLX_DIALOG_BOX_PARAM pfWlxDialogBoxParam = NULL;


//
// Local functions.
//
int WINAPI MyWlxDialogBoxParam(HANDLE, HANDLE, LPWSTR, HWND, DLGPROC, LPARAM);


//
// Hook WlxDialogBoxParam() dispatch function.
//
void HookWlxDialogBoxParam(PVOID pWinlogonFunctions, DWORD dwWlxVersion)
{
	//WlxDialogBoxParam
	pfWlxDialogBoxParam = ((PWLX_DISPATCH_VERSION_1_0) pWinlogonFunctions)->WlxDialogBoxParam;
	((PWLX_DISPATCH_VERSION_1_0) pWinlogonFunctions)->WlxDialogBoxParam = MyWlxDialogBoxParam;
}


//
// Redirected WlxDialogBoxParam() function.
//
int WINAPI MyWlxDialogBoxParam(HANDLE hWlx, HANDLE hInst, LPWSTR lpszTemplate, HWND hwndOwner, DLGPROC dlgprc, LPARAM dwInitParam)
{
	DLGPROC proc2use = dlgprc;
	DWORD dlgid = 0;

	TRACE(eERROR, L"About to create the dialog");
	//
	// We only know MSGINA dialogs by identifiers.
	//
	if (!HIWORD(lpszTemplate))
	{
		// Hook appropriate dialog boxes as necessary.
		int i;
		dlgid = LOWORD(lpszTemplate);    //Cast to remove warning C4311

		//Try to find the dialog
		for (i=0; i<nbDialogsAndProcs; ++i)
		{
			//Is it one of the ID we know ?
			if (gDialogsProc[i].IDD == dlgid)
			{
				//The dialog that asks if you would like to change password, lock, taskmgr, etc.
				TRACEMORE(eERROR, L" IDD %d (index %d)\n", dlgid, i);

				proc2use = gDialogsProc[i].dlgproc;
				gDialogsProc[i].originalproc = dlgprc;

				break;
			}
		}
	}

	if (proc2use == dlgprc)
	{
		TRACE(eERROR, L" (%d). it was not hooked.\n", dlgid);
	}

	return pfWlxDialogBoxParam(hWlx, hInst, lpszTemplate, hwndOwner, proc2use, dwInitParam);
}
