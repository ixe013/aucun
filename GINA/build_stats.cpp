//$Id$
#include "build_stats.h"
#include <windows.h>
#include "global.h"

HINSTANCE hAucunDll;

void OutputBuildStats()
{
#ifdef _WIN64
    OutputDebugString(L"Aucun 64 bits, build timestamp : ");
#else
    OutputDebugString(L"Aucun, build timestamp : ");
#endif
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

