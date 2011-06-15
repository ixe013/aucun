#include <windows.h>
#include "toolhelp_iterator.h"

BOOL _RefCountedPriviledgeHolder::EnablePrivilege(HANDLE hToken, LPCTSTR szPrivName, BOOL fEnable)
{
    TOKEN_PRIVILEGES tp;
    tp.PrivilegeCount = 1;
    LookupPrivilegeValue(NULL, szPrivName, &tp.Privileges[0].Luid);
    tp.Privileges[0].Attributes = fEnable ? SE_PRIVILEGE_ENABLED : 0;
    AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(tp), NULL, NULL);
    return((GetLastError() == ERROR_SUCCESS));
}

BOOL _RefCountedPriviledgeHolder::EnablePrivilegeForCurrentProcess(LPCTSTR szPrivName, BOOL fEnable)
{
    HANDLE hToken;
    BOOL result = FALSE;

    if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken))
    {
        result = EnablePrivilege(hToken, szPrivName, fEnable);
        CloseHandle(hToken);
    }

    return result;
}


_RefCountedPriviledgeHolder::_RefCountedPriviledgeHolder(LPCTSTR szPrivName)
{
    //I do not make a copy because I don't want to force a link to the
    //runtime or shlwapi. Anyway, must (all?) priviledge are constant
    //strings.
    m_Priv = szPrivName;
    AddRef();
}

_RefCountedPriviledgeHolder::~_RefCountedPriviledgeHolder()
{
    Release();
}

void _RefCountedPriviledgeHolder::AddRef()
{
    if (m_Count == 0)
    {
        EnablePrivilegeForCurrentProcess(m_Priv, TRUE);
    }

    ++m_Count;
}

void _RefCountedPriviledgeHolder::Release()
{
    if(m_Count > 0)
    {
        --m_Count;

        if (m_Count == 0)
        {
            EnablePrivilegeForCurrentProcess(m_Priv, FALSE);
        }
    }
}

BOOL CToolHelpProcess::FirstData(HANDLE h, PROCESSENTRY32* pi)
{
    BOOL result = FALSE;

    if (h != INVALID_HANDLE_VALUE)
    {
        result = Process32First(h, pi);
    }

    return result;
}

BOOL CToolHelpProcess::NextData(HANDLE h, PROCESSENTRY32* pi)
{
    BOOL result = FALSE;

    if (h != INVALID_HANDLE_VALUE)
    {
        result = Process32Next(h, pi);
    }

    return result;
}

bool CToolHelpProcess::Compare(const PROCESSENTRY32& p1, const PROCESSENTRY32& p2) const
{
    return p1.th32ProcessID == p2.th32ProcessID;
}


_RefCountedPriviledgeHolder CToolHelpModule::m_Debug(SE_DEBUG_NAME);

CToolHelpModule::CToolHelpModule(bool debugpriv)
{
    mDebugPrivEnabled = debugpriv;

    if(mDebugPrivEnabled)
    {
        m_Debug.AddRef();
    }
}

CToolHelpModule::~CToolHelpModule()
{
    if(mDebugPrivEnabled)
    {
        m_Debug.Release();
    }
}

BOOL CToolHelpModule::FirstData(HANDLE h, MODULEENTRY32* pi)
{
    BOOL result = FALSE;

    if (h != INVALID_HANDLE_VALUE)
    {
        result = Module32First(h, pi);
    }

    return result;
}

BOOL CToolHelpModule::NextData(HANDLE h, MODULEENTRY32* pi)
{
    BOOL result = FALSE;

    if (h != INVALID_HANDLE_VALUE)
    {
        result = Module32Next(h, pi);
    }

    return result;
}

bool CToolHelpModule::Compare(const MODULEENTRY32& p1, const MODULEENTRY32& p2) const
{
    return p1.th32ModuleID == p2.th32ModuleID;
}



BOOL CToolHelpThread::FirstData(HANDLE h, THREADENTRY32* pi)
{
    BOOL result = FALSE;

    if (h != INVALID_HANDLE_VALUE)
    {
        result = Thread32First(h, pi);
    }

    return result;
}

BOOL CToolHelpThread::NextData(HANDLE h, THREADENTRY32* pi)
{
    BOOL result = FALSE;

    if (h != INVALID_HANDLE_VALUE)
    {
        result = Thread32Next(h, pi);
    }

    return result;
}

bool CToolHelpThread::Compare(const THREADENTRY32& p1, const THREADENTRY32& p2) const
{
    return p1.th32ThreadID == p2.th32ThreadID;
}



const CToolHelpIterator<PROCESSENTRY32, CToolHelpProcess, TH32CS_SNAPPROCESS> CToolHelpIterator<PROCESSENTRY32, CToolHelpProcess, TH32CS_SNAPPROCESS>::End;
const CToolHelpIterator<MODULEENTRY32, CToolHelpModule, TH32CS_SNAPMODULE> CToolHelpIterator<MODULEENTRY32, CToolHelpModule, TH32CS_SNAPMODULE>::End;
const CToolHelpIterator<THREADENTRY32, CToolHelpThread, TH32CS_SNAPTHREAD> CToolHelpIterator<THREADENTRY32, CToolHelpThread, TH32CS_SNAPTHREAD>::End;

// Don't forget to remove precompiled header support for this file!
