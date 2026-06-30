#ifndef __CHHI__utils_wingui_h_
#define __CHHI__utils_wingui_h_
#define __CHHI__utils_wingui_h_created_ 20250822
#define __CHHI__utils_wingui_h_updated_ 20260625

#include <windows.h>
#include <windowsx.h>
#include <tchar.h>
#include <assert.h>

#include <sdring.h>


//
// (Use inside a DialogProc)
//
#define HANDLE_dlgMSG(hwnd, message, fn) \
  case (message): \
  return SetDlgMsgResult( hwnd, message, HANDLE_##message((hwnd), (wParam), (lParam), (fn)) );
  // For message processing in a WinAPI user's dialog-procedure, we need a further step
  // beyond that of windowsx.h's HANDLE_MSG(). This HANDLE_dlgMSG() applies that further step.
  // Ref: Raymond Chen https://devblogs.microsoft.com/oldnewthing/20031107-00/?p=41923
  // Evclip: [20241221.c1]

int vaMsgBox(HWND hwnd, UINT utype, const TCHAR *szTitle, const TCHAR *szfmt, ...);
// utype: 0 or MB_ICONERROR, MB_ICONWARNING
#define NULL_TITLE NULL // use for szTitle param

BOOL vaSetWindowText(HWND hwnd, const TCHAR *szfmt, ...);

BOOL vlSetDlgItemText(HWND hwnd, int nIDDlgItem, const TCHAR *szfmt, va_list args);
BOOL vaSetDlgItemText(HWND hwnd, int nIDDlgItem, const TCHAR *szfmt, ...);

void vlAppendText_mled(HWND hedit, const TCHAR *szfmt, va_list args);
void vaAppendText_mled(HWND hedit, const TCHAR *szfmt, ...);

void vaAppendLog_mled(HWND hedit, const TCHAR *szfmt, ...);
void vlAppendLog_mled(HWND hedit, const TCHAR *szfmt, va_list args);
// -- These two will add timestamp to each call.

Sdring sdrGetWindowText(HWND hwnd);
Sdring sdrGetDlgItemText(HWND hDlg, int nIDDlgItem);


bool Set_DlgDefaultButton(HWND hwndDlg, UINT idDefault);

struct FocusToDefaultBtn_st
{
	int uicFocus;
	int uicDefaultBtn;
};
//
void Set_DlgDefaultButton_byFocusId(HWND hwndDlg, int idFocus, 
	const FocusToDefaultBtn_st arUicMap[], int nUicMap);

inline void Set_WindowIcon(HWND hwnd, LPCTSTR resIcon)
{
	SendMessage(hwnd, WM_SETICON, TRUE, 
		(LPARAM)LoadIcon(GetWindowInstance(hwnd), resIcon)
		);
}


typedef void PROC_WM_TIMER_call_once(void *usercontext);
bool WM_TIMER_call_once(HWND hwnd, int delay_millisec, PROC_WM_TIMER_call_once *userproc, void *usercontext);

void util_SetDlgDefaultButton(HWND hwndDlg, UINT idDefault);


int getCheckedRadioButton(HWND hDlg, int nIDFirst, int nIDLast);

inline int enableDlgItem(HWND hDlg, int uic, BOOL isEnable) 
{
	HWND hUic = GetDlgItem(hDlg, uic);
	assert(IsWindow(hUic));
	return EnableWindow(hUic, isEnable);
}

HDC BeginPaint_NoFlicker(HWND hwnd, PAINTSTRUCT *lpPaint);

BOOL EndPaint_NoFlicker(HWND hwnd, PAINTSTRUCT *lpPaint);

BOOL GetClientRect_ScreenPos(HWND hwnd, RECT *pRect);

BOOL Hwnd_GetBorderWidths(HWND hwnd, RECT *pWidths, UINT new_style=0, UINT new_style_ex=0);

BOOL MoveWindow_byClientRect(HWND hwnd, const RECT *prcClientScreen=NULL,
	UINT new_style=0, UINT new_style_ex=0);

BOOL Set_ClientAreaSize(HWND hwnd, int width, int height); // in pixels

UINT Hwnd_TuneWinStyleBits  (HWND hwnd, UINT bits_on, UINT bits_off, bool is_tune_now=false);
UINT Hwnd_TuneWinStyleExBits(HWND hwnd, UINT bits_on, UINT bits_off, bool is_tune_now=false);

HMENU FindSubMenu_byText(HMENU hMenu, const TCHAR* menutext, int *pPos=nullptr);
int FindMenuitemPos_byText(HMENU hMenu, const TCHAR* menutext);
//int FindMenuitemPos_byID(HMENU hMenu, UINT id);
bool IsMenuitemExist_byID(HMENU hMenu, UINT id);
BOOL SetMenuitemText_byID(HMENU hMenu, UINT id, const TCHAR *newtext);



/*
////////////////////////////////////////////////////////////////////////////
 ___                 _                           _        _   _             
|_ _|_ __ ___  _ __ | | ___ _ __ ___   ___ _ __ | |_ __ _| |_(_) ___  _ __  
 | || '_ ` _ \| '_ \| |/ _ \ '_ ` _ \ / _ \ '_ \| __/ _` | __| |/ _ \| '_ \ 
 | || | | | | | |_) | |  __/ | | | | |  __/ | | | || (_| | |_| | (_) | | | |
|___|_| |_| |_| .__/|_|\___|_| |_| |_|\___|_| |_|\__\__,_|\__|_|\___/|_| |_|
              |_|                                                           
////////////////////////////////////////////////////////////////////////////
*/
// ++++++++++++++++++ Implementation Below ++++++++++++++++++
//
#if defined(utils_wingui_IMPL) || (defined CHHI_ALL_IMPL && !defined CHHI_ALL_IMPL_HIDE_utils_wingui) // [IMPL] begin
// [IMPL] //
// [IMPL] //
// Include system/OS headers here
#include <assert.h>
//
#include <vaDbgTs.h>
#include <mswin/utils_env.h> // need GetExeFilename()
// [IMPL] //
// [IMPL] //


int vaMsgBox(HWND hwnd, UINT utype, const TCHAR *szTitle, const TCHAR *szfmt, ...)
{
//	TCHAR szModuleName[40] = {};

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
	TCHAR tbuf[64000] = _T("");
	_vsntprintf_s(tbuf, _TRUNCATE, szfmt, args);

	int pos = GetWindowTextLength (hedit);
	// -- [2024-10-26] Note: when pos reaches 30000, Edit_SetSel() will fail with
	// WinErr=5 (ERROR_ACCESS_DENIED).
	// User needs to raise the limit by calling:
	//   Edit_LimitText(hedit, 200*1024); // EM_LIMITTEXT

	Edit_SetSel(hedit, pos, pos);
	Edit_ReplaceSel(hedit, tbuf);
}

void vaAppendLog_mled(HWND hedit, const TCHAR *szfmt, ...)
{
	va_list args;
	va_start(args, szfmt);
	vlAppendLog_mled(hedit, szfmt, args);
	va_end(args);
}

void vlAppendLog_mled(HWND hedit, const TCHAR *szfmt, va_list args)
{
	static Uint s_prev_msec = va_millisec();
	Uint now_msec = va_millisec();

	TCHAR timebuf[80] = _T("");
	TCHAR tbuf[64000] = _T("");

	// Print extra dot-line to show that time has elapsed for more than one second.
	Uint delta_msec = now_msec - s_prev_msec;
	if (delta_msec >= 1000)
	{
		TCHAR dots[13] = _T("..........\r\n");
		Uint delta_sec = delta_msec / 1000;
		if (delta_sec > 10)
			delta_sec = 10;

		dots[delta_sec] = '\r';
		dots[delta_sec+1] = '\n';
		dots[delta_sec+2] = '\0';
		_tcscpy_s(tbuf, dots);
	}

	_sntprintf_s(tbuf, _TRUNCATE, _T("%s[%s] "), tbuf,
		va_now_ymdhms_ms(timebuf, ARRAYSIZE(timebuf)));
	int pfxlen = (int)_tcslen(tbuf);

	_vsntprintf_s(tbuf+pfxlen, ARRAYSIZE(tbuf)-pfxlen, _TRUNCATE, szfmt, args);

	pfxlen = (int)_tcslen(tbuf);
	if(pfxlen<ARRAYSIZE(tbuf)-2)
	{
		tbuf[pfxlen]='\r'; tbuf[pfxlen+1]='\n'; tbuf[pfxlen+2]='\0';
	}

	int pos = GetWindowTextLength(hedit);
	// -- [2024-10-26] Note: when pos reaches 30000, Edit_SetSel() will fail with
	// WinErr=5 (ERROR_ACCESS_DENIED).
	// User needs to raise the limit by calling:
	//   Edit_LimitText(hedit, 200*1024); // EM_LIMITTEXT

	Edit_SetSel(hedit, pos, pos);
	Edit_ReplaceSel(hedit, tbuf);

	s_prev_msec = now_msec;
}

void vaAppendText_mled(HWND hedit, const TCHAR *szfmt, ...)
{
	va_list args;
	va_start(args, szfmt);
	vlAppendText_mled(hedit, szfmt, args);
	va_end(args);
}

Sdring sdrGetWindowText(HWND hwnd)
{
	assert(IsWindow(hwnd));
	if(!(IsWindow(hwnd)))
		return Sdring();
	
	int slen = GetWindowTextLength(hwnd);
	Sdring sdr(slen+1);
	GetWindowText(hwnd, sdr.getbuf(), sdr.rawlen());
	return sdr;
}

Sdring sdrGetDlgItemText(HWND hDlg, int nIDDlgItem)
{
	HWND hwnd = GetDlgItem(hDlg, nIDDlgItem);
	assert(IsWindow(hDlg));
	return sdrGetWindowText(hwnd);
}

//////////////////////////////////////////

bool Set_DlgDefaultButton(HWND hwndDlg, UINT idDefault) 
{
	// Get that last default control
	UINT nOld = (UINT) SendMessage(hwndDlg, DM_GETDEFID, 0, 0);

	// Reset the current default push button to a regular button.
	if (HIWORD(nOld) == DC_HASDEFID)
	{
		SendDlgItemMessage(hwndDlg, LOWORD(nOld), BM_SETSTYLE, 
			BS_PUSHBUTTON, // make it a normal button
			(LPARAM) TRUE);
	}

	HWND hbtn = GetDlgItem(hwndDlg, idDefault);
	TCHAR classname[40] = _T("");
	GetClassName(hbtn, classname, ARRAYSIZE(classname));
	if(_tcscmp(classname, _T("Button"))!=0)
	{
		assert( (_T("Not a Button"), 0) );
		return false;
	}

	// Update the default push button's control ID.
	SendMessage(hwndDlg, DM_SETDEFID, idDefault, 0L);

	// Set the new style.
	SendDlgItemMessage(hwndDlg, idDefault, BM_SETSTYLE, 
		BS_DEFPUSHBUTTON, // make it a stand-out button
		(LPARAM) TRUE); 

	return true;
}

void Set_DlgDefaultButton_byFocusId(HWND hwndDlg, int idFocus, 
	const FocusToDefaultBtn_st arUicMap[], int nUicMap)
{
	// chjmemo: For the input idFocus, check whether it has mapping in arUicMap[],
	// If yes, set the target Uic(must be a "button") to have BS_DEFPUSHBUTTON style.
	// This caters the paradigm: When user shifts dialog focus to some Uic-F,
	// user want current default-push-button to be some Uic-B, -- so that, 
	// user pressing Enter-key would execute that Uic-B button.
	//
	// If some .uicFocus is -1, then, the .uicDefaultBtn becomes the fallback DefaultBtn.

	int uicFallback = -1;
	assert(idFocus != -1);

	for(int i=0; i<nUicMap; i++)
	{
		if(arUicMap[i].uicFocus == idFocus)
		{
			Set_DlgDefaultButton(hwndDlg, arUicMap[i].uicDefaultBtn);
			return;
		}
		else if(arUicMap[i].uicFocus == -1)
		{
			uicFallback = arUicMap[i].uicDefaultBtn;
		}
	}

	if(uicFallback != -1)
		Set_DlgDefaultButton(hwndDlg, uicFallback);
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


int getCheckedRadioButton(HWND hDlg, int nIDFirst, int nIDLast)
{
    for (int nID = nIDFirst; nID <= nIDLast; nID++)
    {
        if (IsDlgButtonChecked(hDlg, nID) == BST_CHECKED)
        {
            return nID;  // Return the ID of the checked button
        }
    }
    return 0;  // No button checked
}


// Implement BeginPaint_NoFlicker() & EndPaint_NoFlicker()
// Info: https://docs.microsoft.com/en-us/previous-versions/ms969905(v=msdn.10)

struct DblBuffer_st
{
	HDC hdcReal;
	HDC hdcMem;
	HBITMAP hbmp;

	int cxReal;
	int cyReal;

	int nSaveDC;

} *g_pdbdc = NULL; // Here an global var.


HDC BeginPaint_NoFlicker(HWND hwnd, PAINTSTRUCT *lpPaint)
{
	assert(g_pdbdc == NULL);

	DblBuffer_st *pdb = new DblBuffer_st;
	memset(pdb, 0, sizeof(*pdb));

	pdb->hdcReal = BeginPaint(hwnd, lpPaint);

	pdb->hdcMem = CreateCompatibleDC(pdb->hdcReal);

	RECT rcReal = {};
	GetClientRect(hwnd, &rcReal);
	pdb->cxReal = rcReal.right - rcReal.left;
	pdb->cyReal = rcReal.bottom - rcReal.top;

	pdb->hbmp = CreateCompatibleBitmap(pdb->hdcReal, pdb->cxReal, pdb->cyReal);

	SelectObject(pdb->hdcMem, pdb->hbmp);

	pdb->nSaveDC = SaveDC(pdb->hdcMem);

	g_pdbdc = pdb;
	return pdb->hdcMem;
}

BOOL EndPaint_NoFlicker(HWND hwnd, PAINTSTRUCT *lpPaint)
{
	assert(g_pdbdc != NULL);

	DblBuffer_st *pdb = g_pdbdc;

	// Here, we need to restore the pristine DC params for hdcMem,
	// so that LT(0,0), RB(pdb->cxReal,pdb->cyReal) actually refers to the whole client-area.
	// I mean, if the caller had set a non-MM_TEXT mapping mode, 
	// LT(0,0), RB(pdb->cxReal,pdb->cyReal) would refer to a totally different area.
	BOOL succ = RestoreDC(pdb->hdcMem, pdb->nSaveDC);

	succ = BitBlt(pdb->hdcReal, 0, 0, pdb->cxReal, pdb->cyReal,
		pdb->hdcMem, 0, 0, SRCCOPY);

	EndPaint(hwnd, lpPaint);

	succ = ReleaseDC(hwnd, pdb->hdcReal);

	DeleteDC(pdb->hdcMem);
	DeleteObject(pdb->hbmp);

	delete pdb;
	g_pdbdc = NULL;
	return succ;
}


BOOL GetClientRect_ScreenPos(HWND hwnd, RECT *pRect)
{
	// The returned RECT is in screen-coordinate.
	if(!GetClientRect(hwnd, pRect))
		return FALSE;

	SetLastError(0);
	int mret = MapWindowPoints(hwnd, HWND_DESKTOP, (POINT*)pRect, 2);
	(void)mret;
	if(GetLastError())
		return FALSE;

	return TRUE;
}


BOOL Hwnd_GetBorderWidths(HWND hwnd, RECT *pWidths, UINT new_style, UINT new_style_ex)
{
	// Return window border pixels in *pWidth(all positive, at least 0).
	// User can change window styles(really change) before querying the width.
	// [2026-05-09] Used in DigClock2 v2.0

	if(!IsWindow(hwnd))
		return FALSE;

	if(new_style!=0)
		SetWindowLongPtr(hwnd, GWL_STYLE, new_style);

	if(new_style_ex!=0)
		SetWindowLongPtr(hwnd, GWL_EXSTYLE, new_style_ex);

	// (must) Repaint the window frame, so that we can calculate its *new* border size.
	SetWindowPos(hwnd, NULL, 0, 0, 0, 0,
		SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);

	RECT r1 = {}, r0 = {};
	GetWindowRect(hwnd, &r1);
	GetClientRect_ScreenPos(hwnd, &r0);

	RECT &ret = *pWidths;
	ret.left = r0.left - r1.left;
	assert(ret.left>=0);
	ret.right = r1.right - r0.right;
	assert(ret.right>=0);
	ret.top = r0.top - r1.top;
	assert(ret.top>=0);
	ret.bottom = r1.bottom - r0.bottom;
	assert(ret.bottom>=0);

	return TRUE;
}

BOOL MoveWindow_byClientRect(HWND hwnd, const RECT *prcClientScreen, 
	UINT new_style, UINT new_style_ex)
{
	// MoveWindow according to user-assigned client-area position.
	// [2026-05-09] Used in DigClock2 v2.0

	// If new_style!=0 or new_style_ex!=0, window style is changed before moving the window.

	// If prcClientScreen==NULL, we only change window-style, keeping client-area intact.

	RECT rcClientAbs = {};
	if(! GetClientRect_ScreenPos(hwnd, &rcClientAbs))
		return FALSE;

	if(prcClientScreen)
	{
		// Use user-assigned client-area pos.
		rcClientAbs = *prcClientScreen;
	}

	RECT bdw = {};
	Hwnd_GetBorderWidths(hwnd, &bdw, new_style, new_style_ex); // change winstyle inside!

	RECT rcFrame = { rcClientAbs.left - bdw.left , rcClientAbs.top - bdw.top,
		rcClientAbs.right + bdw.right , rcClientAbs.bottom + bdw.bottom };

	SetWindowPos(hwnd, NULL,
		rcFrame.left, rcFrame.top,
		rcFrame.right-rcFrame.left, rcFrame.bottom-rcFrame.top,
		SWP_NOZORDER | SWP_FRAMECHANGED);

	// [2026-05-09] Need this two lines to take effect?
	InvalidateRect(hwnd, NULL, TRUE);
	UpdateWindow(hwnd);

	return TRUE;
}

BOOL Set_ClientAreaSize(HWND hwnd, int width, int height)
{
	RECT rc = {};
	BOOL b = GetClientRect_ScreenPos(hwnd, &rc);
	if(!b)
		return b;

	rc.right = rc.left + width;
	rc.bottom = rc.top + height;
	b = MoveWindow_byClientRect(hwnd, &rc);
	return b;
}


UINT Hwnd_TuneWinStyleBits(HWND hwnd, UINT bits_on, UINT bits_off, bool is_tune_now)
{
	// [2026-05-09] Used in DigClock2 v2.0

	UINT winstyle = (UINT)GetWindowLongPtr(hwnd, GWL_STYLE);
	
	// Important: first do turn-off, then turn-on
	winstyle &= ~bits_off;
	winstyle |= bits_on;

	if(is_tune_now)
		SetWindowLongPtr(hwnd, GWL_STYLE, winstyle);

	return winstyle;
}

UINT Hwnd_TuneWinStyleExBits(HWND hwnd, UINT bits_on, UINT bits_off, bool is_tune_now)
{
	// [2026-05-09] Used in DigClock2 v2.0

	UINT exstyle = (UINT)GetWindowLongPtr(hwnd, GWL_EXSTYLE);

	// Important: first do turn-off, then turn-on
	exstyle &= ~bits_off;
	exstyle |= bits_on;

	if (is_tune_now)
		SetWindowLongPtr(hwnd, GWL_EXSTYLE, exstyle);

	return exstyle;
}


HMENU FindSubMenu_byText(HMENU hMenu, const TCHAR* menutext, int *pPos)
{
	SETTLE_OUTPUT_PTR(int, pPos, -1)

	int ncount = GetMenuItemCount(hMenu);
	for (int i = 0; i < ncount; i++)
	{
		TCHAR buffer[256];
		MENUITEMINFO mii = { sizeof(mii) };
		mii.fMask = MIIM_STRING | MIIM_SUBMENU;
		mii.dwTypeData = buffer;
		mii.cch = ARRAYSIZE(buffer);

		if (GetMenuItemInfo(hMenu, i, TRUE, &mii))
		{
			if (mii.hSubMenu)
			{
				if (_tcscmp(buffer, menutext) == 0)
				{ 
					*pPos = i;
					return mii.hSubMenu;
				}
			}
		}
	}
	return NULL;
}
//
// TODO: Add a function to recurse into root hMenu to find out an array of
// menu handles for user request submenu text strings. So that, user can grab them all
// when processing WM_INITMENUPOPUP.


int FindMenuitemPos_byText(HMENU hMenu, const TCHAR* menutext)
{
	// Return menuitem's position, -1 if not found.
	int ncount = GetMenuItemCount(hMenu);
	for (int i = 0; i < ncount; i++)
	{
		TCHAR buffer[256];
		MENUITEMINFO mii = { sizeof(mii) };
		mii.fMask = MIIM_STRING;
		mii.dwTypeData = buffer;
		mii.cch = ARRAYSIZE(buffer);

		if (GetMenuItemInfo(hMenu, i, TRUE, &mii))
		{
			if (_tcscmp(buffer, menutext) == 0)
				return i;
		}
	}
	return -1;
}

int FindMenuitemPos_byID__not_implemented_yet(HMENU hMenu, UINT id)
{
	MENUITEMINFO mii = { sizeof(mii) };
	mii.fMask = MIIM_ID;

	if (GetMenuItemInfo(hMenu, id, FALSE, &mii))
		return mii.wID;
	else
		return -1;
}

bool IsMenuitemExist_byID(HMENU hMenu, UINT id)
{
	MENUITEMINFO mii = { sizeof(mii) };
	mii.fMask = MIIM_ID;

	if (GetMenuItemInfo(hMenu, id, FALSE, &mii))
		return true;
	else
		return false;
}

BOOL SetMenuitemText_byID(HMENU hMenu, UINT id, const TCHAR *newtext)
{
	MENUITEMINFO mii = { sizeof(mii) };
	mii.fMask = MIIM_STRING;
	mii.dwTypeData = (LPTSTR)newtext;

	BOOL b = SetMenuItemInfo(hMenu, id, FALSE, &mii);
	return b;
}


#endif // [IMPL]


#endif
