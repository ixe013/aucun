#include <windows.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <tchar.h>
#include "settings.h"

#define MAX_TRACE_BUFFER 1024

typedef int (*WriteBufferProc)(const wchar_t *buffer);

void TraceInBuffer(const wchar_t* format, wchar_t* buffer, size_t size, ...)
{
	va_list args;

	va_start(args, format);

	if(size && (vswprintf_s(buffer, size, format, args) <= 0))
	{
		wcscpy_s(buffer, size, L"buf2small");
	}
}

void TracePlain(const wchar_t* format, ...)
{
	wchar_t buffer[MAX_TRACE_BUFFER];
	va_list args;

	va_start(args, format);

	if(vswprintf_s(buffer, sizeof buffer / sizeof *buffer, format, args) <= 0)
	{
		wcscpy_s(buffer, sizeof buffer / sizeof *buffer, L"some error occured trying to format the trace buffer");
	}
}

int WriteBufferToStream(const wchar_t *buffer)
{
	int result = 0;

	return result;
}

int WriteBufferToOutputDebugString(const wchar_t *buffer)
{
	OutputDebugString(buffer);
	return 1;
}

WriteBufferProc GetOutputWriter()
{
	static DWORD lastcheck = 0;
	DWORD current = GetTickCount();
	WriteBufferProc result = 0;

	if((current - lastcheck) > 60000)
	{
		wchar_t buffer[512];
		if(GetDebugSetting(L"Output", buffer, sizeof buffer) == S_OK)
		{
			//Is it output debug string ?
			if(_wcsicmp(L"OutputDebugString", buffer) == 0)
			{
				result = &WriteBufferToOutputDebugString;
			}
			else
			{
				result = &WriteBufferToStream;
			}
		}

		lastcheck = current;
	}
	
	return result;
}


FILE OpenTraceOutput(const wchar_t* streamname);