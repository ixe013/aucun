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
EXTERN BOOL GetLogonSid(HANDLE htok, void* psid, DWORD cbMax);
EXTERN BOOL GetLogonSessionId(HANDLE htok, LUID* pluid);
EXTERN BOOL ExtractProfilePath(wchar_t** ppProfilePath, MSV1_0_INTERACTIVE_PROFILE* pProfile);
EXTERN BOOL AllocWinLogonProfile(WLX_PROFILE_V1_0** ppWinLogonProfile, const wchar_t* profilePath);
EXTERN BOOL CreateProcessAsUserOnDesktop(HANDLE hToken, wchar_t* programImage, wchar_t* desktop, void* env);
EXTERN BOOL ImpersonateAndGetUserName(HANDLE hToken, wchar_t* name, int cch);
EXTERN BOOL IsSameUser(HANDLE hToken1, HANDLE hToken2, BOOLEAN* pbIsSameUser);
EXTERN BOOL IsAdmin(HANDLE hToken);

#endif
