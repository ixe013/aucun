#ifndef __DLGDEFS_H__
#define __DLGDEFS_H__

#include <windows.h>
#include "extern.h"

typedef struct
{
	int IDD;
	DLGPROC dlgproc;
} DialogAndProcs;


#define IDD_LOGGED_OUT_SAS	1500
#define IDD_LOGGED_ON_SAS	1800
#define IDD_LOCKED_SAS		1950

// The three dialogs we hook
enum 
{
	LOGGED_OUT_SAS_dlg,
	LOGGED_ON_SAS_dlg,
	LOCKED_SAS_dlg
};


EXTERN DialogAndProcs gDialogsProc[];

#endif