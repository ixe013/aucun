#include "stdafx.h"
#include "lsahelper.h"

extern LSA_DISPATCH_TABLE *g_pSec;

LPVOID LsaAllocateLsa(ULONG size)
{
   return g_pSec->AllocateLsaHeap(size);
}

LSA_STRING *AllocateLsaStringLsa(LPCSTR szString)
{
   LSA_STRING *s;
   size_t len = strlen(szString);

   s = (LSA_STRING *) LsaAllocateLsa(sizeof(LSA_STRING));
   if (!s)
      return NULL;
   s->Buffer = (char *) LsaAllocateLsa((ULONG) len+1);
   s->Length = (USHORT)len;
   s->MaximumLength = (USHORT)len+1;
   strcpy_s(s->Buffer, len,  szString);
   return s;
}

