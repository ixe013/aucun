#include "stdafx.h"
#include "lsahelper.h"

extern PLSA_SECPKG_FUNCTION_TABLE g_pSec;

LPVOID LsaAllocateLsa(ULONG size)
{
	LPVOID result = 0;

	if(g_pSec)
   		result = g_pSec->AllocateLsaHeap(size);

	return result;
}

LSA_STRING *AllocateLsaStringLsa(LPCSTR szString)
{
   LSA_STRING *s = 0;
   size_t len = strlen(szString);

   s = (LSA_STRING *) LsaAllocateLsa(sizeof(LSA_STRING));

   if (s)
	{
   s->Buffer = (char *) LsaAllocateLsa((ULONG) len+1);
   s->Length = (USHORT)len;
   s->MaximumLength = (USHORT)len+1;
   strcpy_s(s->Buffer, len+1,  szString);
}
   return s;
}

