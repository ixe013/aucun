// unlockap.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "unlockap.h"
#include "lsahelper.h"
#include "trace.h"

//Function pointers back into LSASS
PLSA_SECPKG_FUNCTION_TABLE g_pSec = 0;


NTSTATUS NTAPI LsaApCallPackage(
   PLSA_CLIENT_REQUEST ClientRequest,
   PVOID ProtocolAuthenticationInformation,
   PVOID ClientBufferBase,
   ULONG AuthenticationInformationLength,
   PVOID* ProtocolReturnBuffer,
   PULONG ReturnBufferLength,
   PNTSTATUS ProtocolStatus
)
{
   return STATUS_NOT_IMPLEMENTED;
}

NTSTATUS NTAPI LsaApCallPackageUntrusted(
   PLSA_CLIENT_REQUEST ClientRequest,
   PVOID ProtocolAuthenticationInformation,
   PVOID ClientBufferBase,
   ULONG AuthenticationInformationLength,
   PVOID* ProtocolReturnBuffer,
   PULONG ReturnBufferLength,
   PNTSTATUS ProtocolStatus
)
{
   return STATUS_NOT_IMPLEMENTED;
}

NTSTATUS NTAPI LsaApCallPackagePassthrough(
   PLSA_CLIENT_REQUEST ClientRequest,
   PVOID ProtocolAuthenticationInformation,
   PVOID ClientBufferBase,
   ULONG AuthenticationInformationLength,
   PVOID* ProtocolReturnBuffer,
   PULONG ReturnBufferLength,
   PNTSTATUS ProtocolStatus
)
{
   return STATUS_NOT_IMPLEMENTED;
}

NTSTATUS NTAPI LsaApInitializePackage(
   ULONG AuthenticationPackageId,
   PLSA_DISPATCH_TABLE LsaDispatchTable,
   PLSA_STRING Database,
   PLSA_STRING Confidentiality,
   PLSA_STRING* AuthenticationPackageName
)
{
   /* The cast isn't documented, but it's really the same structure */
   g_pSec = (PLSA_SECPKG_FUNCTION_TABLE)LsaDispatchTable;

   (*AuthenticationPackageName) = AllocateLsaStringLsa(UNLOCK_PACKAGENAME);
   return STATUS_SUCCESS;
}

NTSTATUS NTAPI LsaApLogonUser(
   PLSA_CLIENT_REQUEST ClientRequest,
   SECURITY_LOGON_TYPE LogonType,
   PVOID AuthenticationInformation,
   PVOID ClientAuthenticationBase,
   ULONG AuthenticationInformationLength,
   PVOID* ProfileBuffer,
   PULONG ProfileBufferLength,
   PLUID LogonId,
   PNTSTATUS SubStatus,
   PLSA_TOKEN_INFORMATION_TYPE TokenInformationType,
   PVOID* TokenInformation,
   PUNICODE_STRING* AccountName,
   PUNICODE_STRING* AuthenticatingAuthority
)
{
	NTSTATUS result = STATUS_LOGON_FAILURE;

	TRACE(eERROR, L"LsaApLogonUser called, logon type %d\n", LogonType);

	if(LogonType == Unlock)
	{
		result = STATUS_SUCCESS;
	}

   return result;
}

void NTAPI LsaApLogonTerminated( PLUID LogonId)
{
}
