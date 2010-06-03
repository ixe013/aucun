// MainFrm.cpp : implmentation of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"

#include "shellieView.h"
#include "MainFrm.h"
#include "RESHelper.h"
#include "Settings.h"
#include "BrowserLockDown.h"

BOOL CMainFrame::PreTranslateMessage(MSG* pMsg)
{
	//if(CFrameWindowImpl<CMainFrame>::PreTranslateMessage(pMsg))
	if(baseFrameClass::PreTranslateMessage(pMsg))
		return TRUE;

	return m_view.PreTranslateMessage(pMsg);
}

BOOL CMainFrame::OnIdle()
{
	UIUpdateToolBar();
	return FALSE;
}

LRESULT CMainFrame::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	SetMenu(NULL);

	TCHAR resurl[MAX_PATH+5];

   if((GetSettingText(L"SOFTWARE\\Paralint.com\\Shellie\\Shell", L"URL", resurl, sizeof resurl / sizeof *resurl) != S_OK)
	   || !*resurl)
	{
		CRESHelper::GetURLForID(IDR_HTMLDEFAULT, resurl, MAX_PATH+5);
	}

	m_hWndClient = m_view.Create(m_hWnd, rcDefault, resurl, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_HSCROLL | WS_VSCROLL, 0/*WS_EX_CLIENTEDGE*/);

	//TODO: Install right click menu handler
	//TODO : Register CTRL-P hotkey, tied to printing
	
	// register object for message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->AddMessageFilter(this);
	pLoop->AddIdleHandler(this);

    CComObject<CBrowserLockDown> *pUIH = NULL;
    HRESULT hr = CComObject<CBrowserLockDown>::CreateInstance (&pUIH);
    if (SUCCEEDED(hr))
    {
        // Make our custom DocHostUIHandler the window.external handler
        CComQIPtr<IDocHostUIHandlerDispatch> pIUIH = pUIH;
        hr = m_view.SetExternalUIHandler(pIUIH) ;
    }
    ATLASSERT(SUCCEEDED(hr)) ;

	SetWindowPos(0, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), SWP_NOZORDER|SWP_SHOWWINDOW);

	return 0;
}

LRESULT CMainFrame::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	// unregister message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->RemoveMessageFilter(this);
	pLoop->RemoveIdleHandler(this);

	bHandled = FALSE;

	PostQuitMessage(0);

	return 1;
}


