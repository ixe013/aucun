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


#define IDD_LOGGED_OUT_SAS  1500
#define IDD_LOGGED_ON_SAS   1800
#define IDD_LOCKED_SAS      1950

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

typedef struct
{
    HANDLE CurrentUser;
    HANDLE Winlogon;
    LPARAM HookedLPARAM;
} DialogLParamHook;

EXTERN BOOL CALLBACK DelPropProc(HWND hwndSubclass, LPTSTR lpszString, HANDLE hData, ULONG_PTR x);


typedef struct
{
    const int IDD_SAS;
    const int IDC_LOCKWKSTA;
    const int IDC_LOGOFF;
    const int IDD_UNLOCKPASSWORD;
    const int IDC_USERNAME;
    const int IDC_PASSWORD;
    const int IDC_DOMAIN;
    const int IDS_CAPTION;
    const int IDS_DOMAIN_USERNAME;
    const int IDS_USERNAME;
    const int IDS_GENERIC_UNLOCK;
}
DialogAndControlsID;

static const DialogAndControlsID gDialogsAndControls[] =
{
    //Windows Server 2003
    //XP SP3 (and probably previeus versions also, never tested)
    {
        1800,    // IDD_SAS
        1800,    // IDC_LOCKWKSTA
        1801,    // IDC_LOGOFF
        1950,    // IDD_UNLOCKPASSWORD
        1953,    // IDC_USERNAME
        1954,    // IDC_PASSWORD
        1956,    // IDC_DOMAIN
        1501,    // IDS_CAPTION
        1528,    // IDS_DOMAIN_USERNAME
        1561,    // IDS_USERNAME
        1528     // IDS_GENERIC_UNLOCK //1607
    },
};

static const int nbDialogsAndControlsID = sizeof gDialogsAndControls / sizeof * gDialogsAndControls;
static int gCurrentDlgIndex = -1;

#endif
