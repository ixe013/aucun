#include <windows.h>
#include "settings.h"

HRESULT GetAllowedGroupName(wchar_t *group, DWORD size)
{
   HRESULT result = E_FAIL;
   DWORD type;
   DWORD returnedsize = size;

   HKEY reg;

   RegOpenKey(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Paralint.com\\Aucun", &reg);

   if (RegQueryValueEx(reg, L"GroupName", 0, &type, (LPBYTE)group, &returnedsize) == ERROR_SUCCESS)
   {
      if ((type == REG_SZ) && (returnedsize < size))
      {
         result = S_OK;
      }
   }

   RegCloseKey(reg);

   return result;
}

