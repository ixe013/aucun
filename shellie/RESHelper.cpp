// RESHelper.cpp: implementation of the RESHelper class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "RESHelper.h"
#include <atlstr.h>

TCHAR CRESHelper::mURL[MAX_PATH + 24] = _T("res://");
size_t CRESHelper::len = 0;

void CRESHelper::Initialize(HINSTANCE dll)
{
    TCHAR filename[MAX_PATH];

    if(GetModuleFileName((HMODULE)dll, filename, MAX_PATH))
    {
        _tcscat_s(mURL, sizeof mURL / sizeof * mURL, filename);
        len = _tcslen(mURL);
    }
}

int CRESHelper::GetURLForID(int id, TCHAR* url, size_t size)
{
    TCHAR buf[32];
    buf[0] = '/';
    _itot_s(id, buf + 1, (sizeof buf / sizeof * buf) - 1, 10);
    _tcscpy_s(url, size, mURL);
    _tcscpy_s(url + len, size - len, buf);
    return len + _tcslen(buf);
}

