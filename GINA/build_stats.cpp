//$Id$
//$URL$
#include "build_stats.h"
#include <windows.h>
#include "global.h"

HINSTANCE hAucunDll;

void OutputBuildStats()
{
   OutputDebugString(L"Aucun, build timestamp : ");
   OutputDebugStringA(__TIMESTAMP__);
   OutputDebugStringA("\n");
   OutputDebugStringA("http://www.paralint.com/projects/aucun/\n");
}

BOOLEAN WINAPI DllMain(HINSTANCE hDll, DWORD nReason, LPVOID Reserved)
{
   BOOLEAN bSuccess = TRUE;

   switch (nReason)
   {
      case DLL_PROCESS_ATTACH:
         //  For optimization.
         DisableThreadLibraryCalls(hDll);
         
         OutputBuildStats();
         hAucunDll = hDll;  
         break;

      case DLL_PROCESS_DETACH:
         break;
   }

   return TRUE;
}
//  end DllMain

