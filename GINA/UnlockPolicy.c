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
#include "Settings.h"
#include "UnlockPolicy.h"
#include "trace.h"
#include "debug.h"
#include "SecurityHelper.h"

//Converts a token to an impersonation token, if it is not already one
//
HANDLE ConvertToImpersonationToken(HANDLE token)
{
    HANDLE result = token;
    SECURITY_IMPERSONATION_LEVEL sil;
    DWORD cbsil = sizeof sil;

    //If we are not impersonating
    if(GetTokenInformation(token, TokenImpersonationLevel, (LPVOID)&sil, sizeof sil, &cbsil) == 0)
    {
        HANDLE imptoken = 0;

        //Change to an impersonation token
        if(DuplicateToken(token, SecurityIdentification, &imptoken))
        {
            result = imptoken;
            CloseHandle(token);
        }
    }

    return result;
}


EXTERN int ShouldUnlockForUser(HANDLE lsa, HANDLE current_user, const wchar_t* domain, const wchar_t* username, const wchar_t* password)
{
    int result = eLetMSGINAHandleIt; //secure by default
    HANDLE token = 0;
    wchar_t unlock[MAX_GROUPNAME] = L"";
    wchar_t logoff[MAX_GROUPNAME] = L"";
    //Get the groups early to ensure fail fast if GINA is not configured
    GetGroupName(gUnlockGroupName, unlock, sizeof unlock / sizeof * unlock);
    GetGroupName(gForceLogoffGroupName, logoff, sizeof logoff / sizeof * logoff);

    //Do we have anything to work with ?
    if(*unlock || *logoff)
    {
        BOOL logged_on = FALSE;
        DWORD win32Error;
        TRACE(eERROR, L"We have the %s and %s group.\n", (unlock && *unlock) ? unlock : L"--", (logoff && *logoff) ? logoff : L"--");

        //Let's see if we can authenticate the user (this will generate a event log entry if the policy requires it)
        if(lsa)
        {
            //TODO : Read Authentication package name from the registry
            //logged_on = CallLsaLogonUser(lsa, domain, username, password, L"unlockap", Unlock, 0, &token, 0, &win32Error);
            logged_on = CallLsaLogonUser(lsa, domain, username, password, 0, Unlock, 0, &token, 0, &win32Error);
        }
        else
        {
            logged_on = LogonUser(username, domain, password, LOGON32_LOGON_UNLOCK, LOGON32_PROVIDER_DEFAULT, &token);
            win32Error = GetLastError();
        }

        if(logged_on)
        {
            BOOL is_same_user;
            TRACE(eERROR, L"User logged in.\n");
            token = ConvertToImpersonationToken(token);

            //Sometimes, AUCUN failed to get the current logged on user
            //This is a fail safe. If something goes wrong with the detection, then
            //the regulare MSGINA logic will take over.
            if(current_user)
            {
                IsSameUser(current_user, token, &is_same_user);

                if(is_same_user)
                {
                    TRACE(eERROR, L"Same user, unlocking.\n");
                    result = eUnlock;
                }
                else
                {
                    TRACE(eERROR, L"Different user, ");

                    if(UsagerEstDansGroupe(token, unlock) == S_OK)
                    {
                        TRACEMORE(eERROR, L"in the unlock group, unlocking.\n");
                        result = eUnlock;
                    }
                    else if(UsagerEstDansGroupe(token, logoff) == S_OK)
                    {
                        TRACEMORE(eERROR, L"in the logoff group, forcing a logoff.\n");
                        result = eForceLogoff;
                    }
                    else
                    {
                        TRACEMORE(eERROR, L"no privileges we can handle.\n");
                    }
                }
            }

            CloseHandle(token);
        }
        else
        {
            TRACEMSG(win32Error);
        }
    }

    return result;
}


//----------------------------------------------------------------------
// Name        : UsagerEstDansGroupe
// Description : Genere un SID a partir du nom de groupe puis verifie
//               l'appartenance au groupe de l'usager. Si on nom d'usager
//               est passé au lieu du nom de groupe, cette fonction le
//               traite comme un groupe ne contenant que cet usager.
//               Attention au leaks si vous jouez la dedans... les SID
//               c'est pas joli
// Parametre   : [in] HANDLE usager
// Parametre   : [in] BSTR groupe
// Returns     : S_OK si l'usager est dans le groupe
//               S_FALSE si l'usager n'est pas dans le groupe
//               E_FAIL s'il y a un bobo (on ne trouve pas le groupe)
//----------------------------------------------------------------------
HRESULT UsagerEstDansGroupe(HANDLE usager, const wchar_t* groupe)
{
    HRESULT result = E_FAIL;
    SID_NAME_USE snu;
    WCHAR szDomain[256];
    DWORD dwSidSize = 0;
    DWORD dwSize = sizeof szDomain / sizeof * szDomain;

    if ((LookupAccountNameW(NULL, groupe, 0, &dwSidSize, szDomain, &dwSize, &snu) == 0)
            && (ERROR_INSUFFICIENT_BUFFER == GetLastError()))
    {
        SID* pSid = (SID*)malloc(dwSidSize);

        if (LookupAccountNameW(NULL, groupe, pSid, &dwSidSize, szDomain, &dwSize, &snu))
        {
            BOOL b;

            if (CheckTokenMembership(usager, pSid, &b))
            {
                if (b == TRUE)
                {
                    result = S_OK;
                }
            }
            else
            {
                result = S_FALSE;
            }
        }

        //Si tout vas bien (la presque totalitée des cas), on delete notre pointeur
        //avec le bon operateur.
        free(pSid);
    }

    return result;
}


BOOLEAN ShouldHookUnlockPasswordDialog(HANDLE token)
{
    BOOLEAN result = FALSE;
    wchar_t unlock[MAX_GROUPNAME] = L"";
    wchar_t excluded[MAX_GROUPNAME] = L"";
    wchar_t forcelogoff[MAX_GROUPNAME] = L"";

    //If there is either an unlock or force logoff group,
    if((GetGroupName(gUnlockGroupName, unlock, sizeof unlock / sizeof * unlock) == S_OK)
            || (GetGroupName(gForceLogoffGroupName, forcelogoff, sizeof forcelogoff / sizeof * forcelogoff) == S_OK))
    {
        TRACE(eERROR, L"Groups are set, ");

        //User must not be in the excluded group
        if(GetGroupName(gExcludedGroupName, excluded, sizeof excluded / sizeof * excluded) == S_OK)
        {
            //If is not blacklisted, return TRUE (so the dialog will be hooked)
            if(UsagerEstDansGroupe(token, excluded) != S_OK)
            {
                TRACEMORE(eERROR, L"user is not excluded, should hook.\n");
                result = TRUE;
            }
            else
            {
                TRACEMORE(eERROR, L"user is excluded and will get standard MSGINA behavior.\n");
            }
        }
        else
        {
            //There is no excluded group, let's hook !
            TRACEMORE(eERROR, L"but there is no excluded group, should hook.\n");
            result = TRUE;
        }
    }
    else
    {
        TRACE(eERROR, L"Neither %s or %s group present, shouldn't hook.\n", gUnlockGroupName, gForceLogoffGroupName);
    }

    return result;
}
