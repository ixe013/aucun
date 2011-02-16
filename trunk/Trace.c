#include <windows.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <tchar.h>
#include "trace.h"
#include "settings.h"

#define MAX_TRACE_BUFFER 1024

typedef int (*WriteBufferProc)(LPCWSTR buffer);

int WriteBufferToStream(const wchar_t *buffer)
{
    //Targetted for release 1.5
    return 0;
}

int WriteBufferToOutputDebugString(const wchar_t *buffer)
{
    OutputDebugString(buffer);
    return 1;
}


/* -------------------------------------------------------------------------- */
/** @brief Reads the registry and returns a function that will do the logging
*
* @return
*/
/* ---------------------------------------------------------------------------- */
WriteBufferProc GetOutputWriter()
{
    WriteBufferProc result = 0;

    wchar_t buffer[512];
    if (GetDebugSetting(L"Output", buffer, sizeof buffer / sizeof *buffer) == S_OK)
    {
        //Is it output debug string ?
        if (_wcsicmp(L"OutputDebugString", buffer) == 0)
        {
            result = &WriteBufferToOutputDebugString;
        }
    }

    return result;
}


void Trace(const wchar_t* file, int line, const wchar_t *format, ...)
{
    WriteBufferProc output_proc = GetOutputWriter();

    if (output_proc)
    {
        wchar_t buffer[MAX_TRACE_BUFFER];
        va_list args;

        if (file && line)
        {
            swprintf_s(buffer, sizeof buffer / sizeof *buffer, L"[%s:%ld] ", file, line);
            output_proc(buffer);
        }

        va_start(args, format);

        if (format && vswprintf_s(buffer, sizeof buffer / sizeof *buffer, format, args) >= 0)
        {
            output_proc(buffer);
        }
    }
}

void TraceMessage(const wchar_t* file, int line, DWORD dw) 
{ 
	LPVOID lpMsgBuf;

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | 
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		dw,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR) &lpMsgBuf,
		0, NULL );

	Trace(file, line, L"0x%08X: %s", dw, lpMsgBuf); 

	LocalFree(lpMsgBuf);
}
