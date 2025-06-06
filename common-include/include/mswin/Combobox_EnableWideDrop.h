#ifndef __Combobox_EnableWideDrop_h_20250326_
#define __Combobox_EnableWideDrop_h_20250326_

#include <windows.h>

enum DlgboxCbw_err 
{
	DlgboxCbw_Succ = 0,
	DlgboxCbw_Unknown = 1,

	// Error on enable
	DlgboxCbw_NoMem = 2,
	DlgboxCbw_AlreadyEnabled = 3,
	DlgboxCbw_SetProp = 4, // WinAPI SetProp() error

	// Error on disable
	DlgboxCbw_NotEnabledYet = 10,
	DlgboxCbw_ChainMoved = 11,
};

DlgboxCbw_err Dlgbox_EnableComboboxWideDrop(HWND hdlg);

DlgboxCbw_err Dlgbox_DisableComboboxWideDrop(HWND hdlg);

///////////////////////////////////////////////////////////////
// Implementation Below:
///////////////////////////////////////////////////////////////

#ifdef Combobox_EnableWideDrop_IMPL

#include <assert.h>
#include <stdarg.h>
#include <tchar.h>
#include <WindowsX.h>
#include <mswin/WindowsX2.h> // chj: for SUBCLASS_FILTER_MSG0()
#include <_MINMAX_.h>

#ifdef Combobox_EnableWideDrop_DEBUG
#undef  vaDBG      // revoke empty effect
#else
#define vaDBG(...) // make vaDBG empty, no debugging message
#endif


namespace CbxEWD
{

// Keyword used to store the original WndProc in the window's property-list
const TCHAR* CbxEnableWideDrop_PROP_STR = _T("Combobox_EnableWideProp_workdata");

enum { HEXMAGIC = 0x20250324 };
enum { TimerWaitMax = 1000 , TimerInterval = 100 }; // in millisec

enum { ComboItemsCheckMax = 200 };
enum { ClbExtraWidth = 10 };

struct CbxWideDrop_st
{
	UINT hexmagic;
	WNDPROC wndproc_was;

	HWND hCombobox; // the dropping down combobox

	bool isTimerStarted;
	DWORD msecTimerStart;

	CbxWideDrop_st(WNDPROC wndproc_was)
	{
		this->hexmagic = HEXMAGIC;
		this->wndproc_was = wndproc_was;

		this->hCombobox = NULL;
		this->isTimerStarted = false;
		this->msecTimerStart = 0;
	}

};

struct EnumChildWnd_st {
	int idx;
	HWND hwndVisibleComboLBox;
	RECT rcComboLBox;
};


static BOOL CALLBACK EnumChildProc(HWND hwnd, LPARAM ctx)
{
	EnumChildWnd_st &ecw = *(EnumChildWnd_st*)ctx;

	TCHAR szClassname[80] ={};
	GetClassName(hwnd, szClassname, ARRAYSIZE(szClassname));

	if(_tcscmp(szClassname, _T("ComboLBox"))==0)
	{
		RECT rc = {};
		GetWindowRect(hwnd, &rc);

		DWORD hwnd_pid = 0;
		GetWindowThreadProcessId(hwnd, &hwnd_pid);
		DWORD my_pid = GetCurrentProcessId();

		DWORD ws = GetWindowStyle(hwnd);
		bool isVisible = ws & WS_VISIBLE ? true : false;

		vaDBG(_T("  [ComboLBox #%d](%s) 0x%X, X=%d,Y=%d , w=%d,h=%d (pid=%d)%s"),
			ecw.idx+1, 
			isVisible ? _T("visi") : _T("hide"),
			hwnd,
			rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top,
			hwnd_pid, (my_pid==hwnd_pid ? _T("(self)") : _T(""))
			); (void)my_pid;

		ecw.idx++;

		if(isVisible)
		{
			ecw.hwndVisibleComboLBox = hwnd;
			ecw.rcComboLBox = rc;
		}
	}

	return TRUE;
};

static void CALLBACK TimerProc_ProbeComboLBox(
	HWND hdlg, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);

static void worki_StartTimer(HWND hdlg, HWND hcbx, CbxWideDrop_st &worki)
{
	worki.hCombobox = hcbx;
	worki.isTimerStarted = true;
	worki.msecTimerStart = GetTickCount();

	SetTimer(hdlg, (UINT_PTR)&worki, TimerInterval, TimerProc_ProbeComboLBox);
}

static void worki_StopTimer(HWND hdlg, CbxWideDrop_st &worki)
{
	worki.hCombobox = NULL;
	worki.isTimerStarted = false;
	worki.msecTimerStart = 0;

	KillTimer(hdlg, (UINT_PTR)&worki);
}

void do_AdjustClbWidth(HWND hcbx, HWND hComboLBox, int clb_old_width)
{
	// Find out max width from Combobox items.

	HDC hdc = GetDC(hComboLBox);
	if(!hdc)
		return;

	// Determine whether hComboLBox has a vertical scrollbar, if so, it counts to extra width.
	DWORD ws = GetWindowStyle(hComboLBox);
	int scrollbar_width = (ws & WS_VSCROLL) ? GetSystemMetrics(SM_CXVSCROLL) : 0;

	// Use hComboLBox's current Font to determine droplist text width.

	HFONT hfontUse = GetWindowFont(hComboLBox);
	HFONT hfontOld = SelectFont(hdc, hfontUse);

	int clb_new_width = clb_old_width;
	TCHAR tbuf[800] = {};
	
	int items = ComboBox_GetCount(hcbx);
	items = _MIN_(items, ComboItemsCheckMax);

	for(int i=0; i<items; i++)
	{
		int tlen = ComboBox_GetLBTextLen(hcbx, i);
		if(tlen>=ARRAYSIZE(tbuf))
			continue; // ignore that weirdly long item

		ComboBox_GetLBText(hcbx, i, tbuf);

		SIZE size = {};
		GetTextExtentPoint32(hdc, tbuf, tlen, &size);

		vaDBG(_T("  [Combobox item #%d] pixel width: %d"), i, size.cx);

		clb_new_width = _MAX_(clb_new_width, size.cx + scrollbar_width + ClbExtraWidth);
	}

	SelectFont(hdc, hfontOld);
	ReleaseDC(hComboLBox, hdc);

	vaDBG(_T("  ComboLBox text width max pixels: %d"), clb_new_width);

	// Now actually change ComboLBox window size
	RECT rc = {};
	GetWindowRect(hComboLBox, &rc);
	MoveWindow(hComboLBox, 
		rc.right-clb_new_width, rc.top,
		clb_new_width, rc.bottom-rc.top, TRUE);
}

void CALLBACK TimerProc_ProbeComboLBox(HWND hdlg, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
	CbxWideDrop_st &worki = *(CbxWideDrop_st*)idEvent;

	// Enum all top-level windows to find a visible "ComboLBox"

	EnumChildWnd_st ecw = {};

	EnumChildWindows(NULL, EnumChildProc, (LPARAM)&ecw);

	HWND hClb = ecw.hwndVisibleComboLBox;
	if(hClb) // found
	{
		int clb_width = ecw.rcComboLBox.right-ecw.rcComboLBox.left;

		vaDBG(_T("TimerProc: Found a visible ComboLBox window (hwnd=0x%08X), width=%dpx"), 
			hClb, clb_width);
		
		do_AdjustClbWidth(worki.hCombobox, hClb, clb_width);

		worki_StopTimer(hdlg, worki);
	}
	else // not found
	{
		DWORD msecEnd = worki.msecTimerStart + TimerWaitMax;
		DWORD msecNow = GetTickCount();

		if( int(msecNow - msecEnd) >= 0 )
		{
			vaDBG(_T("TimerProc: Not seeing a visible ComboLBox window after timeout(%d millisec)"), TimerWaitMax);
		
			worki_StopTimer(hdlg, worki);
		}
	}
}


MsgRelay_et Dlg_OnCommand(HWND hdlg, int id, HWND hwndCtl, UINT codeNotify)
{
	CbxWideDrop_st &worki = *(CbxWideDrop_st*)GetProp(hdlg, CbxEnableWideDrop_PROP_STR);

	TCHAR szClassname[80] = {};
	GetClassName(hwndCtl, szClassname, ARRAYSIZE(szClassname));

	if(_tcscmp(szClassname, _T("ComboBox"))==0)
	{
		HWND hcbx = hwndCtl;
		vaDBG(_T("Combobox 0x%X (id=%d) codeNotify=%s"), 
			hcbx, id, ITCSv(codeNotify, itc::CBN_xxx_ComboBox)
			); (void)hcbx;
	}

	if(codeNotify==CBN_DROPDOWN)
	{
		HWND hcbx = hwndCtl;

		if(! worki.isTimerStarted)
		{
			vaDBG(_T("Got CBN_DROPDOWN, so start a timer to wait for ComboLBox's emerge."));
			worki_StartTimer(hdlg, hcbx, worki);
		}
	}

	return Relay_yes;
}

static LRESULT CALLBACK 
Dlg_WndProc_EnableCbxWideDrop(HWND hdlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CbxWideDrop_st *myprop = (CbxWideDrop_st*)GetProp(hdlg, CbxEnableWideDrop_PROP_STR);
	WNDPROC oldWndProc = myprop->wndproc_was;

//	logmsg(_T("Combobox uMsg: %s"), ITCSv(uMsg, WM_xxx));

	switch(uMsg)
	{
	SUBCLASS_FILTER_MSG0(hdlg, WM_COMMAND, Dlg_OnCommand);

	case WM_MOUSELEAVE: 
		// something that does not have message-cracker macro
		break;
	}

	// Call the original WndProc for default processing
	return CallWindowProc(oldWndProc, hdlg, uMsg, wParam, lParam);
}

DlgboxCbw_err _Dlgbox_EnableComboboxWideDrop(HWND hdlg)
{
	CbxWideDrop_st *myprop = (CbxWideDrop_st*)GetProp(hdlg, CbxEnableWideDrop_PROP_STR);
	if(myprop)
		return DlgboxCbw_AlreadyEnabled;

	myprop = new CbxWideDrop_st(nullptr);
	if(!myprop)
		return DlgboxCbw_NoMem;

	WNDPROC oldWndProc = SubclassWindow(hdlg, Dlg_WndProc_EnableCbxWideDrop);
	myprop->wndproc_was = oldWndProc;

	BOOL succ = SetProp(hdlg, CbxEnableWideDrop_PROP_STR, myprop); // Store custom props into hcbx.
	if(succ)
	{
		return DlgboxCbw_Succ;
	}
	else
	{	// restore old state
		SubclassWindow(hdlg, oldWndProc);
		delete myprop;
		return DlgboxCbw_SetProp;
	}
}

DlgboxCbw_err _Dlgbox_DisableComboboxWideDrop(HWND hdlg)
{
	WNDPROC currentWndProc = (WNDPROC)GetWindowLongPtr(hdlg, GWLP_WNDPROC);
	CbxWideDrop_st *myprop = (CbxWideDrop_st*)GetProp(hdlg, CbxEnableWideDrop_PROP_STR);

	if(!myprop)
		return DlgboxCbw_NotEnabledYet;

	if(currentWndProc != Dlg_WndProc_EnableCbxWideDrop)
	{
		// Someone else has further subclassed this hEdit, so we should not unsubclass it.
		return 	DlgboxCbw_ChainMoved;
	}

	assert(myprop->hexmagic==HEXMAGIC);
	assert(myprop->wndproc_was);

	// Restore original WndProc for combobox
	SubclassWindow(hdlg, myprop->wndproc_was);

	// Remove our custom prop
	RemoveProp(hdlg, CbxEnableWideDrop_PROP_STR);

	// Destroy our custom prop storage
	delete myprop;

	return DlgboxCbw_Succ;
}


} // namespace

// non-namespace wrapper:
//
DlgboxCbw_err Dlgbox_EnableComboboxWideDrop(HWND hdlg)
{
	return CbxEWD::_Dlgbox_EnableComboboxWideDrop(hdlg);
}
DlgboxCbw_err Dlgbox_DisableComboboxWideDrop(HWND hdlg)
{
	return CbxEWD::_Dlgbox_DisableComboboxWideDrop(hdlg);
}



#endif // Combobox_EnableWideDrop_IMPL

#endif // __Combobox_EnableWideDrop_h_
