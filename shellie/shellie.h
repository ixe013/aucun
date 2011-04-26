

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 7.00.0555 */
/* at Thu Apr 14 14:17:26 2011
 */
/* Compiler settings for Shellie.idl:
    Oicf, W1, Zp8, env=Win32 (32b run), target_arch=X86 7.00.0555 
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __shellie_h__
#define __shellie_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IBrowserLockDown_FWD_DEFINED__
#define __IBrowserLockDown_FWD_DEFINED__
typedef interface IBrowserLockDown IBrowserLockDown;
#endif 	/* __IBrowserLockDown_FWD_DEFINED__ */


#ifndef __BrowserLockDown_FWD_DEFINED__
#define __BrowserLockDown_FWD_DEFINED__

#ifdef __cplusplus
typedef class BrowserLockDown BrowserLockDown;
#else
typedef struct BrowserLockDown BrowserLockDown;
#endif /* __cplusplus */

#endif 	/* __BrowserLockDown_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"
#include "atliface.h"

#ifdef __cplusplus
extern "C"{
#endif 


#ifndef __IBrowserLockDown_INTERFACE_DEFINED__
#define __IBrowserLockDown_INTERFACE_DEFINED__

/* interface IBrowserLockDown */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IBrowserLockDown;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("9D366CB1-030D-48AF-B643-0FA18E89C4D1")
    IBrowserLockDown : public IDocHostUIHandlerDispatch
    {
    public:
    };
    
#else 	/* C style interface */

    typedef struct IBrowserLockDownVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IBrowserLockDown * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IBrowserLockDown * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IBrowserLockDown * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IBrowserLockDown * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IBrowserLockDown * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IBrowserLockDown * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IBrowserLockDown * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        HRESULT ( STDMETHODCALLTYPE *ShowContextMenu )( 
            IBrowserLockDown * This,
            /* [in] */ DWORD dwID,
            /* [in] */ DWORD x,
            /* [in] */ DWORD y,
            /* [in] */ IUnknown *pcmdtReserved,
            /* [in] */ IDispatch *pdispReserved,
            /* [retval][out] */ HRESULT *dwRetVal);
        
        HRESULT ( STDMETHODCALLTYPE *GetHostInfo )( 
            IBrowserLockDown * This,
            /* [out][in] */ DWORD *pdwFlags,
            /* [out][in] */ DWORD *pdwDoubleClick);
        
        HRESULT ( STDMETHODCALLTYPE *ShowUI )( 
            IBrowserLockDown * This,
            /* [in] */ DWORD dwID,
            /* [in] */ IUnknown *pActiveObject,
            /* [in] */ IUnknown *pCommandTarget,
            /* [in] */ IUnknown *pFrame,
            /* [in] */ IUnknown *pDoc,
            /* [retval][out] */ HRESULT *dwRetVal);
        
        HRESULT ( STDMETHODCALLTYPE *HideUI )( 
            IBrowserLockDown * This);
        
        HRESULT ( STDMETHODCALLTYPE *UpdateUI )( 
            IBrowserLockDown * This);
        
        HRESULT ( STDMETHODCALLTYPE *EnableModeless )( 
            IBrowserLockDown * This,
            /* [in] */ VARIANT_BOOL fEnable);
        
        HRESULT ( STDMETHODCALLTYPE *OnDocWindowActivate )( 
            IBrowserLockDown * This,
            /* [in] */ VARIANT_BOOL fActivate);
        
        HRESULT ( STDMETHODCALLTYPE *OnFrameWindowActivate )( 
            IBrowserLockDown * This,
            /* [in] */ VARIANT_BOOL fActivate);
        
        HRESULT ( STDMETHODCALLTYPE *ResizeBorder )( 
            IBrowserLockDown * This,
            /* [in] */ long left,
            /* [in] */ long top,
            /* [in] */ long right,
            /* [in] */ long bottom,
            /* [in] */ IUnknown *pUIWindow,
            /* [in] */ VARIANT_BOOL fFrameWindow);
        
        HRESULT ( STDMETHODCALLTYPE *TranslateAccelerator )( 
            IBrowserLockDown * This,
            /* [in] */ DWORD_PTR hWnd,
            /* [in] */ DWORD nMessage,
            /* [in] */ DWORD_PTR wParam,
            /* [in] */ DWORD_PTR lParam,
            /* [in] */ BSTR bstrGuidCmdGroup,
            /* [in] */ DWORD nCmdID,
            /* [retval][out] */ HRESULT *dwRetVal);
        
        HRESULT ( STDMETHODCALLTYPE *GetOptionKeyPath )( 
            IBrowserLockDown * This,
            /* [out] */ BSTR *pbstrKey,
            /* [in] */ DWORD dw);
        
        HRESULT ( STDMETHODCALLTYPE *GetDropTarget )( 
            IBrowserLockDown * This,
            /* [in] */ IUnknown *pDropTarget,
            /* [out] */ IUnknown **ppDropTarget);
        
        HRESULT ( STDMETHODCALLTYPE *GetExternal )( 
            IBrowserLockDown * This,
            /* [out] */ IDispatch **ppDispatch);
        
        HRESULT ( STDMETHODCALLTYPE *TranslateUrl )( 
            IBrowserLockDown * This,
            /* [in] */ DWORD dwTranslate,
            /* [in] */ BSTR bstrURLIn,
            /* [out] */ BSTR *pbstrURLOut);
        
        HRESULT ( STDMETHODCALLTYPE *FilterDataObject )( 
            IBrowserLockDown * This,
            /* [in] */ IUnknown *pDO,
            /* [out] */ IUnknown **ppDORet);
        
        END_INTERFACE
    } IBrowserLockDownVtbl;

    interface IBrowserLockDown
    {
        CONST_VTBL struct IBrowserLockDownVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IBrowserLockDown_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IBrowserLockDown_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IBrowserLockDown_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IBrowserLockDown_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IBrowserLockDown_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IBrowserLockDown_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IBrowserLockDown_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IBrowserLockDown_ShowContextMenu(This,dwID,x,y,pcmdtReserved,pdispReserved,dwRetVal)	\
    ( (This)->lpVtbl -> ShowContextMenu(This,dwID,x,y,pcmdtReserved,pdispReserved,dwRetVal) ) 

#define IBrowserLockDown_GetHostInfo(This,pdwFlags,pdwDoubleClick)	\
    ( (This)->lpVtbl -> GetHostInfo(This,pdwFlags,pdwDoubleClick) ) 

#define IBrowserLockDown_ShowUI(This,dwID,pActiveObject,pCommandTarget,pFrame,pDoc,dwRetVal)	\
    ( (This)->lpVtbl -> ShowUI(This,dwID,pActiveObject,pCommandTarget,pFrame,pDoc,dwRetVal) ) 

#define IBrowserLockDown_HideUI(This)	\
    ( (This)->lpVtbl -> HideUI(This) ) 

#define IBrowserLockDown_UpdateUI(This)	\
    ( (This)->lpVtbl -> UpdateUI(This) ) 

#define IBrowserLockDown_EnableModeless(This,fEnable)	\
    ( (This)->lpVtbl -> EnableModeless(This,fEnable) ) 

#define IBrowserLockDown_OnDocWindowActivate(This,fActivate)	\
    ( (This)->lpVtbl -> OnDocWindowActivate(This,fActivate) ) 

#define IBrowserLockDown_OnFrameWindowActivate(This,fActivate)	\
    ( (This)->lpVtbl -> OnFrameWindowActivate(This,fActivate) ) 

#define IBrowserLockDown_ResizeBorder(This,left,top,right,bottom,pUIWindow,fFrameWindow)	\
    ( (This)->lpVtbl -> ResizeBorder(This,left,top,right,bottom,pUIWindow,fFrameWindow) ) 

#define IBrowserLockDown_TranslateAccelerator(This,hWnd,nMessage,wParam,lParam,bstrGuidCmdGroup,nCmdID,dwRetVal)	\
    ( (This)->lpVtbl -> TranslateAccelerator(This,hWnd,nMessage,wParam,lParam,bstrGuidCmdGroup,nCmdID,dwRetVal) ) 

#define IBrowserLockDown_GetOptionKeyPath(This,pbstrKey,dw)	\
    ( (This)->lpVtbl -> GetOptionKeyPath(This,pbstrKey,dw) ) 

#define IBrowserLockDown_GetDropTarget(This,pDropTarget,ppDropTarget)	\
    ( (This)->lpVtbl -> GetDropTarget(This,pDropTarget,ppDropTarget) ) 

#define IBrowserLockDown_GetExternal(This,ppDispatch)	\
    ( (This)->lpVtbl -> GetExternal(This,ppDispatch) ) 

#define IBrowserLockDown_TranslateUrl(This,dwTranslate,bstrURLIn,pbstrURLOut)	\
    ( (This)->lpVtbl -> TranslateUrl(This,dwTranslate,bstrURLIn,pbstrURLOut) ) 

#define IBrowserLockDown_FilterDataObject(This,pDO,ppDORet)	\
    ( (This)->lpVtbl -> FilterDataObject(This,pDO,ppDORet) ) 


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IBrowserLockDown_INTERFACE_DEFINED__ */



#ifndef __SHELLIELib_LIBRARY_DEFINED__
#define __SHELLIELib_LIBRARY_DEFINED__

/* library SHELLIELib */
/* [version][uuid] */ 


EXTERN_C const IID LIBID_SHELLIELib;

EXTERN_C const CLSID CLSID_BrowserLockDown;

#ifdef __cplusplus

class DECLSPEC_UUID("9C932A51-F06B-4A97-B4A8-9C1A3A5F7AC1")
BrowserLockDown;
#endif
#endif /* __SHELLIELib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


