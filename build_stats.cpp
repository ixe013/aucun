#include "build_stats.h"
#include <windows.h>

#define asd
void OutputBuildStats()
{
   OutputDebugString(L"Aucun, build ");
   OutputDebugStringA(__TIMESTAMP__);
   OutputDebugStringA("\n");
   OutputDebugStringA("<?dml?><link cmd=\".shell start iexplore http://www.paralint.com/projects/aucun/\">Any user can unlock now custom GINA</link>\n");
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

