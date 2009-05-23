// RESHelper.h: interface for the RESHelper class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RESHELPER_H__3B9CBB5B_6C6E_421A_8CEC_67EF5C16424A__INCLUDED_)
#define AFX_RESHELPER_H__3B9CBB5B_6C6E_421A_8CEC_67EF5C16424A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CRESHelper
{
protected:
	static TCHAR mURL[MAX_PATH+24];
	static int len;

public:
	static void Initialize(HINSTANCE dll);
	static int GetURLForID(int id, TCHAR *url, size_t size);
};

#endif // !defined(AFX_RESHELPER_H__3B9CBB5B_6C6E_421A_8CEC_67EF5C16424A__INCLUDED_)
