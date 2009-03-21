#include "build_stats.h"
#include <windows.h>

#define asd
void OutputBuildStats()
{
	OutputDebugString(L"Aucun, build ");
	OutputDebugStringA(__TIMESTAMP__);
	OutputDebugStringA("\n");
	OutputDebugStringA("http://www.paralint.com/projects/aucun/\n");
}

BOOLEAN 
WINAPI 
DllMain(
    HINSTANCE hDllHandle, 
    IN DWORD     nReason,    
    IN LPVOID    Reserved    
)
{
    BOOLEAN bSuccess = TRUE;


//  Perform global initialization.

    switch ( nReason )
    {
        case DLL_PROCESS_ATTACH:

        //  For optimization.

            DisableThreadLibraryCalls( hDllHandle );
			OutputBuildStats();

            break;

        case DLL_PROCESS_DETACH:

            break;
    }


    return TRUE;

}
//  end DllMain

