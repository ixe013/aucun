#ifndef __DEBUG_H__
#define __DEBUG_H__

#include <windows.h>
#include <Ntsecapi.h>

#ifndef EXTERN
#ifdef __cplusplus
#define EXTERN extern "C"
#else
#define EXTERN extern
#endif
#endif

EXTERN LPCWSTR GetWindowsMessageName(UINT msg);

EXTERN BOOLEAN GetLUIDFromToken(HANDLE token, PLUID session);
EXTERN void OutputGetSessionUserName(PLUID session);


#endif