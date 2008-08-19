#ifndef __TRACE_H__
#define __TRACE_H__

#define WIDEN2(x) L ## x
#define WIDEN(x) WIDEN2(x)
#define __WFILE__ WIDEN(__FILE__)

#define TRACE(s, ...) Trace(__WFILE__, __LINE__, s, __VA_ARGS__)
#define TRACEMORE(s, ...) Trace(0, 0, s, __VA_ARGS__)

#include "extern.h"

EXTERN void Trace(const wchar_t* file, int line, const wchar_t *format, ...);

#endif
	
