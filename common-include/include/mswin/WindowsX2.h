#ifndef __WindowsX2_h_20250326_
#define __WindowsX2_h_20250326_

//#include <WindowsX.h>


enum MsgRelay_et { 
	MsgRelay_no=0, MsgRelay_yes=1, 
	Relay_no=0, Relay_yes=1 // old-names
};

#define SUBCLASS_FILTER_MSG0(hwnd, message, fn)    \
    case (message): \
{ \
	MsgRelay_et is_relay = FILTER_##message((hwnd), (wParam), (lParam), (fn)); \
	if(is_relay) \
		break; \
	else \
		return 0; \
} // the "0" in macro name "_MSG0" implies 'return 0 if MsgRelay_no'.

/*
Note: Most code below, is defining various FILTER_WM_xxx macros, which are very similar
to those HANDLE_WM_xxx macros in WindowsX.h, except that, FILTER_WM_xxx() always "returns"
a MsgRelay_et value.
*/

// MsgRelay_et Cls_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
#define FILTER_WM_COMMAND(hwnd, wParam, lParam, fn) \
	(fn)((hwnd), (int)(LOWORD(wParam)), (HWND)(lParam), (UINT)HIWORD(wParam))


// MsgRelay_et Cls_OnKey(HWND hwnd, UINT vk, BOOL fDown, int cRepeat, UINT flags)
#define FILTER_WM_KEYDOWN(hwnd, wParam, lParam, fn) \
	(fn)((hwnd), (UINT)(wParam), TRUE, (int)(short)LOWORD(lParam), (UINT)HIWORD(lParam))

// MsgRelay Cls_OnKey(HWND hwnd, UINT vk, BOOL fDown, int cRepeat, UINT flags)
#define FILTER_WM_KEYUP(hwnd, wParam, lParam, fn) \
	(fn)((hwnd), (UINT)(wParam), FALSE, (int)(short)LOWORD(lParam), (UINT)HIWORD(lParam))

// MsgRelay_et Cls_OnChar(HWND hwnd, TCHAR ch, int cRepeat)
#define FILTER_WM_CHAR(hwnd, wParam, lParam, fn) \
	(fn)((hwnd), (TCHAR)(wParam), (int)(short)LOWORD(lParam))

// MsgRelay_et Cls_OnMouseMove(HWND hEdit, int x, int y, UINT keyFlags)
#define FILTER_WM_MOUSEMOVE(hwnd, wParam, lParam, fn) \
	(fn)((hwnd), (int)(short)LOWORD(lParam), (int)(short)HIWORD(lParam), (UINT)(wParam))

// MsgRelay_et Cls_OnNCDestroy(HWND hwnd)
#define FILTER_WM_NCDESTROY(hwnd, wParam, lParam, fn) \
	(fn)(hwnd)



#endif // __WindowsX2_h_
