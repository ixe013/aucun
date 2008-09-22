#ifdef _DEBUG
#ifndef __DEBUG_H__
#define __DEBUG_H__

#include <windows.h>
#include <Ntsecapi.h>

#include "extern.h"

EXTERN LPCWSTR GetWindowsMessageName(UINT msg);

EXTERN BOOLEAN GetLUIDFromToken(HANDLE token, PLUID session);
EXTERN void OutputGetSessionUserName(PLUID session);
EXTERN BOOLEAN ExtractTokenOwner( HANDLE token, wchar_t *csOwner_o, size_t size);
EXTERN void OutputDebugStringError(DWORD dw);



#endif
#else


#endif