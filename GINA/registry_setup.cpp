#include <windows.h>
#include "registry_setup.h"

//reg add "HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Winlogon" /v DefaultUserName /t REG_SZ /d lastusername /f

HRESULT SetupSelfserveRegistry(const wchar_t *lastusername)
{
   HRESULT result = E_FAIL;
   HKEY reg;

   if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Winlogon", 0, KEY_WRITE, &reg) == ERROR_SUCCESS)
   {
      if (RegSetValueEx(reg, L"DefaultUserName", 0, REG_SZ, (LPBYTE)lastusername, wcslen(lastusername)+1) == ERROR_SUCCESS)
      {
            result = S_OK;
      }

      RegCloseKey(reg);
   }

   return result;
}

HRESULT TearDownSelfserveRegistry()
{
	BOOL result = FALSE;
	return result;
}

