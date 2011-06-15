// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

//typedef CWinTraits<WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION> CMainFrameTraits;
typedef CWinTraits<WS_POPUP> CMainFrameTraits;

class CMainFrame
    : public CFrameWindowImpl<CMainFrame, CWindow, CMainFrameTraits>
    , public CUpdateUI<CMainFrame>
    , public CMessageFilter
    , public CIdleHandler
{
    public:
        typedef CFrameWindowImpl<CMainFrame, CWindow, CMainFrameTraits> baseFrameClass ;
        DECLARE_FRAME_WND_CLASS(NULL, IDR_MAINFRAME)

        CShellieView m_view;

        virtual BOOL PreTranslateMessage(MSG* pMsg);
        virtual BOOL OnIdle();

        BEGIN_UPDATE_UI_MAP(CMainFrame)
        //UPDATE_ELEMENT(ID_VIEW_TOOLBAR, UPDUI_MENUPOPUP)
        //UPDATE_ELEMENT(ID_VIEW_STATUS_BAR, UPDUI_MENUPOPUP)
        END_UPDATE_UI_MAP()

        BEGIN_MSG_MAP(CMainFrame)
        MESSAGE_HANDLER(WM_CREATE, OnCreate)
        MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
        CHAIN_MSG_MAP(CUpdateUI<CMainFrame>)
        CHAIN_MSG_MAP(baseFrameClass)
        //CHAIN_MSG_MAP(CMainFrameBase)
        //TODO : Navigation complete handling
        END_MSG_MAP()

        // Handler prototypes (uncomment arguments if needed):
        //  LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
        //  LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
        //  LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

        LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
        LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
};
