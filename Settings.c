#include <windows.h>
#include "settings.h"


HRESULT GetGroupName(const wchar_t *name, wchar_t *group, DWORD size)
{
   HRESULT result = E_FAIL;
   DWORD type;
   DWORD returnedsize = size;

   HKEY reg;

   RegOpenKey(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Paralint.com\\Aucun", &reg);

   if (RegQueryValueEx(reg, name, 0, &type, (LPBYTE)group, &returnedsize) == ERROR_SUCCESS)
   {
      if ((type == REG_SZ) && (returnedsize < size))
      {
         result = S_OK;
      }
   }

   RegCloseKey(reg);

   return result;
}

HRESULT GetAllowedGroupName(wchar_t *group, DWORD size)
{
	return GetGroupName(L"GroupName", group, size);
}

HRESULT GetBannedGroupName(wchar_t *group, DWORD size)
{
	return GetGroupName(L"BannedGroupName", group, size);
}

