#ifdef _DEBUG
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
EXTERN BOOLEAN ExtractTokenOwner( HANDLE token, wchar_t *csOwner_o, size_t size);
EXTERN void OutputDebugStringError(DWORD dw);


#define TRACE(s) OutputDebugString(s);
#define TRACELN(s) OutputDebugString(s);OutputDebugString(L"\n");

#endif
#else

#define TRACE(s) ;
#define TRACELN(s) ;

#endif