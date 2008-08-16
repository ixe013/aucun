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

#ifndef __GINAHOOK_H__
#define __GINAHOOK_H__

//
// Function prototypes for the GINA interface.
//

typedef BOOL(WINAPI * PFWLXNEGOTIATE)(DWORD, DWORD *);
typedef BOOL(WINAPI * PFWLXINITIALIZE)(LPWSTR, HANDLE, PVOID, PVOID, PVOID *);
typedef VOID(WINAPI * PFWLXDISPLAYSASNOTICE)(PVOID);
typedef int(WINAPI * PFWLXLOGGEDOUTSAS)(PVOID, DWORD, PLUID, PSID, PDWORD,
                                        PHANDLE, PWLX_MPR_NOTIFY_INFO,
                                        PVOID *);
typedef BOOL(WINAPI * PFWLXACTIVATEUSERSHELL)(PVOID, PWSTR, PWSTR, PVOID);
typedef int(WINAPI * PFWLXLOGGEDONSAS)(PVOID, DWORD, PVOID);
typedef VOID(WINAPI * PFWLXDISPLAYLOCKEDNOTICE)(PVOID);
typedef int(WINAPI * PFWLXWKSTALOCKEDSAS)(PVOID, DWORD);
typedef BOOL(WINAPI * PFWLXISLOCKOK)(PVOID);
typedef BOOL(WINAPI * PFWLXISLOGOFFOK)(PVOID);
typedef VOID(WINAPI * PFWLXLOGOFF)(PVOID);
typedef VOID(WINAPI * PFWLXSHUTDOWN)(PVOID, DWORD);

//
// New for version 1.1
//

typedef BOOL(WINAPI * PFWLXSCREENSAVERNOTIFY)(PVOID, BOOL *);
typedef BOOL(WINAPI * PFWLXSTARTAPPLICATION)(PVOID, PWSTR, PVOID, PWSTR);

//
// New for version 1.3
//

typedef BOOL(WINAPI * PFWLXNETWORKPROVIDERLOAD)(PVOID, PWLX_MPR_NOTIFY_INFO);
typedef BOOL(WINAPI * PFWLXDISPLAYSTATUSMESSAGE)(PVOID, HDESK, DWORD, PWSTR, PWSTR);
typedef BOOL(WINAPI * PFWLXGETSTATUSMESSAGE)(PVOID, DWORD *, PWSTR, DWORD);
typedef BOOL(WINAPI * PFWLXREMOVESTATUSMESSAGE)(PVOID);

//
// New for version 1.4
//

typedef BOOL(WINAPI * PWLXGETCONSOLESWITCHCREDENTIALS)(PVOID, PVOID);
typedef VOID(WINAPI * PWLXRECONNECTNOTIFY)(PVOID);
typedef VOID(WINAPI * PWLXDISCONNECTNOTIFY)(PVOID);

typedef struct
{
	HANDLE mCurrentUser;
	HANDLE Winlogon;
	PVOID mHookedContext;
} MyGinaContext;


#endif
