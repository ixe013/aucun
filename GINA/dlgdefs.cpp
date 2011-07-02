#include "dlgdefs.h"
#include "loggedout_dlg.h"
#include "loggedon_dlg.h"
#include "locked_dlg.h"

DialogAndProcs gDialogsProc[] =
{
    { IDD_LOGGED_OUT_SAS,  MyWlxWkstaLoggedOutSASDlgProc, 0},
    { IDD_LOGGED_ON_SAS, MyWlxWkstaLoggedOnSASDlgProc, 0 },
    { IDD_LOCKED_SAS, MyWlxWkstaLockedSASDlgProc, 0 },
};

const int nbDialogsAndProcs = sizeof gDialogsProc / sizeof * gDialogsProc;

