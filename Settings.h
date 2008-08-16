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
#define MAX_GROUPNAME 512

EXTERN HRESULT GetGroupName(const wchar_t *key, wchar_t *group, DWORD size);
EXTERN HRESULT GetNoticeText(const wchar_t *name, wchar_t *text, DWORD size);
EXTERN HRESULT GetDebugSetting(const wchar_t *name, wchar_t *text, DWORD size);

EXTERN const wchar_t* gUnlockGroupName;
EXTERN const wchar_t* gForceLogoffGroupName;
EXTERN const wchar_t* gExcludedGroupName;

#endif