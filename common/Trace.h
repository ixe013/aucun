#ifndef __TRACE_H__
#define __TRACE_H__

#define WIDEN2(x) L ## x
#define WIDEN(x) WIDEN2(x)
#define __WFILE__ WIDEN(__FILE__)

enum
{
	eNONE,
	eDEBUG,
	eINFO,
	eWARN,
	eERROR,
};

#define TRACE(l,s, ...) Trace(l, __WFILE__, __FUNCTION__, __LINE__, s, __VA_ARGS__)
#define TRACEMORE(l,s, ...) Trace(l, 0, 0, 0, s, __VA_ARGS__)

#define TRACEMSG(d) TraceMessage(__WFILE__, __FUNCTION__, __LINE__, d)


#include "extern.h"

EXTERN void Trace(int level, const wchar_t* file, const char *function, int line, const wchar_t *format, ...);
EXTERN void TraceMessage(const wchar_t* file, const char* function, int line, DWORD dw);


#endif
	
