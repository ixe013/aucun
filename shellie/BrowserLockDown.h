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
	// Do not show the context menu
	STDMETHOD(ShowContextMenu)(DWORD dwID, DWORD x, DWORD y, IUnknown  *pcmdtReserved, IDispatch *pdispReserved, HRESULT  *dwRetVal);
	STDMETHOD(GetHostInfo)(DWORD  *pdwFlags, DWORD  *pdwDoubleClick);
	STDMETHOD(ShowUI)(DWORD dwID, IUnknown  *pActiveObject, IUnknown  *pCommandTarget, IUnknown  *pFrame, IUnknown  *pDoc, HRESULT  *dwRetVal);
	STDMETHOD(HideUI)(void);
	STDMETHOD(UpdateUI)(void);
	STDMETHOD(EnableModeless)(VARIANT_BOOL fEnable);
	STDMETHOD(OnDocWindowActivate)(VARIANT_BOOL fActivate);
	STDMETHOD(OnFrameWindowActivate)(VARIANT_BOOL fActivate);
	STDMETHOD(ResizeBorder)(long left, long top, long right, long bottom, IUnknown  *pUIWindow, VARIANT_BOOL fFrameWindow);
	STDMETHOD(TranslateAccelerator)(DWORD hWnd, DWORD nMessage, DWORD wParam, DWORD lParam, BSTR bstrGuidCmdGroup, DWORD nCmdID, HRESULT  *dwRetVal);
	STDMETHOD(GetOptionKeyPath)(BSTR  *pbstrKey, DWORD dw);
	STDMETHOD(GetDropTarget)(IUnknown  *pDropTarget, IUnknown  **ppDropTarget);
	STDMETHOD(GetExternal)(IDispatch  **ppDispatch);
	STDMETHOD(TranslateUrl)(DWORD dwTranslate, BSTR bstrURLIn, BSTR  *pbstrURLOut);
	STDMETHOD(FilterDataObject)(IUnknown  *pDO, IUnknown  **ppDORet);
};

//OBJECT_ENTRY_AUTO(__uuidof(BrowserLockDown), CBrowserLockDown)
