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

#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#include "extern.h"


typedef struct
{
    HANDLE mCurrentUser;
    HANDLE Winlogon;
    HANDLE mLSA;
    BOOL mLogonScenario;
    PVOID mHookedContext;
} MyGinaContext;

enum
{
    eRegularLogon,
    eAutoLogon,
    eSelfServeLogon,
    eSelfServeLogonTimeout,
};


EXTERN PVOID g_pWinlogon;
EXTERN MyGinaContext* pgAucunContext;
EXTERN HINSTANCE hAucunDll;
EXTERN HINSTANCE hDll;
EXTERN HINSTANCE hResourceDll;
EXTERN wchar_t gUsername[];
EXTERN size_t gUsername_len;
EXTERN const wchar_t gEncryptedTag[];
EXTERN const size_t gEncryptedTag_len;
EXTERN wchar_t gEncryptedRandomSelfservePassword[];
EXTERN const DWORD gEncryptedRandomSelfservePassword_len;

EXTERN const wchar_t gAucunWinlogonContext[];

#define AUCUN_PWLEN (16)

#endif


