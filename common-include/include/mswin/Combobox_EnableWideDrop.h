#ifndef __Combobox_EnableWideDrop_h_20250708_
#define __Combobox_EnableWideDrop_h_20250708_

#include <windows.h>

enum DlgboxCbw_err 
{
	DlgboxCbw_Succ = 0,
	DlgboxCbw_Unknown = 1,

	// Error on enable
	DlgboxCbw_NoMem = 2,
	DlgboxCbw_AlreadyEnabled = 3,
	DlgboxCbw_BadParam = 4, 

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

#define CxxWindowSubclass_IMPL
#include <mswin/CxxWindowSubclass.h>


#ifndef Combobox_EnableWideDrop_DEBUG
#include <CHHI_vaDBG_hide.h>
#endif


namespace ComboboxEWD
{

const TCHAR *s_sigSubclass = _T("sig_ComboboxEWD");

enum { TimerWaitMax = 1000 , TimerInterval = 100 }; // in millisec

enum { ComboItemsCheckMax = 200 };
enum { ClbExtraWidth = 10 };

class CbxPeeker : public CxxWindowSubclass
{
public:
	CbxPeeker() 
	{
		this->hCombobox = NULL;
		this->isTimerStarted = false;
		this->msecTimerStart = 0;
	}

	virtual LRESULT WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
	MsgRelay_et Dlg_OnCommand(HWND hdlg, int id, HWND hwndCtl, UINT codeNotify);

public:
	HWND hCombobox; // the dropping down combobox

	bool isTimerStarted;
	DWORD msecTimerStart;
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

		vaDBG2(_T("  [ComboLBox #%d](%s) 0x%X, X=%d,Y=%d , w=%d,h=%d (pid=%d)%s"),
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

static void worki_StartTimer(HWND hdlg, HWND hcbx, CbxPeeker &worki)
{
	worki.hCombobox = hcbx;
	worki.isTimerStarted = true;
	worki.msecTimerStart = GetTickCount();

	SetTimer(hdlg, (UINT_PTR)&worki, TimerInterval, TimerProc_ProbeComboLBox);
}

static void worki_StopTimer(HWND hdlg, CbxPeeker &worki)
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

		vaDBG2(_T("  [Combobox item #%d] pixel width: %d"), i, size.cx);

		clb_new_width = _MAX_(clb_new_width, size.cx + scrollbar_width + ClbExtraWidth);
	}

	SelectFont(hdc, hfontOld);
	ReleaseDC(hComboLBox, hdc);

	vaDBG2(_T("  ComboLBox text width max pixels: %d"), clb_new_width);

	// Now actually change ComboLBox window size
	RECT rc = {};
	GetWindowRect(hComboLBox, &rc);
	MoveWindow(hComboLBox, 
		rc.right-clb_new_width, rc.top,
		clb_new_width, rc.bottom-rc.top, TRUE);
}

void CALLBACK TimerProc_ProbeComboLBox(HWND hdlg, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
	CbxPeeker &worki = *(CbxPeeker*)idEvent;

	// Enum all top-level windows to find a visible "ComboLBox"

	EnumChildWnd_st ecw = {};

	EnumChildWindows(NULL, EnumChildProc, (LPARAM)&ecw);

	HWND hClb = ecw.hwndVisibleComboLBox;
	if(hClb) // found
	{
		int clb_width = ecw.rcComboLBox.right-ecw.rcComboLBox.left;

		vaDBG2(_T("TimerProc: Found a visible ComboLBox window (hwnd=0x%08X), width=%dpx"), 
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
			vaDBG2(_T("TimerProc: Not seeing a visible ComboLBox window after timeout(%d millisec)"), TimerWaitMax);
		
			worki_StopTimer(hdlg, worki);
		}
	}
}


MsgRelay_et 
CbxPeeker::Dlg_OnCommand(HWND hdlg, int id, HWND hwndCtl, UINT codeNotify)
{
	TCHAR szClassname[80] = {};
	GetClassName(hwndCtl, szClassname, ARRAYSIZE(szClassname));

	if(_tcscmp(szClassname, _T("ComboBox"))==0)
	{
		HWND hcbx = hwndCtl;
		vaDBG2(_T("Combobox 0x%X (id=%d) codeNotify=%s"), 
			hcbx, id, ITCSv(codeNotify, itc::CBN_xxx_ComboBox)
			); (void)hcbx;
	}

	if(codeNotify==CBN_DROPDOWN)
	{
		HWND hcbx = hwndCtl;

		if(! isTimerStarted)
		{
			vaDBG2(_T("Got CBN_DROPDOWN, so start a timer to wait for ComboLBox's emerge."));
			worki_StartTimer(hdlg, hcbx, *this);
		}
	}

	return Relay_yes;
}

LRESULT  
CbxPeeker::WndProc(HWND hdlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
	SUBCLASS_FILTER_MSG0(hdlg, WM_COMMAND, Dlg_OnCommand);

	case WM_MOUSELEAVE: 
		// something that does not have message-cracker macro
		break;
	}

	// Call into existing WndProc chain.
	return DefSubclassProc(hdlg, uMsg, wParam, lParam);
}


DlgboxCbw_err _Dlgbox_EnableComboboxWideDrop(HWND hdlg)
{
	if(!IsWindow(hdlg))
		return DlgboxCbw_BadParam;

	CxxWindowSubclass::ReCode_et err = CxxWindowSubclass::E_Fail;
	CbxPeeker *peeker = CxxWindowSubclass::FetchCxxobjFromHwnd<CbxPeeker>(
		hdlg, s_sigSubclass, 
		TRUE, 
		&err);

	if(err==CxxWindowSubclass::E_Existed)
		return DlgboxCbw_AlreadyEnabled;

	if(!peeker)
		return DlgboxCbw_Unknown;

	return DlgboxCbw_Succ;
}

DlgboxCbw_err _Dlgbox_DisableComboboxWideDrop(HWND hdlg)
{
	if(!IsWindow(hdlg))
		return DlgboxCbw_BadParam;

	CxxWindowSubclass::ReCode_et err = CxxWindowSubclass::E_Fail;
	CbxPeeker *peeker = CxxWindowSubclass::FetchCxxobjFromHwnd<CbxPeeker>(
		hdlg, s_sigSubclass, 
		FALSE, // only fetch existing
		&err);

	if(err==CxxWindowSubclass::E_NotExist)
		return DlgboxCbw_NotEnabledYet;

	peeker->DetachHwnd(true);

	return DlgboxCbw_Succ;
}


} // namespace

// non-namespace wrapper:
//
DlgboxCbw_err Dlgbox_EnableComboboxWideDrop(HWND hdlg)
{
	return ComboboxEWD::_Dlgbox_EnableComboboxWideDrop(hdlg);
}
DlgboxCbw_err Dlgbox_DisableComboboxWideDrop(HWND hdlg)
{
	return ComboboxEWD::_Dlgbox_DisableComboboxWideDrop(hdlg);
}


#ifndef Combobox_EnableWideDrop_DEBUG
#include <CHHI_vaDBG_show.h>
#endif

#endif // Combobox_EnableWideDrop_IMPL

#endif
