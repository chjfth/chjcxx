#ifndef __WIN32CLARIFY_H
#define __WIN32CLARIFY_H

#include <stdio.h>
#include <stdarg.h>
#include <tchar.h>

#include <commdefs.h>

//..................... COMMON  ...................

#define HWND_NULL NULL
#define NULL_HWND NULL
#define NULL_HINSTANCE NULL

#define NO_SECURITY_ATTR NULL
#define NULL_SECURITY_ATTR NULL
	/* Many WIN32 functions require the parameter of `SECURITY_ATTRIBUTES*' ,
	if you don't want to assign a "security-attribute", you could use
	this symbol instead of NULL. These functions are:
		CreateFile,
		CreateThread,
		_beginthreadex,
	*/

// CreateEvent()
#define MANUAL_RESET TRUE
#define INITIAL_SIGNALED TRUE

//..................................................

// _beginthreadex()
#define STACK_SIZE_0 0
	/* use 0 to indicate the use of default stack size. */
#define CREATE_AND_RUN 0	
	/* For the "unsigned initflag" param. That means: create the thread an 
	run it immediately, as apposed to "CREATE_SUSPEND" flag. */


// Rectangle...

#define PARAM_EXT_RECT(r) (r).left, (r).top, (r).right, (r).bottom
	/* parameter extension for RECT.
		Usage: Rectangle(hdc, PARAM_RECT(r));
	*/

// SetDlgItemInt()
#define YES_SIGNED TRUE
#define NOT_SIGNED FALSE
	/* For the final param "BOOL bSigned" */

// WM_INITDIALOG

// return value:
#define ADMIT_DEFAULT_FOCUS		TRUE
#define SET_NEW_FOCUS			FALSE


#define EnableDlgItem(hWnd, ChildWndId) EnableWindow(GetDlgItem((hWnd),(ChildWndId)),TRUE)
#define DisableDlgItem(hWnd, ChildWndId) EnableWindow(GetDlgItem((hWnd),(ChildWndId)),FALSE)
#define EnableDlgItem_(hWnd, ChildWndId, bEnable) \
	EnableWindow(GetDlgItem((hWnd),(ChildWndId)),(ChildWndId),(bEnable))

/////////////////////////////////////////////////////////////

inline
int cl_MessageBoxPrintf(HWND hwnd, TCHAR *szCaption, TCHAR * szFormat, ...)
{
	TCHAR   szBuffer [1024] ;
	va_list pArgList ;
	
	va_start(pArgList, szFormat) ;
		// The va_start macro (defined in STDARG.H) is usually equivalent to:
		// pArgList = (char *) &szFormat + sizeof(szFormat); [if sizeof(szFormat) is multiple of sizeof(int*)]
	
	_vsntprintf(szBuffer, sizeof (szBuffer) / sizeof (TCHAR), szFormat, pArgList) ;
		// The last argument to wvsprintf points to the arguments
	
	va_end(pArgList) ;
	return MessageBox(hwnd, szBuffer, szCaption, 0) ;
}



inline 
int cl_MessageBoxIds(HWND hWnd, UINT idsCaption, UINT idsText, UINT uType = 0, 
	HINSTANCE hInstanceWithIds = NULL)
{
	int lenText = 0, lenCaption = 0;
	TCHAR tsCaption[128], tsText[1024-128];
	lenText = LoadString(hInstanceWithIds, idsText, tsText, GetEleQuan(tsText));
	if(idsCaption) lenCaption = LoadString(hInstanceWithIds, idsCaption, tsCaption, GetEleQuan(tsCaption));
	return MessageBox(hWnd, tsText, idsCaption?tsCaption:NULL, uType);
}


inline
int cl_MessageBoxIdsPrintf(HWND hwnd, HINSTANCE hInstanceWithIds, 
	UINT idsCaption, UINT idsTextFormat, ...)
{
	int lenTextFormat = 0, lenCaption = 0;
	TCHAR tsCaption[128], tsTextFormat[1024-128], tsText[1024];
	lenTextFormat = LoadString(hInstanceWithIds, idsTextFormat, tsTextFormat, GetEleQuan(tsTextFormat));
	if(idsCaption) lenCaption = LoadString(hInstanceWithIds, idsCaption, tsCaption, GetEleQuan(tsCaption));
	
	va_list pArgList ;
	va_start(pArgList, idsTextFormat) ;
	_vsntprintf(tsText, GetEleQuan(tsText), tsTextFormat, pArgList) ;
	va_end(pArgList) ;

	return MessageBox(hwnd, tsText, idsCaption?tsCaption:NULL, 0);
}



#endif
