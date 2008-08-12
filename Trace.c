#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <tchar.h>

int (*WriteBufferProc)(const wchar_t *buffer);


void Trace(const wchar_t* format, wchar_t* buffer, size_t size, ...)
{
	va_list args;

	va_start(args, format);

	if(size && (vswprintf_s(buffer, size, format, args) <= 0))
	{
		wcscpy_s(buffer, size, L"buf2small");
	}
}

void TraceFmt()
{
}

int WriteBufferToStream(const wchart_t *buffer)
{
	int result = 0;
	return result;
}

int WriteBufferToOutputDebugString(const wchart_t *buffer)
{
	int result = 0;
	OutputDebugString(buffer);
	return result;
}


FILE OpenTraceOutput(const wchar_t* streamname)