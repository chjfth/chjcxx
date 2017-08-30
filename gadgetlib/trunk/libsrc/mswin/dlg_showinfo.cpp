#include "stdafx.h"

#include <EnsureClnup_common.h>
#include <EnsureClnup_mswin.h>
#include <AutoBuf.h>

#include <mswin/JULayout.h>
#include <mswin/CmnHdr-Jeffrey.h>

#include <gadgetlib/enum_monitors.h>
#include <gadgetlib/ReposNewbox.h>
#include <gadgetlib/dlg_showinfo.h>

#include <dlg_showinfo_ids.h>

const int dlg_timer_id = 1;

struct DsiDlgParams_st
{
	// Input params from caller:
	const TCHAR *title;
	TCHAR *textbuf;
	int bufchars;
	HICON hIcon;
	bool isFixedWidthFont;
	//
	PROC_DlgShowinfo_GetText procGetText;
	void *ctxGetText;
	//
	int msecAutoRefresh; // non-zero will enable auto-refresh feature
	bool isAutoRefreshNow; 
	//
	bool isOnlyClosedByProgram;
	//
	const TCHAR *szOK;
	const TCHAR *szRefreshBtnText; // user can customize button text
	const TCHAR *szAutoChktext;

	// Internal data for Showinfo-dialog:
	HWND hwndRealParent;
	HFONT hfontFixedWidth;

	Rect_st rectNewboxVisualMax; 
		// This Rect-size is enough to hold all our info text.
		// This Rect will inflate along with user text increasing, not deflating.
		// The rect is from GetWindowRect.
	bool ScrollbarAlways;

	bool isTimerOn;
	HICON hIconNow; // may switch between hIcon and an "error icon"

	dlg_showinfo_callback_st cb_info;

	JULayout jul; 

public:
	DsiDlgParams_st(const dlg_showinfo_st &in, const TCHAR *pszInfo);

};

DsiDlgParams_st::DsiDlgParams_st(const dlg_showinfo_st &in, const TCHAR *pszInfo)
{
	memset(this, 0, sizeof(*this));

	title = in.title;
	textbuf = (TCHAR*)pszInfo;
	bufchars = in.bufchars;
	hIcon = in.hIcon;
	isFixedWidthFont = in.fixedwidth_font;
	//
	procGetText = in.procGetText;
	ctxGetText = in.ctxGetText;
	//
	msecAutoRefresh = in.msecAutoRefresh;
	isAutoRefreshNow = in.isAutoRefreshNow;
	//
	isOnlyClosedByProgram = in.isOnlyClosedByProgram;
	//
	szOK = in.szOK;
	szRefreshBtnText = szRefreshBtnText;
	szAutoChktext = szAutoChktext;
}

static void 
dsi_SetIcon(HWND hwnd, HICON hNewIcon, DsiDlgParams_st *pr)
{
	if(hNewIcon==pr->hIconNow)
		return; // no need to redraw the icon

	pr->hIconNow = hNewIcon;

	HWND hctlIcon = GetDlgItem(hwnd, IDI_SHOW_INFO);
	Static_SetIcon(hctlIcon, pr->hIconNow);
}

static void 
dsi_StartTimer(HWND hwnd, DsiDlgParams_st *pr)
{
	if(pr->isTimerOn)
		return;

	UINT_PTR succ = SetTimer(hwnd, dlg_timer_id, pr->msecAutoRefresh, NULL);
	if(succ)
		pr->isTimerOn = true;
}

static void 
dsi_StopTimer(HWND hwnd, DsiDlgParams_st *pr)
{
	if(pr->isTimerOn==false)
		return;

	KillTimer(hwnd, dlg_timer_id);
	pr->isTimerOn = false;
}

static Rect_st 
dsi_CalNewboxTextMax(HWND hdlg, DsiDlgParams_st *pr)
{
	// Determine display-area of the input string(probably multi-line), 
	// then we know(return) the corresponding dialogbox-window size.

	Rect_st rectTextMax = {0,0,0,0}; // as return value

	// ... cal input string display width ...

	HWND hEdit = GetDlgItem(hdlg, IDC_EDIT_SHOW_INFO);
	HDC hdcEdit = GetDC(hEdit);
	int textlen = (int)_tcslen(pr->textbuf);

	RECT drawarea = {0,0,1,1};
	SelectObject(hdcEdit, pr->hfontFixedWidth); // we need this to get correct draw-width
	DrawText(hdcEdit, pr->textbuf, textlen, &drawarea, DT_CALCRECT);
	// GetTextExtentPoint32() does not consider multi-line height so don't use it.
	// But note: Line-breaks must be "\r\n"(not "\n") to have DrawText calculate drawarea correctly.

	ReleaseDC(hdlg, hdcEdit);

	// ... cal ideal width of the newbox window ... (newbox is the new MessageBox we will pop)

	const int width_vscrollbar = GetSystemMetrics(SM_CXVSCROLL);
	const int width_extra_gap = 16; // casual, 0 may cause unexpected line-break when scrollbar is shown

	RECT rectDlg, rectEdit;
	GetWindowRect(hdlg, &rectDlg);
	GetWindowRect(hEdit, &rectEdit);
	int width_diff =  RECTcx(rectDlg) - RECTcx(rectEdit);
	int height_diff = RECTcy(rectDlg) - RECTcy(rectEdit);
	int newbox_width = width_diff + RECTcx(drawarea) + width_vscrollbar + width_extra_gap;
	int newbox_height = height_diff + RECTcy(drawarea);

	//  ... get monitors info so to know newbox ideal size ...

	CAutoBuf<OneMonitorInfo_st, sizeof(OneMonitorInfo_st)> abMonsinfo;
	WinErr_t winerr = 0;
	do {
		winerr = ggt_EnumMonitors(abMonsinfo, abMonsinfo, abMonsinfo);
	} while(winerr==ERROR_MORE_DATA);

	if(winerr)
		return rectTextMax;

	ReposNewboxInput_st nbi = {};
	nbi.nMonitors = abMonsinfo;
	//
	int rect_buf_bytes = nbi.nMonitors*sizeof(RECT);
	Cec_delete_pchar rectbuf = new char[rect_buf_bytes];
	nbi.arMonitorRect = (Rect_st*)(void*)rectbuf;
	for(int i=0; i<nbi.nMonitors; i++)
		nbi.arMonitorRect[i] = *(Rect_st*)&abMonsinfo[i].rcWorkArea;
	//
	GetWindowRect(pr->hwndRealParent, (RECT*)&nbi.rectParent);
	nbi.sizeNewboxIdeal.cx = newbox_width;
	nbi.sizeNewboxIdeal.cy = newbox_height;
	//
	int idxNewMonitor = ggt_ReposNewbox(nbi, &rectTextMax);
	if(idxNewMonitor<0)
		return rectTextMax; // not likely to happen

	const Rect_st &rectNewMonitor = nbi.arMonitorRect[idxNewMonitor];

	if( RECTcx(rectNewMonitor)<nbi.sizeNewboxIdeal.cx || RECTcy(rectNewMonitor)<nbi.sizeNewboxIdeal.cy )
		pr->ScrollbarAlways = true;

	return rectTextMax;
}

static void 
dsi_CallbackRefreshUserText(HWND hwnd, DsiDlgParams_st *pr)
{
	pr->cb_info.hDlg = hwnd;

	DlgShowinfoCallback_ret ret = pr->procGetText(pr->ctxGetText, pr->cb_info,
		pr->textbuf, pr->bufchars); 
		// Will update pr->textbuf[]
	
	if(ret==DSICB_OK)
	{
		dsi_SetIcon(hwnd, pr->hIcon, pr);
	}
	else if(ret==DSICB_CloseDlg)
	{
		EndDialog(hwnd, Dsie_Success);
	}
	else 
	{
		// Set a yellow triangle icon
		dsi_SetIcon(hwnd, LoadIcon(NULL, IDI_EXCLAMATION), pr);

		if(ret==DSICB_Fail_StopAutoRefresh)
		{
			dsi_StopTimer(hwnd, pr);
			CheckDlgButton(hwnd, IDC_CHK_AUTOREFRESH, BST_UNCHECKED);
		}
	}

	Rect_st rectNow;
	GetWindowRect(hwnd, (RECT*)&rectNow);
	bool wasAtVisualMax = RECT_IsSameSize(rectNow, pr->rectNewboxVisualMax) ? true : false;

	bool isGoBigger = false;
	Rect_st rectTextMax = dsi_CalNewboxTextMax(hwnd, pr);

	if( RECTcx(rectTextMax) > RECTcx(pr->rectNewboxVisualMax) )
	{
		pr->rectNewboxVisualMax.left = rectTextMax.left;
		pr->rectNewboxVisualMax.right = rectTextMax.right;
		isGoBigger = true;
	}

	if( RECTcy(rectTextMax) > RECTcy(pr->rectNewboxVisualMax) )
	{
		pr->rectNewboxVisualMax.top = rectTextMax.top;
		pr->rectNewboxVisualMax.bottom = rectTextMax.bottom;
		isGoBigger = true;
	}

	if(isGoBigger && wasAtVisualMax)
	{
		// If dlg has been shrunk(due to user purpose), don't resize it even if we get more text.
		// And, Not changing top-left position.
		MoveWindow(hwnd, rectNow.left, rectNow.top, 
			RECTcx(pr->rectNewboxVisualMax), RECTcy(pr->rectNewboxVisualMax), TRUE);
	}

	SetDlgItemText(hwnd, IDC_EDIT_SHOW_INFO, pr->textbuf);
}

static DlgShowinfoCallback_ret 
Dumb_GetText(void *ctx, const dlg_showinfo_callback_st &cb_info, TCHAR *textbuf, int bufchars)
{
	(void)ctx; (void)cb_info; (void)textbuf; (void)bufchars;
	return DSICB_Fail;
}


BOOL dsi_OnInitDialog(HWND hdlg, HWND hwndFocus, LPARAM lParam) 
{
	DsiDlgParams_st *pr = (DsiDlgParams_st*)lParam;
	SetWindowLongPtr(hdlg, DWLP_USER, (LONG_PTR)pr); // save our dlg private data

	JULayout &jul = pr->jul;
	jul.Initialize(hdlg);

	jul.AnchorControl(0, 0, 100, 100, IDC_EDIT_SHOW_INFO); 
	jul.AnchorControl(50, 100, 50, 100, IDOK);

	if(pr->isAutoRefreshNow)
	{
		dsi_CallbackRefreshUserText(hdlg, pr);
	}

	HWND hctlIcon = GetDlgItem(hdlg, IDI_SHOW_INFO);
	Static_SetIcon(hctlIcon, pr->hIcon);
	if(pr->hIcon)
	{
		SendMessage(hdlg, WM_SETICON, TRUE,  (LPARAM)pr->hIcon);
		SendMessage(hdlg, WM_SETICON, FALSE, (LPARAM)pr->hIcon);

	}
	//	chSETDLGICONS(hdlg, IDI_NEWLAND16);

	SetWindowText(hdlg, pr->title);
	SetDlgItemText(hdlg, IDC_EDIT_SHOW_INFO, pr->textbuf);

	// Create fixed-width font if required.
	if(pr->isFixedWidthFont)
	{
		LOGFONT logfont = {};
		HFONT hFont = (HFONT)SendDlgItemMessage(hdlg, IDC_EDIT_SHOW_INFO, WM_GETFONT, 0, 0);
		int retbytes = GetObject(hFont, sizeof(logfont), &logfont);
		(void)retbytes;
		assert(retbytes==sizeof(logfont));
		logfont.lfPitchAndFamily = FIXED_PITCH; //logfont.lfWeight = FW_BOLD;
		logfont.lfFaceName[0] = 0; // let system pick a default fixed-width font
		pr->hfontFixedWidth = CreateFontIndirect(&logfont);
		assert(pr->hfontFixedWidth);

		SendDlgItemMessage(hdlg, IDC_EDIT_SHOW_INFO, WM_SETFONT, (WPARAM)(pr->hfontFixedWidth), TRUE);
	}

	Rect_st &rectMax = pr->rectNewboxVisualMax;
	rectMax = dsi_CalNewboxTextMax(hdlg, pr);

	MoveWindow(hdlg, rectMax.left, rectMax.top, RECTcx(rectMax), RECTcy(rectMax), TRUE);

	// Hide hEdit's ugly scrollbar, because we do not need it now.
	// It will be shown later when the message box is shrunk, when processing WM_SIZE.
	HWND hEdit = GetDlgItem(hdlg, IDC_EDIT_SHOW_INFO);
	ShowScrollBar(hEdit, SB_VERT, pr->ScrollbarAlways?TRUE:FALSE);

	// Hide unwanted UI elements

	if(pr->szOK)
	{
		SetDlgItemText(hdlg, IDOK, pr->szOK);
	}
	else
	{
		ShowWindow(GetDlgItem(hdlg, IDOK), SW_HIDE);
		SetFocus(NULL);
	}

	if(pr->procGetText==Dumb_GetText)
	{
		ShowWindow(GetDlgItem(hdlg, IDC_BTN_REFRESH), SW_HIDE); 
	}

	if(pr->msecAutoRefresh>0)
	{
		if(pr->isAutoRefreshNow)
		{
			CheckDlgButton(hdlg, IDC_CHK_AUTOREFRESH, BST_CHECKED);
			dsi_StartTimer(hdlg, pr);
		}
	}
	else
	{
		// hide []Auto chkbox
		ShowWindow(GetDlgItem(hdlg, IDC_CHK_AUTOREFRESH), SW_HIDE); 
	}


	return FALSE; // no default focus
}

void dsi_OnDestroy(HWND hwnd)
{
	DsiDlgParams_st *pr = (DsiDlgParams_st*)GetWindowLongPtr(hwnd, DWLP_USER);

	if(pr->hfontFixedWidth)
		DeleteObject(pr->hfontFixedWidth);
}

void dsi_OnSize(HWND hdlg, UINT state, int cx, int cy) 
{
	DsiDlgParams_st *pr = (DsiDlgParams_st*)GetWindowLongPtr(hdlg, DWLP_USER);

	// Reposition the child controls
	pr->jul.AdjustControls(cx, cy);

	// If user shrink the show-info window, vertical scrollbar should be displayed,
	// and if user restores show-info window original size, vertical scrollbar may need to hide.
	if(!pr->ScrollbarAlways)
	{
		RECT rectNow;
		GetWindowRect(hdlg, &rectNow);
		BOOL showsb = RECT_AnySide_A_shorter_than_B(rectNow, pr->rectNewboxVisualMax) ? TRUE : FALSE;
		ShowScrollBar(GetDlgItem(hdlg, IDC_EDIT_SHOW_INFO), SB_VERT, showsb);
	}
}


void dsi_OnGetMinMaxInfo(HWND hdlg, PMINMAXINFO pMinMaxInfo) 
{
	DsiDlgParams_st *pr = (DsiDlgParams_st*)GetWindowLongPtr(hdlg, DWLP_USER);

	// Return minimum size of dialog box
	pr->jul.HandleMinMax(pMinMaxInfo);

	pMinMaxInfo->ptMaxTrackSize.x = RECTcx(pr->rectNewboxVisualMax);
	pMinMaxInfo->ptMaxTrackSize.y = RECTcy(pr->rectNewboxVisualMax);
}

void dsi_OnTimer(HWND hwnd, UINT id)
{
	DsiDlgParams_st *pr = (DsiDlgParams_st*)GetWindowLongPtr(hwnd, DWLP_USER);

	assert(id==dlg_timer_id); (void)id;

	dsi_CallbackRefreshUserText(hwnd, pr);
}


void dsi_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify) 
{
	DsiDlgParams_st *pr = (DsiDlgParams_st*)GetWindowLongPtr(hwnd, DWLP_USER);
//	DWORD winerr;

	switch (id) 
	{{
	case IDC_BTN_REFRESH:
	{
		dsi_CallbackRefreshUserText(hwnd, pr);
		break;
	}

	case IDC_CHK_AUTOREFRESH:
	{
		UINT chkst = IsDlgButtonChecked(hwnd, IDC_CHK_AUTOREFRESH);
		if(chkst==BST_CHECKED)
		{
			dsi_CallbackRefreshUserText(hwnd, pr);
			dsi_StartTimer(hwnd, pr);
		}
		else
		{
			dsi_StopTimer(hwnd, pr);
		}
		break;
	}

	case IDOK:
	{
		pr->cb_info.isOKBtnRequested = true;
		pr->cb_info.msecOKBtnRequested = GetTickCount();
		break;
	}

	case IDCANCEL:
	{
		pr->cb_info.isCloseRequested = true;
		pr->cb_info.msecCloseRequested = GetTickCount();
		break;
	}

	default:
		break;
	}}

	if(id==IDOK || id==IDCANCEL)
	{
		if(!pr->isOnlyClosedByProgram)
			EndDialog(hwnd, Dsie_Success);
	}

}


INT_PTR CALLBACK
dsi_DlgProc(__in HWND   hwnd,
	__in UINT   uMsg,
	__in WPARAM wParam,
	__in LPARAM lParam)
{
	switch(uMsg) 
	{{
		chHANDLE_DLGMSG(hwnd, WM_INITDIALOG, dsi_OnInitDialog);
		chHANDLE_DLGMSG(hwnd, WM_DESTROY, dsi_OnDestroy);
		chHANDLE_DLGMSG(hwnd, WM_COMMAND, dsi_OnCommand);
		chHANDLE_DLGMSG(hwnd, WM_SIZE,          dsi_OnSize); // JULayout
		chHANDLE_DLGMSG(hwnd, WM_GETMINMAXINFO, dsi_OnGetMinMaxInfo); // JULayout
		chHANDLE_DLGMSG(hwnd, WM_TIMER, dsi_OnTimer);

	default: 
		return FALSE;
	}}

	return TRUE;
} 


dlg_showinfo_ret 
dlg_showinfo_t2(HINSTANCE hinstExeDll, LPCTSTR resIdDlgbox,
	HWND hwndRealParent, const dlg_showinfo_st *p_usr_opt, const TCHAR *pszInfo)
{
	dlg_showinfo_st opt;
	if(p_usr_opt)
		opt = *p_usr_opt;

	if(opt.isOnlyClosedByProgram && !opt.procGetText)
		return Dsie_OnlyClosedByProgram_but_NoCallback;

	// Set valid "defaults" for opt.

	if(opt.title==NULL)
		opt.title = _T("Showinfo");
	
	if(opt.hIcon==NULL)
		opt.hIcon = LoadIcon(NULL, IDI_INFORMATION);

	TCHAR null_char = _T('\0');
	if(!pszInfo)
	{
		pszInfo = &null_char; // better make it a writable space
		opt.bufchars = 1;
	}

	if(!opt.procGetText || opt.bufchars<=0)
	{
		opt.procGetText = Dumb_GetText;
		opt.isRefreshNow = opt.isAutoRefreshNow = false;
	}
	
	DsiDlgParams_st dsi(opt, pszInfo);

	dsi.hwndRealParent = hwndRealParent;

	INT_PTR dlgret = DialogBoxParam(hinstExeDll, resIdDlgbox,
		hwndRealParent, dsi_DlgProc, (LPARAM)&dsi);
	
	if(dlgret==-1)
		return Dsie_NoMem;
	else
		return (dlg_showinfo_ret)dlgret;
}

