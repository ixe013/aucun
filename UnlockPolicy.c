#include <windows.h>
#include "Settings.h"
#include "UnlockPolicy.h"

#define MAX_GROUPNAME 512


EXTERN BOOLEAN ShouldUnlockForUser(const wchar_t *domain, const wchar_t *username, const wchar_t *password)
{
	BOOLEAN result = FALSE;
	HANDLE token = 0;

	wchar_t allowed[MAX_GROUPNAME] = L"";
	wchar_t banned[MAX_GROUPNAME] = L"";

	//Banned (blacklisted) group is optional
	GetBannedGroupName(banned, sizeof banned);

	if(SUCCEEDED(GetAllowedGroupName(allowed, sizeof allowed)))
	{
		//Let's see if we can authenticate the user (this will generate a event log entry if the policy requires it)
		//if (LogonUserEx(username, domain, password, LOGON32_LOGON_UNLOCK, LOGON32_PROVIDER_DEFAULT, &token, 0, 0, 0, 0))
		if (LogonUser(username, domain, password, LOGON32_LOGON_UNLOCK, LOGON32_PROVIDER_DEFAULT, &token))
		{
			SECURITY_IMPERSONATION_LEVEL sil;
			DWORD cbsil = sizeof sil;

			//LOGON32_LOGON_UNLOCK returns a primary token, we need a impersonation token with identify access only
			if(GetTokenInformation(token, TokenImpersonationLevel, (LPVOID)&sil, sizeof sil, &cbsil) == 0)
			{
				HANDLE imptoken = 0;

				//Change to an impersonation token
				if(DuplicateToken(token, SecurityIdentification, &imptoken))
				{
					if(*banned)
					{
						if(UsagerEstDansGroupe(imptoken, banned) == S_OK)
						{
							*allowed = 0; //Banned group takes precedence over allowed group
							              //a terminated allowed buffer is used here as a flag
						}
					}

					if(*allowed && UsagerEstDansGroupe(imptoken, allowed) == S_OK)
					{
						result = TRUE;
					}

					CloseHandle(imptoken);
				}

				CloseHandle(token);
			}
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

