#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#ifndef EXTERN
#ifdef __cplusplus
#define EXTERN extern "C"
#else
#define EXTERN extern
#endif
#endif

EXTERN PVOID g_pWinlogon;
EXTERN MyGinaContext *pgAucunContext;
EXTERN HINSTANCE hDll;

#endif


