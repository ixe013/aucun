#ifndef __SETTINGS_H__
#define __SETTINGS_H__

#ifndef EXTERN
#ifdef __cplusplus
#define EXTERN extern "C"
#else
#define EXTERN extern
#endif
#endif

#define MAX_DOMAIN  512
#define MAX_USERNAME 512
#define MAX_PASSWORD 512

EXTERN HRESULT GetGroupName(const wchar_t *key, wchar_t *group, DWORD size);

EXTERN const wchar_t* gUnlockGroupName;
EXTERN const wchar_t* gForceLogoffGroupName;

#endif