#include <windows.h>
#include "settings.h"

const wchar_t* gUnlockGroupName = L"unlock";
const wchar_t* gForceLogoffGroupName = L"force logoff";
const wchar_t* gExcludedGroupName = L"excluded";


HRESULT GetGroupName(const wchar_t *name, wchar_t *group, DWORD size)
{
   HRESULT result = E_FAIL;
   DWORD type;
   DWORD returnedsize = size;

   HKEY reg;

   RegOpenKey(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Paralint.com\\Aucun\\Groups", &reg);

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

