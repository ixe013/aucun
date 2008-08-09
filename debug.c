#ifdef _DEBUG
#include <windows.h>
#include "debug.h"

typedef struct tagMessageList
{
	LPCWSTR pMessageID;		// Text based ID
	UINT dMessageNo;		// Numeric ID
	BOOL bFilter;			// Filter: 0=Show message, 1=Do not show
	LPCWSTR pMessageDesc;		// Text based description
	BYTE dWParam;			// wParam Signature
	BYTE dLParam;			// lParam Signature
	BYTE dLResult;			// lResult Signature
} MessageList;

static const MessageList g_mlTable[] = 
{	
	{ L"WM_NULL",			0,		0,	L"No operation", 0, 0, 0 },
	{ L"WM_CREATE",			1,		0,	L"Window created and about to be displayed", 0, 0, 0 },
	{ L"WM_DESTROY",			2,		0,	L"Window is being destroy and has been removed from the screen", 0, 0, 0 },
	{ L"WM_MOVE",			3,		0,	L"Window has been moved", 0, 0, 0 },
	{ L"WM_SIZE",			5,		0,	L"Window size has been changed", 0, 0, 0 },
	{ L"WM_ACTIVATE",		6,		0,	L"Window is either being activated for deactivated", 0, 0, 0 },
	{ L"WM_SETFOCUS",		7,		0,	L"Window has keyboard focus", 0, 0, 0 },
	{ L"WM_KILLFOCUS",		8,		0,	L"Window is about to lose focus", 0, 0, 0 },
	{ L"WM_ENABLE",			10,		0,	L"Window enabled state is changing", 0, 0, 0 },
	{ L"WM_SETREDRAW",		11,		0,	L"Allow or prevent redraws to take place", 0, 0, 0 },
	{ L"WM_SETTEXT",			12,		0,	L"Set the text of the window", 0, 0, 0 },
	{ L"WM_GETTEXT",			13,		0,	L"Retrieve corresponding window text", 0, 0, 0 },
	{ L"WM_GETTEXTLENGTH",	14,		0,	L"Return length, in characters, of associated text", 0, 0, 0 },
	{ L"WM_PAINT",			15,		0,	L"Paint a portion of the client area", 0, 0, 0 },
	{ L"WM_CLOSE",			16,		0,	L"Window or application should terminate", 0, 0, 0 },
	{ L"WM_QUERYENDSESSION",	17,		0,	L"User chose to end current Windows session", 0, 0, 0 },
	{ L"WM_QUIT",			18,		0,	L"Terminate application request", 0, 0, 0 },
	{ L"WM_QUERYOPEN",		19,		0,	L"User requested that iconic window be restored", 0, 0, 0 },
	{ L"WM_ERASEBKGND",		20,		0,	L"Window background must be erased", 0, 0, 0 },
	{ L"WM_SYSCOLORCHANGE",	21,		0,	L"A change has been made to a system color setting", 0, 0, 0 },
	{ L"WM_ENDSESSION",		22,		0,	L"Windows session might be ending", 0, 0, 0 },
	{ L"WM_SHOWWINDOW",		24,		0,	L"Window is about to be hidden or shown", 0, 0, 0 },
	{ L"WM_SETTINGCHANGE",	26,		0,	L"A system wide setting has changed", 0, 0, 0 },
	{ L"WM_WININICHANGE",	26,		0,	L"A system wide setting has changed", 0, 0, 0 },
	{ L"WM_DEVMODECHANGE",	27,		0,	L"User changed device-mode settings", 0, 0, 0 },
	{ L"WM_ACTIVATEAPP",		28,		0,	L"A window belonging to another application is about to be activated", 0, 0, 0 },
	{ L"WM_FONTCHANGE",		29,		0,	L"Pool of font resources has changed", 0, 0, 0 },
	{ L"WM_TIMECHANGE",		30,		0,	L"System time has changed", 0, 0, 0 },
	{ L"WM_CANCELMODE",		31,		0,	L"Window is to cancel its current mode", 0, 0, 0 },
	{ L"WM_SETCURSOR",		32,		0,	L"Cursor moved within window and is not captured", 0, 0, 0 },
	{ L"WM_MOUSEACTIVATE",	33,		0,	L"Window activation via mouse click", 0, 0, 0 },
	{ L"WM_CHILDACTIVATE",	34,		0,	L"MDI child window has been activated, moved, or sized", 0, 0, 0 },
	{ L"WM_QUEUESYNC",		35,		0,	L"Separate user-input messages from other journal playback hook messages", 0, 0, 0 },
	{ L"WM_GETMINMAXINFO",	36,		0,	L"Return minimum or maximum tracking size", 0, 0, 0 },
	{ L"WM_PAINTICON",		38,		0,	L"Icon is to be painted of minimized window", 0, 0, 0 },
	{ L"WM_ICONERASEBKGND",	39,		0,	L"Background of the minimized window icon must be painted", 0, 0, 0 },
	{ L"WM_NEXTDLGCTL",		40,		0,	L"Set the keyboard focus to a different control", 0, 0, 0 },
	{ L"WM_SPOOLERSTATUS",	42,		0,	L"Job as been added or removed from the Print Manager queue", 0, 0, 0 },
	{ L"WM_DRAWITEM",		43,		0,	L"Visual aspect of control has changed", 0, 0, 0 },
	{ L"WM_MEASUREITEM",		44,		0,	L"Provide measurements of owner drawn control", 0, 0, 0 },
	{ L"WM_DELETEITEM",		45,		0,	L"Items removed from the list/combo box", 0, 0, 0 },
	{ L"WM_VKEYTOITEM",		46,		0,	L"List box want keyboard input notification", 0, 0, 0 },
	{ L"WM_CHARTOITEM",		47,		0,	L"List box with keyboard input enabled received a character message", 0, 0, 0 },
	{ L"WM_SETFONT",			48,		0,	L"Set desired font window is to use while drawing text", 0, 0, 0 },
	{ L"WM_GETFONT",			49,		0,	L"Return the font with which the window is currently drawing its text", 0, 0, 0 },
	{ L"WM_SETHOTKEY",		50,		0,	L"Associate hot key", 0, 0, 0 },
	{ L"WM_GETHOTKEY",		51,		0,	L"Hot key association query", 0, 0, 0 },
	{ L"WM_QUERYDRAGICON",	55,		0,	L"Minimized iconic window is about to be dragged by user", 0, 0, 0 },
	{ L"WM_COMPAREITEM",		57,		0,	L"Report the relative item position in a sorted list of an owner-draw combo or list box", 0, 0, 0 },
	{ L"WM_COMPACTING",		65,		0,	L"System memory is low", 0, 0, 0 },
	{ L"WM_WINDOWPOSCHANGING",70,	0,	L"Size, position, or Z order is about to change", 0, 0, 0 },
	{ L"WM_WINDOWPOSCHANGED",71,		0,	L"Size, position, or Z order has changed", 0, 0, 0 },
	{ L"WM_POWER",			72,		0,	L"System is about to enter suspended mode", 0, 0, 0 },
	{ L"WM_COPYDATA",		74,		0,	L"Data has be passed to application ", 0, 0, 0 },
	{ L"WM_CANCELJOURNAL",	75,		0,	L"User cancelled journaling activities", 0, 0, 0 },
	{ L"WM_NOTIFY",			78,		0,	L"Control event occurred or requires information", 0, 0, 0 },
	{ L"WM_INPUTLANGCHANGEREQUEST",80,0,	L"User requests to change the current input language", 0, 0, 0 },
	{ L"WM_INPUTLANGCHANGE",	81,		0,	L"Input language has changed", 0, 0, 0 },
	{ L"WM_TCARD",			82,		0,	L"User clicked an authorable button on a WinHelp Training Card", 0, 0, 0 },
	{ L"WM_HELP",			83,		0,	L"User pressed F1", 0, 0, 0 },
	{ L"WM_USERCHANGED",		84,		0,	L"User has logged on or off", 0, 0, 0 },
	{ L"WM_NOTIFYFORMAT",	85,		0,	L"Return whether should use ANSI or Unicode structures", 0, 0, 0 },
	{ L"WM_CONTEXTMENU",		123,	0,	L"User clicked the right mouse button in the window", 0, 0, 0 },
	{ L"WM_STYLECHANGING",	124,	0,	L"One or more of the window's styles is about to change", 0, 0, 0 },
	{ L"WM_STYLECHANGED",	125,	0,	L"Window's style has changed", 0, 0, 0 },
	{ L"WM_DISPLAYCHANGE",	126,	0,	L"Display resolution has changed", 0, 0, 0 },
	{ L"WM_GETICON",			127,	0,	L"Return the handle of the large or small associated icon", 0, 0, 0 },
	{ L"WM_SETICON",			128,	0,	L"Set large or small icon", 0, 0, 0 },
	{ L"WM_NCCREATE",		129,	0,	L"Non-client area created and is about to be displayed", 0, 0, 0 },
	{ L"WM_NCDESTROY",		130,	0,	L"Non-client area is being destroyed", 0, 0, 0 },
	{ L"WM_NCCALCSIZE",		131,	0,	L"Size and position of client area must be calculated", 0, 0, 0 },
	{ L"WM_NCHITTEST",		132,	0,	L"Mouse cursor moved or button was pressed or released", 0, 0, 0 },
	{ L"WM_NCPAINT",			133,	0,	L"Frame must be painted", 0, 0, 0 },
	{ L"WM_NCACTIVATE",		134,	0,	L"Nonclient area needs to be changed to reflect active state", 0, 0, 0 },
	{ L"WM_GETDLGCODE",		135,	0,	L"Chance to override default behavior to dialog box input", 0, 0, 0 },
	{ L"WM_NCMOUSEMOVE",		160,	0,	L"User moved mouse within non-client area", 0, 0, 0 },
	{ L"WM_NCLBUTTONDOWN",	161,	0,	L"User pressed left mouse button in non-client area", 0, 0, 0 },
	{ L"WM_NCLBUTTONUP",		162,	0,	L"User released left mouse button in non-client area", 0, 0, 0 },
	{ L"WM_NCLBUTTONDBLCLK",	163,	0,	L"User double clicked left mouse button in non-client area", 0, 0, 0 },
	{ L"WM_NCRBUTTONDOWN",	164,	0,	L"User pressed right mouse button in non-client area", 0, 0, 0 },
	{ L"WM_NCRBUTTONUP",		165,	0,	L"User released right mouse button in non-client area", 0, 0, 0 },
	{ L"WM_NCRBUTTONDBLCLK",	166,	0,	L"User double clicked right mouse button in non-client area", 0, 0, 0 },
	{ L"WM_NCMBUTTONDOWN",	167,	0,	L"User pressed middle mouse button in non-client area", 0, 0, 0 },
	{ L"WM_NCMBUTTONUP",		168,	0,	L"User released middle mouse button in non-client area", 0, 0, 0 },
	{ L"WM_NCMBUTTONDBLCLK",	169,	0,	L"User double clicked middle mouse button in non-client area", 0, 0, 0 },
	{ L"WM_KEYDOWN",			256,	0,	L"Non-system key pressed", 0, 0, 0 },
	{ L"WM_KEYUP",			257,	0,	L"Non-system key released", 0, 0, 0 },
	{ L"WM_CHAR",			258,	0,	L"Key down message has been translated", 0, 0, 0 },
	{ L"WM_DEADCHAR",		259,	0,	L"Dead character code translated", 0, 0, 0 },
	{ L"WM_SYSKEYDOWN",		260,	0,	L"System key has been pressed (F10 or ALT-key)", 0, 0, 0 },
	{ L"WM_SYSKEYUP",		261,	0,	L"System key has been released (F10 or ALT-key)", 0, 0, 0 },
	{ L"WM_SYSCHAR",			262,	0,	L"System ALT key has been translated", 0, 0, 0 },
	{ L"WM_SYSDEADCHAR",		263,	0,	L"Dead system character code translated", 0, 0, 0 },
	{ L"WM_IME_STARTCOMPOSITION",269,0,	L"IME is about to generate a composition string due to a keystroke", 0, 0, 0 },
	{ L"WM_IME_ENDCOMPOSITION",270,	0,	L"IME ended composition", 0, 0, 0 },
	{ L"WM_IME_COMPOSITION",	271,	0,	L"IME changed composition status as a result of a key stroke", 0, 0, 0 },
	{ L"WM_INITDIALOG",		272,	0,	L"Dialog box is about to be displayed", 0, 0, 0 },
	{ L"WM_COMMAND",			273,	0,	L"Window received a command notification", 0, 0, 0 },
	{ L"WM_SYSCOMMAND",		274,	0,	L"User chose a command from the system menu", 0, 0, 0 },
	{ L"WM_TIMER",			275,	0,	L"Timer expired", 0, 0, 0 },
	{ L"WM_HSCROLL",			276,	0,	L"Scroll event occurred in the horizontal scroll bar", 0, 0, 0 },
	{ L"WM_VSCROLL",			277,	0,	L"Scroll event occurred in the vertical scroll bar", 0, 0, 0 },
	{ L"WM_INITMENU",		278,	0,	L"Menu is about to become active", 0, 0, 0 },
	{ L"WM_INITMENUPOPUP",	279,	0,	L"Drop-down or sub-menu is about to become active", 0, 0, 0 },
	{ L"WM_MENUSELECT",		287,	0,	L"Menu item has been selected", 0, 0, 0 },
	{ L"WM_MENUCHAR",		288,	0,	L"User pressed an unknown menu key", 0, 0, 0 },
	{ L"WM_ENTERIDLE",		289,	0,	L"Child modal dialog or menu is entering an idle state", 0, 0, 0 },
	{ L"WM_CTLCOLORMSGBOX",	306,	0,	L"Message box is about to be drawn", 0, 0, 0 },
	{ L"WM_CTLCOLOREDIT",	307,	0,	L"Edit box is about to be drawn", 0, 0, 0 },
	{ L"WM_CTLCOLORLISTBOX",	308,	0,	L"List box is about to be drawn", 0, 0, 0 },
	{ L"WM_CTLCOLORBTN",		309,	0,	L"Button is about to be drawn", 0, 0, 0 },
	{ L"WM_CTLCOLORDLG",		310,	0,	L"Dialog box is about to be drawn", 0, 0, 0 },
	{ L"WM_CTLCOLORSCROLLBAR",311,	0,	L"Scroll bar is about to be drawn", 0, 0, 0 },
	{ L"WM_CTLCOLORSTATIC",	312,	0,	L"Static control is about to be drawn", 0, 0, 0 },
	{ L"WM_MOUSEMOVE",		512,	0,	L"Mouse cursor moved", 0, 0, 0 },
	{ L"WM_LBUTTONDOWN",		513,	0,	L"User pressed left mouse button in client area", 0, 0, 0 },
	{ L"WM_LBUTTONUP",		514,	0,	L"User released left mouse button in client area", 0, 0, 0 },
	{ L"WM_LBUTTONDBLCLK",	515,	0,	L"User double clicked left mouse button in client area", 0, 0, 0 },
	{ L"WM_RBUTTONDOWN",		516,	0,	L"User pressed right mouse button in client area", 0, 0, 0 },
	{ L"WM_RBUTTONUP",		517,	0,	L"User released right mouse button in client area", 0, 0, 0 },
	{ L"WM_RBUTTONDBLCLK",	518,	0,	L"User double clicked right mouse button in client area", 0, 0, 0 },
	{ L"WM_MBUTTONDOWN",		519,	0,	L"User pressed middle mouse button in client area", 0, 0, 0 },
	{ L"WM_MBUTTONUP",		520,	0,	L"User released middle mouse button in client area", 0, 0, 0 },
	{ L"WM_MBUTTONDBLCLK",	521,	0,	L"User double clicked middle mouse button in client area", 0, 0, 0 },
	{ L"WM_MOUSEWHEEL",		522,	0,	L"Mouse wheel rotated", 0, 0, 0 },
	{ L"WM_PARENTNOTIFY",	528,	0,	L"Child window created or destroyed, or mouse click event occurred to child", 0, 0, 0 },
	{ L"WM_ENTERMENULOOP",	529,	0,	L"Menu modal loop has been entered", 0, 0, 0 },
	{ L"WM_EXITMENULOOP",	530,	0,	L"Menu modal loop has been exited", 0, 0, 0 },
	{ L"WM_NEXTMENU",		531,	0,	L"Right or left arrow key was used to switch between menu bar and system menu", 0, 0, 0 },
	{ L"WM_SIZING",			532,	0,	L"Window is resizing", 0, 0, 0 },
	{ L"WM_CAPTURECHANGED",	533,	0,	L"Window is losing mouse capture", 0, 0, 0 },
	{ L"WM_MOVING",			534,	0,	L"Window is moving", 0, 0, 0 },
	{ L"WM_POWERBROADCAST",	536,	0,	L"Power-management event notification", 0, 0, 0 },
	{ L"WM_DEVICECHANGE",	537,	0,	L"Hardware configuration has changed", 0, 0, 0 },
	{ L"WM_MDICREATE",		544,	0,	L"Create a MDI child window", 0, 0, 0 },
	{ L"WM_MDIDESTROY",		545,	0,	L"Destroy MDI child window", 0, 0, 0 },
	{ L"WM_MDIACTIVATE",		546,	0,	L"Activate a different MDI child window", 0, 0, 0 },
	{ L"WM_MDIRESTORE",		547,	0,	L"Restore MDI child window size", 0, 0, 0 },
	{ L"WM_MDINEXT",			548,	0,	L"Activate the next or previous MDI child window", 0, 0, 0 },
	{ L"WM_MDIMAXIMIZE",		549,	0,	L"Maximize a MDI child window", 0, 0, 0 },
	{ L"WM_MDITILE",			550,	0,	L"Arrange all MDI children in a tile format", 0, 0, 0 },
	{ L"WM_MDICASCADE",		551,	0,	L"Arrange all MDI child windows in cascade format", 0, 0, 0 },
	{ L"WM_MDIICONARRANGE",	552,	0,	L"Arrange all minimized MDI child windows", 0, 0, 0 },
	{ L"WM_MDIGETACTIVE",	553,	0,	L"Return handle of active MDI child window", 0, 0, 0 },
	{ L"WM_MDISETMENU",		560,	0,	L"Replace entire menu of the MDI frame", 0, 0, 0 },
	{ L"WM_ENTERSIZEMOVE",	561,	0,	L"Entered moving or sizing modal loop", 0, 0, 0 },
	{ L"WM_EXITSIZEMOVE",	562,	0,	L"Window exited the moving or sizing modal loop", 0, 0, 0 },
	{ L"WM_DROPFILES",		563,	0,	L"Dropped file notification", 0, 0, 0 },
	{ L"WM_MDIREFRESHMENU",	564,	0,	L"Refresh the MDI menu of the frame", 0, 0, 0 },
	{ L"WM_IME_SETCONTEXT",	641,	0,	L"IME window is about to be activated", 0, 0, 0 },
	{ L"WM_IME_NOTIFY",		642,	0,	L"IME window change notification", 0, 0, 0 },
	{ L"WM_IME_CONTROL",		643,	0,	L"IME window must carry out requested command", 0, 0, 0 },
	{ L"WM_IME_COMPOSITIONFULL",644,	0,	L"IME cannot extend the area of the composition window", 0, 0, 0 },
	{ L"WM_IME_SELECT",		645,	0,	L"System is about to change the current IME", 0, 0, 0 },
	{ L"WM_IME_CHAR",		646,	0,	L"IME got a character of the conversion result", 0, 0, 0 },
	{ L"WM_IME_KEYDOWN",		656,	0,	L"IME key press notification", 0, 0, 0 },
	{ L"WM_IME_KEYUP",		657,	0,	L"IME key release notification", 0, 0, 0 },
	{ L"WM_MOUSEHOVER",		673,	0,	L"Mouse cursor is hovering", 0, 0, 0 },
	{ L"WM_MOUSELEAVE",		675,	0,	L"Mouse cursor moved out of window", 0, 0 ,0 },
	{ L"WM_CUT",				768,	0,	L"Delete current selection", 0, 0, 0 },
	{ L"WM_COPY",			769,	0,	L"Copy current selection into the clipboard in text format", 0, 0, 0 },
	{ L"WM_PASTE",			770,	0,	L"Copy current content of the clipboard to control", 0, 0, 0 },
	{ L"WM_CLEAR",			771,	0,	L"Clear the current selection, if any, from the edit control", 0, 0, 0 },
	{ L"WM_UNDO",			772,	0,	L"Undo last operation", 0, 0, 0 },
	{ L"WM_RENDERFORMAT",	773,	0,	L"Render data in the specified clipboard format", 0, 0, 0 },
	{ L"WM_RENDERALLFORMATS",774,	0,	L"Render data in all clipboard formats", 0, 0, 0 },
	{ L"WM_DESTROYCLIPBOARD",775,	0,	L"Clipboard has been emptied", 0, 0, 0 },
	{ L"WM_DRAWCLIPBOARD",	776,	0,	L"Content of the clipboard changed", 0, 0, 0 },
	{ L"WM_PAINTCLIPBOARD",	777,	0,	L"Clipboard viewer's client area needs repainting", 0, 0, 0 },
	{ L"WM_VSCROLLCLIPBOARD",778,	0,	L"Event occurred in the clipboard viewer's vertical scroll bar", 0, 0, 0 },
	{ L"WM_SIZECLIPBOARD",	779,	0,	L"Clipboard viewer's client area has changed size", 0, 0, 0 },
	{ L"WM_ASKCBFORMATNAME",	780,	0,	L"Request of an owner displayed clipboard format", 0, 0, 0 },
	{ L"WM_CHANGECBCHAIN",	781,	0,	L"A window is being removed from the clipboard viewer chain", 0, 0, 0 },
	{ L"WM_HSCROLLCLIPBOARD",782,	0,	L"Event occurred in the clipboard viewer's horizontal scroll bar", 0, 0, 0 },
	{ L"WM_QUERYNEWPALETTE",	783,	0,	L"Window may realize its logical palette what focus is received", 0, 0, 0 },
	{ L"WM_PALETTEISCHANGING",784,	0,	L"A window is about to realize its logical palette", 0, 0, 0 },
	{ L"WM_PALETTECHANGED",	785,	0,	L"A window has realized its logical palette", 0, 0, 0 },
	{ L"WM_HOTKEY",			786,	0,	L"User pressed registered hotkey", 0, 0, 0 },
	{ L"WM_PRINT",			791,	0,	L"Render image in current device context request", 0, 0, 0 },
	{ L"WM_PRINTCLIENT",		792,	0,	L"Render client area in current device context request", 0, 0, 0 },
	{ L"WM_DDE_INITIATE",	992,	0,	L"Initiate a conversation with a DDE client", 0, 0, 0 },
	{ L"WM_DDE_TERMINATE",	993,	0,	L"Terminate a DDE conversation", 0, 0, 0 },
	{ L"WM_DDE_ADVISE",		994,	0,	L"DDE server data item update notification request", 0, 0, 0 },
	{ L"WM_DDE_UNADVISE",	995,	0,	L"Data or clipboard format client item should no longer be updated", 0, 0, 0 },
	{ L"WM_DDE_ACK",			996,	0,	L"DDE message has been received and processed", 0, 0, 0 },
	{ L"WM_DDE_DATA",		997,	0,	L"Data item received or data item available", 0, 0, 0 },
	{ L"WM_DDE_REQUEST",		998,	0,	L"Client requests the value of a data item", 0, 0, 0 },
	{ L"WM_DDE_POKE",		999,	0,	L"Client requests unsolicited data be accepted", 0, 0, 0 },
	{ L"WM_DDE_EXECUTE",		1000,	0,	L"Process a string as a series of commands", 0, 0, 0 },
	{ L"Unknown",			-1,		0,	L"Unknown message", 0, 0, 0 } 
};

static const int nbMessages = sizeof g_mlTable / sizeof *g_mlTable;

LPCWSTR GetWindowsMessageName(UINT msg)
{
	LPCWSTR result = g_mlTable[nbMessages-1].pMessageID;

	int i;
	for(i=0; i<nbMessages; ++i)
	{
		if(g_mlTable[i].dMessageNo == msg)
		{
			result = g_mlTable[i].pMessageID;
		}
	}

	return result;
}


BOOLEAN GetLUIDFromToken(HANDLE token, PLUID session)
{
	BOOLEAN result = FALSE;
	TOKEN_STATISTICS stats = {0};
	DWORD nbstats = sizeof stats;
	GetTokenInformation(token, TokenStatistics, &stats, sizeof stats, &nbstats); 

	*session = stats.AuthenticationId;

	return result;
}

void OutputGetSessionUserName(PLUID session)
{
	PSECURITY_LOGON_SESSION_DATA sessionData = NULL;
	NTSTATUS retval;

	// Check for a valid session.
	if (!session ) {
		OutputDebugString(L"Error - Invalid logon session identifier.\n");
		return;
	}
	// Get the session information.
	retval = LsaGetLogonSessionData (session, &sessionData);
	if(!retval) 
	{
		// Determine whether there is session data to parse. 
		if(!sessionData) 
		{ // no data for session
			OutputDebugString(L"Invalid logon session data. \n");
		}
		if (sessionData->UserName.Buffer != NULL) 
		{
			OutputDebugString(L"User from LUID is ");
			OutputDebugString((sessionData->UserName).Buffer);
			OutputDebugString(L"\n");
		}
	// Free the memory returned by the LSA.
	}
	if (sessionData) 
			LsaFreeReturnBuffer(sessionData);
	return;
}


#endif

