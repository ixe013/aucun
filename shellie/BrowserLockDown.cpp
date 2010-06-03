// BrowserLockDown.cpp : Implementation of CBrowserLockDown

#include "stdafx.h"
#include "BrowserLockDown.h"


// CBrowserLockDown

STDMETHODIMP CBrowserLockDown::ShowContextMenu(ULONG dwID, tagPOINT * ppt, IUnknown * pcmdtReserved, IDispatch * pdispReserved)
{
	if(GetAsyncKeyState(VK_CONTROL) < 0)
		return S_FALSE;

	return S_OK;
}

STDMETHODIMP CBrowserLockDown::GetHostInfo(_DOCHOSTUIINFO * pInfo)
{
	if (pInfo == NULL)
		return E_POINTER;

	pInfo->cbSize = sizeof _DOCHOSTUIINFO;
	pInfo->dwFlags = 0
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

	pInfo->dwDoubleClick = DOCHOSTUIDBLCLK_DEFAULT;
		
	return S_OK;
}

STDMETHODIMP CBrowserLockDown::ShowUI(ULONG dwID, IOleInPlaceActiveObject * pActiveObject, IOleCommandTarget * pCommandTarget, IOleInPlaceFrame * pFrame, IOleInPlaceUIWindow * pDoc)
{
	return S_OK;
}

STDMETHODIMP CBrowserLockDown::HideUI()
{
	return S_OK;
}

STDMETHODIMP CBrowserLockDown::UpdateUI()
{
	return S_OK;
}

STDMETHODIMP CBrowserLockDown::EnableModeless(BOOL fEnable)
{
	return S_OK;
}

STDMETHODIMP CBrowserLockDown::OnDocWindowActivate(BOOL fActivate)
{
	return S_OK;
}

STDMETHODIMP CBrowserLockDown::OnFrameWindowActivate(BOOL fActivate)
{
	return S_OK;
}

STDMETHODIMP CBrowserLockDown::ResizeBorder(const RECT * prcBorder, IOleInPlaceUIWindow * pUIWindow, BOOL fRameWindow)
{
	return S_OK;
}

STDMETHODIMP CBrowserLockDown::TranslateAccelerator(MSG * lpmsg, const GUID * pguidCmdGroup, DWORD nCmdID)
{
	return E_NOTIMPL;
}

STDMETHODIMP CBrowserLockDown::GetOptionKeyPath(LPWSTR * pchKey, ULONG dw)
{
	if (pchKey != NULL)
		pchKey = 0;
		
	return S_FALSE;
}

STDMETHODIMP CBrowserLockDown::GetDropTarget(IDropTarget * pDropTarget, IDropTarget * * ppDropTarget)
{
	return E_NOTIMPL;
}

STDMETHODIMP CBrowserLockDown::GetExternal(IDispatch * * ppDispatch)
{
	if(ppDispatch)
		*ppDispatch = 0;

	return E_NOTIMPL;
}

STDMETHODIMP CBrowserLockDown::TranslateUrl(ULONG dwTranslate, OLECHAR * pchURLIn, OLECHAR ** ppchURLOut)
{
	return S_FALSE;
}

STDMETHODIMP CBrowserLockDown::FilterDataObject(IDataObject * pDO, IDataObject * * ppDORet)
{
	return S_FALSE;
}
