#include <windows.h>
#include <wincrypt.h>
#include "randpasswd.h"
#include "trace.h"
#include <utility>


//Taken from http://www.microsoft.com/smallbusiness/support/articles/select_sec_passwords.mspx
static wchar_t SuggestedUnicodePasswordChars[] = 
		  L"abcdefghijklmnopqrstuvwxyz"
		  L"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		  L"0123456789"
		  //L"+-=_@#$%^&;:,.<>/~\\[](){}?!|"
;
 
static int nSuggestedUnicodePasswordChars = sizeof  SuggestedUnicodePasswordChars / sizeof *SuggestedUnicodePasswordChars;


int GenerateRandomUnicodePassword(wchar_t *buffer, size_t size)
{
   HCRYPTPROV   hCryptProv;
   int result = 0;
	--size; //Save room for the terminating character

//-------------------------------------------------------------------
// Acquire a cryptographic provider context handle.

   if (CryptAcquireContext(&hCryptProv, NULL, NULL, PROV_RSA_FULL, CRYPT_MACHINE_KEYSET|CRYPT_VERIFYCONTEXT))
   {
      if (CryptGenRandom(hCryptProv, sizeof(wchar_t)*size, (BYTE*)buffer))
      {
			for(size_t i=0; i<size; ++i)
			{
				//I reuse the buffer for two purpose : on the right hand side, buffer contains a 
				//random number. I use it as an index to the suggested char table and replace the
				//random by a random, valid password char.
				buffer[i] = SuggestedUnicodePasswordChars[buffer[i]%(nSuggestedUnicodePasswordChars-1)];
			}
         result = size;
			buffer[size] = 0;
         CryptReleaseContext(hCryptProv, 0);
      }
   }

   if (!result)
      TRACE(eERROR, L"Unable to generate a random password\n");

	return result;
} 
