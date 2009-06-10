#include "build_stats.h"
#include <windows.h>
#include "global.h"

void OutputBuildStats()
{
   OutputDebugString(L"Aucun, build timestamp : ");
   OutputDebugStringA(__TIMESTAMP__);
   OutputDebugStringA("\n");
   OutputDebugStringA("http://www.paralint.com/projects/aucun/\n");
}

BOOLEAN WINAPI DllMain(HINSTANCE hDllHandle, DWORD nReason, LPVOID Reserved)
{
   BOOLEAN bSuccess = TRUE;

   switch (nReason)
   {
      case DLL_PROCESS_ATTACH:
         //  For optimization.
         DisableThreadLibraryCalls(hDllHandle);
         OutputBuildStats();
         break;

      case DLL_PROCESS_DETACH:
         break;
   }

   return TRUE;
}
//  end DllMain

