#include <atlbase.h>
#include <atlapp.h>
#include <atlctrls.h>

#include "StaticPrompt.h"

template< class T, class TBase, class TWinTraits >
CStaticPromptImpl<T, TBase, TWinTraits>::CStaticPromptImpl()
            : m_margin(DEFAULT_MARGIN_PIXEL)
            , m_hand(0)
{
}

// Operations
template< class T, class TBase = CStatic, class TWinTraits >
BOOL CStaticPromptImpl<T, TBase, TWinTraits>::SubclassWindow(HWND hWnd)
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
void CStaticPromptImpl::_Init()
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

            SetClassLong(*this, GCL_HCURSOR, (LONG)LoadCursor(0, IDC_HAND));
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
int CStaticPromptImpl::TotalPadding()
{
    // Let the compiler optimise this. The 0 is the widht of the text,
    // the rest is taken up by margins and the icon
    return m_margin + DEFAULT_ICON_SIZE + m_margin + 0 + m_margin;
}

//Return the static control height that can hold all the text
int CStaticPromptImpl::ComputeRequiredHeight(int margin = -1)
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

LRESULT CStaticPromptImpl::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
    _Init();
    return 0;
}
LRESULT CStaticPromptImpl::OnEraseBackground(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
    return 1; // We're painting it all
}

LRESULT CStaticPromptImpl::OnPaint(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
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
void CStaticPromptImpl::DoPaint(CDCHandle dc)
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



