#ifndef __mswinClarify_h_20250514_
#define __mswinClarify_h_20250514_

#include <stdio.h>
#include <stdarg.h>
#include <tchar.h>
#include <windows.h>

// #include <commdefs.h>

//..................... COMMON  ...................

#define HWND_NULL       NULL
#define NULL_HWND       NULL
#define NULL_HINSTANCE  NULL

#define NO_SECURITY_ATTR   NULL
#define NULL_SECURITY_ATTR NULL
	/* Many WIN32 functions require the parameter of `SECURITY_ATTRIBUTES*' ,
	if you don't want to assign a "security-attribute", you could use
	this symbol instead of NULL. These functions are:
		CreateFile,
		CreateThread,
		_beginthreadex,
	*/

// CreateEvent()
#define MANUAL_RESET      TRUE
#define INITIAL_SIGNALED  TRUE

//..................................................

// _beginthreadex()
#define STACK_SIZE_0 0
	/* use 0 to indicate the use of default stack size. */
#define CREATE_AND_RUN 0	
	/* For the "unsigned initflag" param. That means: create the thread an 
	run it immediately, as apposed to "CREATE_SUSPEND" flag. */


// Rectangle...

#define RECT_4PARAMS(r) (r).left, (r).top, (r).right, (r).bottom
	/* parameter extension for RECT.
		Usage: Rectangle(hdc, PARAM_RECT(r));
	*/

// SetDlgItemInt()
#define YES_SIGNED TRUE
#define NOT_SIGNED FALSE
	/* For the final param "BOOL bSigned" */

// WM_INITDIALOG's return value:
#define ACCEPT_DEFAULT_FOCUS	TRUE
#define SET_NEW_FOCUS			FALSE


#define EnableDlgItem(hWnd, ChildWndId) EnableWindow(GetDlgItem((hWnd),(ChildWndId)),TRUE)
#define DisableDlgItem(hWnd, ChildWndId) EnableWindow(GetDlgItem((hWnd),(ChildWndId)),FALSE)
#define EnableDlgItem_(hWnd, ChildWndId, bEnable) \
	EnableWindow(GetDlgItem((hWnd),(ChildWndId)),(ChildWndId),(bEnable))


// [2025-05-14] 

typedef INT_PTR DLGPROC_ret; // DialogProc's return value
enum { DLGPROC_ActionedNo = FALSE, DLGPROC_ActionedYes = TRUE };

inline bool IsDLGR12(int msg)
{
	if( (msg) == WM_CTLCOLORMSGBOX      || \
        (msg) == WM_CTLCOLOREDIT        || \
        (msg) == WM_CTLCOLORLISTBOX     || \
        (msg) == WM_CTLCOLORBTN         || \
        (msg) == WM_CTLCOLORDLG         || \
        (msg) == WM_CTLCOLORSCROLLBAR   || \
        (msg) == WM_CTLCOLORSTATIC      || \
        (msg) == WM_COMPAREITEM         || \
        (msg) == WM_VKEYTOITEM          || \
        (msg) == WM_CHARTOITEM          || \
        (msg) == WM_QUERYDRAGICON       || \
        (msg) == WM_INITDIALOG )
        return true;
    else
    	return false;
}



#endif
