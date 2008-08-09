#include <windows.h>
#include "Settings.h"
#include "UnlockPolicy.h"
#include "debug.h"

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

//Taken from Keith Brown Full Ginal Sample on MSJ (or MSDN mag, whatever)
HRESULT IsSameUser(HANDLE hToken1, HANDLE hToken2) 
{
	HRESULT result = E_FAIL; 

    BYTE buf1[sizeof(TOKEN_USER) + SECURITY_MAX_SID_SIZE];
    BYTE buf2[sizeof(TOKEN_USER) + SECURITY_MAX_SID_SIZE];

    DWORD cb;
    if (GetTokenInformation(hToken1, TokenUser, buf1, sizeof buf1, &cb) &&
		GetTokenInformation(hToken2, TokenUser, buf2, sizeof buf2, &cb)) 
	{
        if(EqualSid(((TOKEN_USER*)buf1)->User.Sid, ((TOKEN_USER*)buf2)->User.Sid))
		{
			result = S_OK;
		}
		else
		{
			result = S_FALSE;
		}
    }

    return result;
}


EXTERN int ShouldUnlockForUser(HANDLE current_user, const wchar_t *domain, const wchar_t *username, const wchar_t *password)
{
	int result = eLetMSGINAHandleIt; //secure by default
	HANDLE token = 0;

	wchar_t unlock[MAX_GROUPNAME] = L"";
	wchar_t logoff[MAX_GROUPNAME] = L"";

	//Get the groups early to ensure fail fast if GINA is not configured
	GetGroupName(gUnlockGroupName, unlock, sizeof unlock / sizeof *unlock);
	GetGroupName(gForceLogoffGroupName, logoff, sizeof logoff / sizeof *logoff);

	//Do we have anything to work with ?
	if(*unlock || *logoff)
	{
		TRACE(L"On a les groupes ");
		TRACE(unlock);
		TRACE(L" ");
		TRACELN(logoff);

		//Let's see if we can authenticate the user (this will generate a event log entry if the policy requires it)
		if (LogonUser(username, domain, password, LOGON32_LOGON_UNLOCK, LOGON32_PROVIDER_DEFAULT, &token))
		{
			HRESULT is_same_user;
			token = ConvertToImpersonationToken(token);

			//Sometimes, AUCUN failed to get the current logged on user
			//This is a fail safe. If something goes wrong with the detection, then
			//the regulare MSGINA logic will take over.
			if(current_user)
			{
				TRACE(L"We have a user");

				is_same_user = IsSameUser(current_user, token);

				if(is_same_user == S_OK)
				{
					TRACE(L" it is the same user");
					result = eUnlock; 
				}
				else if(is_same_user == S_FALSE)
				{
					if(UsagerEstDansGroupe(token, unlock) == S_OK)
					{
						TRACELN(L" dans le groupe unlock");
						result = eUnlock;
					}
					else if(UsagerEstDansGroupe(token, logoff) == S_OK)
					{
						TRACELN(L" dans le groupe dans le groupe logoff");

						result = eForceLogoff;
					}
				}
				//if(is_same_user == E_FAIL) ;//let msgina handle it.

				//CloseHandle(current_user);
			}

			CloseHandle(token);
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
HRESULT UsagerEstDansGroupe(HANDLE usager, const wchar_t *groupe)
{
	HRESULT result = E_FAIL;
	SID_NAME_USE snu;
	WCHAR szDomain[256];
	DWORD dwSidSize =0;

	DWORD dwSize = sizeof szDomain / sizeof *szDomain;

	if ((LookupAccountNameW(NULL, groupe, 0, &dwSidSize, szDomain, &dwSize, &snu) == 0)
		&& (ERROR_INSUFFICIENT_BUFFER == GetLastError()))
	{
		SID *pSid = (SID*)malloc(dwSidSize);

		if (LookupAccountNameW(NULL, groupe, pSid, &dwSidSize, szDomain, &dwSize, &snu))
		{
			BOOL b;

			if (CheckTokenMembership(usager, pSid, &b) && (b == TRUE))
			{
				result = S_OK;
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
	if((GetGroupName(gUnlockGroupName, unlock, sizeof unlock / sizeof *unlock) == S_OK)
	|| (GetGroupName(gForceLogoffGroupName, forcelogoff, sizeof forcelogoff / sizeof *forcelogoff) == S_OK))
	{
		//User must not be in the excluded group
		if(GetGroupName(gExcludedGroupName, excluded, sizeof excluded / sizeof *excluded) == S_OK)
		{
			//If is not blacklisted, return TRUE (so the dialog will be hooked)
			result = UsagerEstDansGroupe(token, excluded) == S_FALSE;
		}
		else
		{
			//There is no excluded group, let's hook !
			result = S_OK;
		}
	}

	return result;
}
