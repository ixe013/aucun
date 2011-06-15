// BrowserLockDown.cpp : Implementation of CBrowserLockDown

#include "stdafx.h"
#include "BrowserLockDown.h"


// CBrowserLockDown

STDMETHODIMP CBrowserLockDown::ShowContextMenu(DWORD dwID, DWORD x, DWORD y, IUnknown*  pcmdtReserved, IDispatch* pdispReserved, HRESULT*  dwRetVal)
{
    if(GetAsyncKeyState(VK_CONTROL) < 0)
    {
        *dwRetVal = S_FALSE;
    }
    else
    {
        *dwRetVal = S_OK;
    }

    return S_OK;
}

STDMETHODIMP CBrowserLockDown::GetHostInfo(DWORD*  pdwFlags, DWORD*  pdwDoubleClick)
{
    *pdwFlags = 0
                //| DOCHOSTUIFLAG_DIALOG
                | DOCHOSTUIFLAG_DISABLE_HELP_MENU
                //| DOCHOSTUIFLAG_NO3DBORDER
                //| DOCHOSTUIFLAG_NO3DOUTERBORDER
                //| DOCHOSTUIFLAG_SCROLL_NO
                | DOCHOSTUIFLAG_DISABLE_SCRIPT_INACTIVE
                //| DOCHOSTUIFLAG_OPENNEWWIN
                //| DOCHOSTUIFLAG_DISABLE_OFFSCREEN
                | DOCHOSTUIFLAG_FLAT_SCROLLBAR
                //| DOCHOSTUIFLAG_DIV_BLOCKDEFAULT
                | DOCHOSTUIFLAG_ACTIVATE_CLIENTHIT_ONLY
                //| DOCHOSTUIFLAG_OVERRIDEBEHAVIORFACTORY
                //| DOCHOSTUIFLAG_CODEPAGELINKEDFONTS
                //| DOCHOSTUIFLAG_URL_ENCODING_DISABLE_UTF8
                //| DOCHOSTUIFLAG_URL_ENCODING_ENABLE_UTF8
                //| DOCHOSTUIFLAG_ENABLE_FORMS_AUTOCOMPLETE
                //| DOCHOSTUIFLAG_ENABLE_INPLACE_NAVIGATION
                //| DOCHOSTUIFLAG_IME_ENABLE_RECONVERSION
                //| DOCHOSTUIFLAG_THEME
                | DOCHOSTUIFLAG_NOTHEME
                //| DOCHOSTUIFLAG_NOPICS
                ;
    *pdwDoubleClick = DOCHOSTUIDBLCLK_DEFAULT;
    return S_OK;
}

STDMETHODIMP CBrowserLockDown::ShowUI(DWORD dwID, IUnknown*  pActiveObject, IUnknown*  pCommandTarget, IUnknown*  pFrame, IUnknown*  pDoc, HRESULT*  dwRetVal)
{
    return S_OK;
}

STDMETHODIMP CBrowserLockDown::HideUI(void)
{
    return S_OK;
}

STDMETHODIMP CBrowserLockDown::UpdateUI(void)
{
    return S_OK;
}

STDMETHODIMP CBrowserLockDown::EnableModeless(VARIANT_BOOL fEnable)
{
    return S_OK;
}

STDMETHODIMP CBrowserLockDown::OnDocWindowActivate(VARIANT_BOOL fActivate)
{
    return S_OK;
}

STDMETHODIMP CBrowserLockDown::OnFrameWindowActivate(VARIANT_BOOL fActivate)
{
    return S_OK;
}

STDMETHODIMP CBrowserLockDown::ResizeBorder(long left, long top, long right, long bottom, IUnknown*  pUIWindow, VARIANT_BOOL fFrameWindow)
{
    return S_OK;
}

STDMETHODIMP CBrowserLockDown::TranslateAccelerator(DWORD hWnd, DWORD nMessage, DWORD wParam, DWORD lParam, BSTR bstrGuidCmdGroup, DWORD nCmdID, HRESULT*  dwRetVal)
{
    return E_NOTIMPL;
}

STDMETHODIMP CBrowserLockDown::GetOptionKeyPath(BSTR*  pbstrKey, DWORD dw)
{
    //if (pchKey != NULL)
    //  pchKey = 0;
    return E_NOTIMPL;
}

STDMETHODIMP CBrowserLockDown::GetDropTarget(IUnknown*  pDropTarget, IUnknown**  ppDropTarget)
{
    return E_NOTIMPL;
}

STDMETHODIMP CBrowserLockDown::GetExternal(IDispatch**  ppDispatch)
{
    if(ppDispatch)
    {
        *ppDispatch = 0;
    }

    return E_NOTIMPL;
}

STDMETHODIMP CBrowserLockDown::TranslateUrl(DWORD dwTranslate, BSTR bstrURLIn, BSTR*  pbstrURLOut)
{
    return S_FALSE;
}

STDMETHODIMP CBrowserLockDown::FilterDataObject(IUnknown*  pDO, IUnknown**  ppDORet)
{
    return S_FALSE;
}

