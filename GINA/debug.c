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

#ifdef _DEBUG
#include <windows.h>
#include "trace.h"
#include "debug.h"

BOOLEAN GetLUIDFromToken(HANDLE token, PLUID session)
{
    BOOLEAN result = FALSE;
    TOKEN_STATISTICS stats = {0};
    DWORD nbstats = sizeof stats;
    GetTokenInformation(token, TokenStatistics, &stats, sizeof stats, &nbstats);

    *session = stats.AuthenticationId;

    return result;
}

void OutputGetSessionUserName(PLUID session)
{
    PSECURITY_LOGON_SESSION_DATA sessionData = NULL;
    NTSTATUS retval;

    // Check for a valid session.
    if (!session ) {
        OutputDebugString(L"Error - Invalid logon session identifier.\n");
        return;
    }
    // Get the session information.
    retval = LsaGetLogonSessionData (session, &sessionData);
    if (!retval)
    {
        // Determine whether there is session data to parse.
        if (!sessionData)
        { // no data for session
            OutputDebugString(L"Invalid logon session data. \n");
        }
        if (sessionData->UserName.Buffer != NULL)
        {
            OutputDebugString(L"User from LUID is ");
            OutputDebugString((sessionData->UserName).Buffer);
            OutputDebugString(L"\n");
        }
        // Free the memory returned by the LSA.
    }
    if (sessionData)
        LsaFreeReturnBuffer(sessionData);
    return;
}

#endif
