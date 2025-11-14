#ifndef __utils_wingui_h_20250822_
#define __utils_wingui_h_20250822_

#include <windows.h>
#include <windowsx.h>
#include <tchar.h>


//
// (Use inside a DialogProc)
//
#define HANDLE_dlgMSG(hwnd, message, fn) \
  case (message): \
  return SetDlgMsgResult( hwnd, message, HANDLE_##message((hwnd), (wParam), (lParam), (fn)) );
// For message processing in a WinAPI user's dialog-procedure, we need a further step
// beyond that of windowsx.h's HANDLE_MSG(). This HANDLE_dlgMSG() applies that further step.
// Ref: Raymond Chen https://devblogs.microsoft.com/oldnewthing/20031107-00/?p=41923


int vaMsgBox(HWND hwnd, UINT utype, const TCHAR *szTitle, const TCHAR *szfmt, ...);
#define NULL_TITLE NULL // use for szTitle param

BOOL vaSetWindowText(HWND hwnd, const TCHAR *szfmt, ...);

BOOL vlSetDlgItemText(HWND hwnd, int nIDDlgItem, const TCHAR *szfmt, va_list args);
BOOL vaSetDlgItemText(HWND hwnd, int nIDDlgItem, const TCHAR *szfmt, ...);

void vlAppendText_mled(HWND hedit, const TCHAR *szfmt, va_list args);
void vaAppendText_mled(HWND hedit, const TCHAR *szfmt, ...);

typedef void PROC_WM_TIMER_call_once(void *usercontext);
bool WM_TIMER_call_once(HWND hwnd, int delay_millisec, PROC_WM_TIMER_call_once *userproc, void *usercontext);

void util_SetDlgDefaultButton(HWND hwndDlg, UINT idDefault);


///////////////////////////////////////////////////////////////
// Implementation Below:
///////////////////////////////////////////////////////////////

#ifdef utils_wingui_IMPL

#include <assert.h>

#define utils_env_IMPL
#include <mswin/utils_env.h> // need GetExeFilename


int vaMsgBox(HWND hwnd, UINT utype, const TCHAR *szTitle, const TCHAR *szfmt, ...)
{
	TCHAR szModuleName[40] = {};

	va_list args;
	va_start(args, szfmt);

	if(!szTitle) {
		szTitle = GetExeFilename();
	}

	TCHAR msgtext[4000] = {};
	_vsntprintf_s(msgtext, _TRUNCATE, szfmt, args);

	int ret = MessageBox(hwnd, msgtext, szTitle, utype);

	va_end(args);
	return ret;
}


BOOL vaSetWindowText(HWND hwnd, const TCHAR *szfmt, ...)
{
	va_list args;
	va_start(args, szfmt);

	TCHAR msgtext[400] = {};
	_vsntprintf_s(msgtext, _TRUNCATE, szfmt, args);

	BOOL succ = SetWindowText(hwnd, msgtext);

	va_end(args);
	return succ;
}

BOOL vlSetDlgItemText(HWND hwnd, int nIDDlgItem, const TCHAR *szfmt, va_list args)
{
	TCHAR tbuf[64000] = {};
	_vsntprintf_s(tbuf, _TRUNCATE, szfmt, args);
	BOOL succ = SetDlgItemText(hwnd, nIDDlgItem, tbuf);
	return succ;	
}

BOOL vaSetDlgItemText(HWND hwnd, int nIDDlgItem, const TCHAR *szfmt, ...)
{
	va_list args;
	va_start(args, szfmt);
	BOOL succ = vlSetDlgItemText(hwnd, nIDDlgItem, szfmt, args);
	va_end(args);
	return succ;
}

void vlAppendText_mled(HWND hedit, const TCHAR *szfmt, va_list args)
{
	TCHAR tbuf[64000] = {};
	_vsntprintf_s(tbuf, _TRUNCATE, szfmt, args);

	int pos = GetWindowTextLength (hedit);
	// -- [2024-10-26] Note: when pos reaches 30000, Edit_SetSel() will fail with
	// WinErr=5 (ERROR_ACCESS_DENIED).
	// User needs to raise the limit by calling:
	//   Edit_LimitText(hedit, 200*1024); // EM_LIMITTEXT

	Edit_SetSel(hedit, pos, pos);
	Edit_ReplaceSel(hedit, tbuf);
}

void vaAppendText_mled(HWND hedit, const TCHAR *szfmt, ...)
{
	va_list args;
	va_start(args, szfmt);
	vlAppendText_mled(hedit, szfmt, args);
	va_end(args);
}


//////////////////////////////////////////

typedef void PROC_WM_TIMER_call_once(void *usercontext);

struct WM_TIMER_call_once_st
{
	HWND hwnd;
	PROC_WM_TIMER_call_once *userproc;
	void *usercontext;
};

void CALLBACK WM_TIMER_call_once_TimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
	WM_TIMER_call_once_st *indata = (WM_TIMER_call_once_st*)idEvent;

	indata->userproc(indata->usercontext);

	KillTimer(indata->hwnd, idEvent);
	delete indata;
}


bool WM_TIMER_call_once(HWND hwnd, int delay_millisec, 
	PROC_WM_TIMER_call_once *userproc, void *usercontext)
{
	// TODO: If hwnd==NULL, maybe we can create a hidden window automatically.
	assert(hwnd);
	if(!hwnd)
		return false;

	WM_TIMER_call_once_st *indata = new WM_TIMER_call_once_st;
	indata->hwnd = hwnd;
	indata->userproc = userproc;
	indata->usercontext = usercontext;

	UINT_PTR uret = SetTimer(hwnd, (UINT_PTR)indata, delay_millisec, WM_TIMER_call_once_TimerProc);
	if(uret!=(UINT_PTR)indata)
	{
		delete indata;
		return false;
	}

	return true;
}


void util_SetDlgDefaultButton(HWND hwndDlg, UINT idDefault) 
{
	// Provided by PSSA2000 TokenMaster.cpp 

	// Get that last default control
	UINT nOld = (UINT) SendMessage(hwndDlg, DM_GETDEFID, 0, 0);

	// Reset the current default push button to a regular button.
	if (HIWORD(nOld) == DC_HASDEFID)
	{
		SendDlgItemMessage(hwndDlg, LOWORD(nOld), BM_SETSTYLE, 
			BS_PUSHBUTTON, // make it a normal button
			(LPARAM) TRUE);
	}

	// Update the default push button's control ID.
	SendMessage(hwndDlg, DM_SETDEFID, idDefault, 0L);

	// Set the new style.
	SendDlgItemMessage(hwndDlg, idDefault, BM_SETSTYLE, 
		BS_DEFPUSHBUTTON, // make it a stand-out button
		(LPARAM) TRUE); 
}


#endif // utils_wingui_IMPL


#endif
