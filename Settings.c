#include <windows.h>
#include "settings.h"

const wchar_t* gUnlockGroupName = L"unlock";
const wchar_t* gForceLogoffGroupName = L"force logoff";
const wchar_t* gExcludedGroupName = L"excluded";


HRESULT GetSettingText(const wchar_t *key, const wchar_t *name, wchar_t *text, DWORD size);

//Will return one of the following
//S_OK We have retrieved a group name
//E_FAIL We have not retrieved a value
HRESULT GetGroupName(const wchar_t *name, wchar_t *group, DWORD size)
{
	return GetSettingText(L"SOFTWARE\\Paralint.com\\Aucun\\Groups", name, group, size);
}

HRESULT GetNoticeText(const wchar_t *name, wchar_t *text, DWORD size)
{
	return GetSettingText(L"SOFTWARE\\Paralint.com\\Aucun\\Notice", name, text, size);
}

HRESULT GetDebugSetting(const wchar_t *name, wchar_t *text, DWORD size)
{
	return GetSettingText(L"SOFTWARE\\Paralint.com\\Aucun\\Debug", name, text, size);
}


HRESULT GetSettingText(const wchar_t *key, const wchar_t *name, wchar_t *text, DWORD size)
{
   HRESULT result = E_FAIL;
   DWORD type;
   DWORD returnedsize = size;

   HKEY reg;

   if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, key, 0, KEY_READ, &reg) == ERROR_SUCCESS)
   {
	   if (RegQueryValueEx(reg, name, 0, &type, (LPBYTE)text, &returnedsize) == ERROR_SUCCESS)
	   {
		  if ((type == REG_SZ) && (returnedsize < size) && (returnedsize > 0))
		  {
			 result = S_OK;
		  }
	   }

		RegCloseKey(reg);
   }

   return result;
}

