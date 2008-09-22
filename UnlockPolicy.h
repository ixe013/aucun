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
	eLetMSGINAHandleIt=1,
	eUnlock,
	eForceLogoff,
};

EXTERN HANDLE ConvertToImpersonationToken(HANDLE token);
EXTERN int ShouldUnlockForUser(HANDLE lsa, HANDLE current_user, const wchar_t *domain, const wchar_t *username, const wchar_t *password);
EXTERN BOOLEAN ShouldHookUnlockPasswordDialog(HANDLE token);
EXTERN HRESULT UsagerEstDansGroupe(HANDLE usager, const wchar_t *groupe);

#endif