// BrowserLockDown.h : Declaration of the CBrowserLockDown

#pragma once
#include "resource.h"       // main symbols

#include "shellie.h"
#include <atliface.h>



// CBrowserLockDown

class ATL_NO_VTABLE CBrowserLockDown :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CBrowserLockDown, &CLSID_BrowserLockDown>,
	public IDispatchImpl<IBrowserLockDown, &IID_IBrowserLockDown, &LIBID_SHELLIELib, /*wMajor =*/ 1, /*wMinor =*/ 0>
{
public:
	CBrowserLockDown()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_BROWSERLOCKDOWN)


BEGIN_COM_MAP(CBrowserLockDown)
	COM_INTERFACE_ENTRY(IBrowserLockDown)
	COM_INTERFACE_ENTRY(IDocHostUIHandlerDispatch)
	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()



	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
	}

public:
	/*
// IDocHostUIHandler
	//STDMETHOD(ShowContextMenu)(ULONG dwID, tagPOINT * ppt, IUnknown * pcmdtReserved, IDispatch * pdispReserved);
	//HRESULT STDMETHODCALLTYPE ShowContextMenu(DWORD dwID, DWORD x, DWORD y, IUnknown *pcmdtReserved, IDispatch *pdispReserved, HRESULT *dwRetVal )
	STDMETHOD(GetHostInfo)(_DOCHOSTUIINFO * pInfo);
	STDMETHOD(ShowUI)(ULONG dwID, IOleInPlaceActiveObject * pActiveObject, IOleCommandTarget * pCommandTarget, IOleInPlaceFrame * pFrame, IOleInPlaceUIWindow * pDoc);
	STDMETHOD(HideUI)();
	STDMETHOD(UpdateUI)();
	STDMETHOD(EnableModeless)(BOOL fEnable);
	STDMETHOD(OnDocWindowActivate)(BOOL fActivate);
	STDMETHOD(OnFrameWindowActivate)(BOOL fActivate);
	STDMETHOD(ResizeBorder)(const RECT * prcBorder, IOleInPlaceUIWindow * pUIWindow, BOOL fRameWindow);
	STDMETHOD(TranslateAccelerator)(MSG * lpmsg, const GUID * pguidCmdGroup, DWORD nCmdID);
	STDMETHOD(GetOptionKeyPath)(LPWSTR * pchKey, ULONG dw);
	STDMETHOD(GetDropTarget)(IDropTarget * pDropTarget, IDropTarget * * ppDropTarget);
	STDMETHOD(GetExternal)(IDispatch * * ppDispatch);
	STDMETHOD(TranslateUrl)(ULONG dwTranslate, OLECHAR * pchURLIn, OLECHAR * * ppchURLOut);
	STDMETHOD(FilterDataObject)(IDataObject * pDO, IDataObject * * ppDORet);
	/*/
	// Do not show the context menu
	STDMETHOD (ShowContextMenu) ( 
		DWORD dwID,
		DWORD x,
		DWORD y,
		IUnknown  *pcmdtReserved,
		IDispatch *pdispReserved,
		HRESULT  *dwRetVal)
	;
	// Mark all the other methods as not implemented

	STDMETHOD (GetHostInfo) ( 
		DWORD  *pdwFlags,
		DWORD  *pdwDoubleClick)
	{
		return E_NOTIMPL;
	}


	STDMETHOD (ShowUI) ( 
		DWORD dwID,
		IUnknown  *pActiveObject,
		IUnknown  *pCommandTarget,
		IUnknown  *pFrame,
		IUnknown  *pDoc,
		HRESULT  *dwRetVal)
	{
		return E_NOTIMPL;
	}


	STDMETHOD (HideUI) (void)
	{
		return E_NOTIMPL;
	}


	STDMETHOD (UpdateUI) (void)
	{
		return E_NOTIMPL;
	}

	STDMETHOD (EnableModeless) ( 
		VARIANT_BOOL fEnable)
	{
		return E_NOTIMPL;
	}


	STDMETHOD (OnDocWindowActivate) ( 
		VARIANT_BOOL fActivate)
	{
		return E_NOTIMPL;
	}


	STDMETHOD (OnFrameWindowActivate) ( 
		VARIANT_BOOL fActivate)
	{
		return E_NOTIMPL;
	}


	STDMETHOD (ResizeBorder) ( 
		long left,
		long top,
		long right,
		long bottom,
		IUnknown  *pUIWindow,
		VARIANT_BOOL fFrameWindow)
	{
		return E_NOTIMPL;
	}


	STDMETHOD (TranslateAccelerator) ( 
		DWORD hWnd,
		DWORD nMessage,
		DWORD wParam,
		DWORD lParam,
		BSTR bstrGuidCmdGroup,
		DWORD nCmdID,
		HRESULT  *dwRetVal)
	{
		return E_NOTIMPL;
	}


	STDMETHOD (GetOptionKeyPath) ( 
		BSTR  *pbstrKey,
		DWORD dw)
	{
		return E_NOTIMPL;
	}


	STDMETHOD (GetDropTarget) ( 
		IUnknown  *pDropTarget,
		IUnknown  **ppDropTarget)
	{
		return E_NOTIMPL;
	}


	STDMETHOD (GetExternal) ( 
		IDispatch  **ppDispatch)
	{
		return E_NOTIMPL;
	}


	STDMETHOD (TranslateUrl) ( 
		DWORD dwTranslate,
		BSTR bstrURLIn,
		BSTR  *pbstrURLOut)
	{
		return E_NOTIMPL;
	}


	STDMETHOD (FilterDataObject) ( 
		IUnknown  *pDO,
		IUnknown  **ppDORet)
	{
		return E_NOTIMPL;
	}
	//*/
};

//OBJECT_ENTRY_AUTO(__uuidof(BrowserLockDown), CBrowserLockDown)
