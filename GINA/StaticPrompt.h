#ifndef __STATICPROMPT_H__
#define __STATICPROMPT_H__

#pragma once

/////////////////////////////////////////////////////////////////////////////
// StaticPrompt - Static prompt, like a fixed tooltip control implementation
//
// Based on Gradient Label by Bjarke Viksoe (bjarke@viksoe.dk)
// Transmogrified by Guillaume Seguin (guillaume@paralint.com)
//
// This code may be used in compiled form in any way you desire. This
// file may be redistributed by any means PROVIDING it is
// not sold for profit without the authors written consent, and
// providing that this notice and the authors name is included.
//
// This file is provided "as is" with no expressed or implied warranty.
// The author accepts no liability if it causes any damage to you or your
// computer whatsoever. It's free, so don't hassle me about it.
//
// Beware of bugs.
//

#ifndef __cplusplus
#error ATL requires C++ compilation (use a .cpp suffix)
#endif

#ifndef __ATLAPP_H__
#error StaticPrompt.h requires atlapp.h to be included first
#endif

#ifndef __ATLCTRLS_H__
#error StaticPrompt.h requires atlctrls.h to be included first
#endif

#if (_WIN32_IE < 0x0400)
#error StaticPrompt.h requires _WIN32_IE >= 0x0400
#endif

#define DEFAULT_MARGIN_PIXEL 5
#define DEFAULT_ICON_SIZE    32

template< class T, class TBase = CStatic, class TWinTraits = CControlWinTraits >
class ATL_NO_VTABLE CStaticPromptImpl : public CWindowImpl< T, TBase, TWinTraits >
{
    public:
        typedef CStaticPromptImpl< T, TBase, TWinTraits > thisClass;

        CFont m_font;
        int m_margin;
        HCURSOR m_hand;

        CStaticPromptImpl()
            : m_margin(DEFAULT_MARGIN_PIXEL)
            , m_hand(0)
        {
        }

        // Operations
        BOOL SubclassWindow(HWND hWnd)
        {
            ATLASSERT(m_hWnd == NULL);
            ATLASSERT(::IsWindow(hWnd));
            BOOL bRet = CWindowImpl< T, TBase, TWinTraits >::SubclassWindow(hWnd);

            if (bRet)
            {
                _Init();
            }

            return bRet;
        }

        // Implementation
        void _Init()
        {
            ATLASSERT(::IsWindow(m_hWnd));
            // Check if we should paint a label
            TCHAR lpszBuffer[10] = { 0 };

            if (::GetClassName(m_hWnd, lpszBuffer, 9))
            {
                if (::lstrcmpi(lpszBuffer, TBase::GetWndClassName()) == 0)
                {
                    ModifyStyle(0, SS_NOTIFY);  // We need this
                    DWORD dwStyle = GetStyle() & 0x000000FF;

                    if (dwStyle == SS_ICON || dwStyle == SS_BLACKRECT || dwStyle == SS_GRAYRECT ||
                            dwStyle == SS_WHITERECT || dwStyle == SS_BLACKFRAME || dwStyle == SS_GRAYFRAME ||
                            dwStyle == SS_WHITEFRAME || dwStyle == SS_OWNERDRAW ||
                            dwStyle == SS_BITMAP || dwStyle == SS_ENHMETAFILE)
                    {
                        ATLASSERT("Invalid static style for gradient label" == NULL);
                    }
                }
            }

            // Set font
            CWindow wnd = GetParent();
            CFontHandle font = wnd.GetFont();

            if( !font.IsNull() )
            {
                SetFont(font);
            }
        }

        //Returns the amount of space not covered by text, in pixels
        int TotalPadding()
        {
            // Let the compiler optimise this. The 0 is the widht of the text,
            // the rest is taken up by margins and the icon
            return m_margin + DEFAULT_ICON_SIZE + m_margin + 0 + m_margin;
        }

        //Return the static control height that can hold all the text
        int ComputeRequiredHeight(int margin = -1)
        {
            if(margin < 0)
            {
                margin = m_margin;
            }

            wchar_t prompt[2048];
            RECT rect;
            GetClientRect(&rect);
            // Let the compiler optimise this. The 0 is the widht of the text,
            // the rest is taken up by margins and the icon
            rect.left = TotalPadding();
            GetWindowText(prompt, sizeof prompt / sizeof * prompt);
            CPaintDC dc(*this);
            // Set font
            CWindow wnd = GetParent();
            CFontHandle font = wnd.GetFont();

            if( !font.IsNull())
            {
                dc.SelectFont(font);
            }

            dc.DrawText(prompt, -1, &rect, DT_CALCRECT | DT_WORDBREAK); //Compute the height
            //Room for the text and a top and bottom margin
            return max(rect.bottom - rect.top, DEFAULT_ICON_SIZE) + 2 * margin;
        }

        // Message map and handlers
        BEGIN_MSG_MAP(thisClass)
        MESSAGE_HANDLER(WM_CREATE, OnCreate)
        MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBackground)
        MESSAGE_HANDLER(WM_PAINT, OnPaint)
        MESSAGE_HANDLER(WM_PRINTCLIENT, OnPaint)
        MESSAGE_HANDLER(WM_SETCURSOR, OnSetCursor)
        END_MSG_MAP()

        LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
        {
            _Init();
            return 0;
        }
        LRESULT OnEraseBackground(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
        {
            return 1; // We're painting it all
        }

        LRESULT OnPaint(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
        {
            T* pT = static_cast<T*>(this);

            if (wParam != NULL)
            {
                pT->DoPaint((HDC) wParam);
            }
            else
            {
                CPaintDC dc(m_hWnd);
                pT->DoPaint((HDC) dc);
            }

            return 0;
        }

        // Paint methods
        void DoPaint(CDCHandle dc)
        {
            RECT rcClient;
            GetClientRect(&rcClient);
            //Tooltip color background
            dc.FillRect(&rcClient, GetSysColorBrush(COLOR_INFOBK));
            HICON question = (HICON)LoadImage(0, IDI_QUESTION, IMAGE_ICON, DEFAULT_ICON_SIZE, DEFAULT_ICON_SIZE, LR_SHARED);
            dc.DrawIconEx(m_margin, m_margin, question, DEFAULT_ICON_SIZE, DEFAULT_ICON_SIZE, DI_NORMAL);
            rcClient.left = TotalPadding() - m_margin;
            rcClient.right -= m_margin;
            rcClient.bottom -= m_margin;
            rcClient.top = m_margin;
            dc.SetBkMode(TRANSPARENT);
            dc.SetTextColor(GetSysColor(COLOR_INFOTEXT));
            HFONT hOldFont = dc.SelectFont(GetFont());
            int nLen = GetWindowTextLength();

            if (nLen > 0)
            {
                LPTSTR lpszText = (LPTSTR) _alloca((nLen + 1) * sizeof(TCHAR));

                if (GetWindowText(lpszText, nLen + 1))
                {
                    DWORD dwStyle = GetStyle();
                    int nDrawStyle = DT_LEFT;

                    if (dwStyle & SS_CENTER)
                    {
                        nDrawStyle = DT_CENTER;
                    }
                    else if (dwStyle & SS_RIGHT)
                    {
                        nDrawStyle = DT_RIGHT;
                    }

                    if (dwStyle & SS_SIMPLE)
                    {
                        nDrawStyle = DT_VCENTER | DT_SINGLELINE;
                    }

                    dc.DrawText(lpszText, -1, &rcClient, nDrawStyle | DT_WORDBREAK);
                }
            }

            GetClientRect(&rcClient);
            dc.FrameRect(&rcClient, GetSysColorBrush(COLOR_3DSHADOW));
            dc.SelectFont(hOldFont);
        }

        LRESULT OnSetCursor(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
        {
            //TODO: Set cursor code does not work, fix it or destroy it
            SetCursor(LoadCursor(0, IDC_HAND));
            bHandled = TRUE;
            return 1;
        }
};


class CStaticPromptCtrl : public CStaticPromptImpl<CStaticPromptCtrl>
{
    public:
        DECLARE_WND_CLASS(_T("WTL_StaticPrompt"))
};


#endif //__STATICPROMPT_H__

