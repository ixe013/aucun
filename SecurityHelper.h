// SecurityHelper.h
//
// Routines that interface with Win32 and LSA security APIs
//

#ifndef __SECURITY_HELPER_H__
#define __SECURITY_HELPER_H__

#include <windows.h>
#include <ntsecapi.h>
#include <winwlx.h>
#include "extern.h"

#define LOGON_PROCESS_NAME "AUCUN-http://paralint.com"


EXTERN BOOL RegisterLogonProcess(const char* logonProcessName, HANDLE* phLsa);
EXTERN BOOL CallLsaLogonUser(HANDLE hLsa,
                      const wchar_t* domain,
                      const wchar_t* user,
                      const wchar_t* pass,
                      SECURITY_LOGON_TYPE logonType,
                      LUID* pLogonSessionId,
                      HANDLE* phToken,
                      MSV1_0_INTERACTIVE_PROFILE** ppProfile,
                      DWORD* pWin32Error);
EXTERN BOOL IsSameUser(HANDLE hToken1, HANDLE hToken2, BOOL* pbIsSameUser);
EXTERN BOOL CreateProcessAsUserOnDesktop(HANDLE hToken, wchar_t* programImage, wchar_t* desktop, void* env);
EXTERN BOOL ImpersonateAndGetUserName(HANDLE hToken, wchar_t* name, int cch);
EXTERN int GetUsernameAndDomainFromToken(HANDLE token, wchar_t *domain, DWORD domain_len, wchar_t *username, DWORD username_len);

#endif
