#ifndef __DLGDEFS_H__
#define __DLGDEFS_H__

#include <windows.h>
#include "extern.h"
#include "resource.h"

typedef struct
{
	int IDD;
	DLGPROC dlgproc;
	DLGPROC originalproc;
} DialogAndProcs;


#define IDD_LOGGED_OUT_SAS	1500
#define IDD_LOGGED_ON_SAS	1800
#define IDD_LOCKED_SAS		1950

// The three dialogs we hook
// MUST BE IN SYNC WITH DialogAndProcsBuffer
enum 
{
	LOGGED_OUT_SAS_dlg,
	LOGGED_ON_SAS_dlg,
	LOCKED_SAS_dlg,
	NUMBER_OF_DIALOGS //Always keep last
};


EXTERN DialogAndProcs gDialogsProc[];
EXTERN const int nbDialogsAndProcs;

#endif
