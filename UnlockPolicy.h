#ifndef __UNLOCKPOLICY_H_
#define __UNLOCKPOLICY_H_

#ifndef EXTERN
#ifdef __cplusplus
#define EXTERN extern "C"
#else
#define EXTERN extern
#endif
#endif


enum
{
	eNotSureYet,
	eLetMSGINAHandleIt,
	eUnlock,
	eForceLogoff,
};

EXTERN HANDLE ConvertToImpersonationToken(HANDLE token);
EXTERN int ShouldUnlockForUser(const wchar_t *domain, const wchar_t *username, const wchar_t *password);
EXTERN HRESULT UsagerEstDansGroupe(HANDLE usager, const wchar_t *groupe);
EXTERN HANDLE GetCurrentLoggedOnUserToken();

#endif