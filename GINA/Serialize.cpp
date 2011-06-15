#include <tchar.h>
#include <windows.h>
#include "Serialize.h"

static const TCHAR* gAvailableSemaphoreName = L"Local\\NA-06603283-A2A6-4fbc-A659-60851EC426DA";
static const TCHAR* gWaitingSemaphoreName = L"Local\\NW-D20C9303-7E39-452c-8E3C-CE6020778CB9";

static HANDLE gAvailableSemaphore = CreateSemaphore(0, 1, 1, gAvailableSemaphoreName);
static HANDLE gWaitingSemaphore = CreateSemaphore(0, 0, 1024, gWaitingSemaphoreName); //event



void SerializeEnter()
{
    if(SignalObjectAndWait(gWaitingSemaphore, gAvailableSemaphore, INFINITE, FALSE) == WAIT_OBJECT_0)
    {
        WaitForSingleObject(gWaitingSemaphore, INFINITE);
    }
}

void SerializeLeave()
{
    ReleaseSemaphore(gAvailableSemaphore, 1, 0);
    CloseHandle(gAvailableSemaphore);
    CloseHandle(gWaitingSemaphore);
}


