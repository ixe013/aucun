#ifndef __UNLOCKPOLICY_H_
#define __UNLOCKPOLICY_H_

#ifndef EXTERN
#ifdef __cplusplus
#define EXTERN extern "C"
#else
#define EXTERN extern
#endif
#endif

EXTERN BOOLEAN ShouldUnlockForUser(const wchar_t *domain, const wchar_t *username, const wchar_t *password);
EXTERN HRESULT UsagerEstDansGroupe(HANDLE usager, const wchar_t *groupe);

#endif