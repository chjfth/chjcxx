#include "stdafx.h"

#include <_MINMAX_.h>
#include <EnsureClnup_common.h>
#include <EnsureClnup_mswin.h>
#include <AutoBuf.h>

#include <mswin/JULayout.h>
#include <mswin/CmnHdr-Jeffrey.h>

#include <gadgetlib/clipboard.h>

#include <gadgetlib/enum_monitors.h>
#include <gadgetlib/ReposNewbox.h>
#include <gadgetlib/unstraddle_dlgbox.h>
#include <gadgetlib/dlg_showinfo.h>

#include <gadgetlib/dlg_showinfo_ids.h>
#include <gadgetlib/dlg_showinfo.h>

#define DLL_AUTO_EXPORT_STUB
extern"C" void gadgetlib_lib__dlg_showinfo__DLL_AUTO_EXPORT_STUB(void){}

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
	const TCHAR *szAutoChkText;

	// Internal data for Showinfo-dialog:
	HWND hwndRealParent;
	HFONT hfontEditbox;

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
	szAutoChkText = szAutoChkText;
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

																		// LOGFONT logfontE = {0};
																		// //HFONT hFont = (HFONT)SendDlgItemMessage(hdlg, IDC_EDIT_SHOW_INFO, WM_GETFONT, 0, 0);
																		// HFONT hFont = (HFONT)SelectObject(hdcEdit, (HGDIOBJ)GetStockObject(SYSTEM_FIXED_FONT));
																		// int retbytes = GetObject(hFont, sizeof(logfontE), &logfontE);
																		// (VOID)retbytes;

	RECT drawarea = {0,0,1,1};
	SelectObject(hdcEdit, pr->hfontEditbox); // we need this to get correct draw-width
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

	// Cal draw-width & draw-height reserved for our multi-line editbox area.
	int drawwidth = RECTcx(drawarea);
	drawwidth = _MAX_(drawwidth, RECTcx(rectEdit));
	int drawheight = RECTcy(drawarea); // If a single text line, we'll see drawheight<RECTcy(rectEdit)
	drawheight = _MAX_(drawheight, RECTcy(rectEdit)); 

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

	ReposNewboxInput_st nbi = {0};
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

	const int px_torrent = 14;
	Rect_st rectNow;
	GetWindowRect(hwnd, (RECT*)&rectNow);
	//bool wasAtVisualMax = RECT_IsSameSize(rectNow, pr->rectNewboxVisualMax) ? true : false; // see secret 6px bias, give up acurate size checking
	bool cxSame = ( RECTcx(pr->rectNewboxVisualMax)-RECTcx(rectNow) <= px_torrent );
	bool cySame = ( RECTcy(pr->rectNewboxVisualMax)-RECTcy(rectNow) <= px_torrent );
	bool wasAtVisualMax = cxSame && cySame;

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

	// If dlg has been shrunk(due to user purpose), don't resize it even if we get more text.
	// So we check isGoBigger here.
	if(isGoBigger && wasAtVisualMax)
	{
		// Note: We try to keep dlgbox's left-top position here.
		// Imagine: Dlgbox pops up at initial position A, and user drags it to his preferred
		// position B for viewing. When dlgbox's text is updated, it's better to keep the box
		// at user's preferred left-top corner position(B) intact.
		//
		// And the new bigger box may be wider or higher which causes some dlgbox portion
		// go beyond its original monitor, so we adjust it again with ggt_ReposNewbox.

		Rect_st rect_new = { rectNow.left, rectNow.top, rectNow.right, rectNow.bottom };

		rect_new = ggt_unstraddle_dlgbox(rect_new, 
			RECTcx(pr->rectNewboxVisualMax), RECTcy(pr->rectNewboxVisualMax));

		MoveWindow(hwnd, rect_new.left, rect_new.top, RECTcx(rect_new), RECTcy(rect_new), TRUE);
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

	// Create a font to be used for EditBox
	LOGFONT logfont = {0};
	HFONT hFont = (HFONT)SendDlgItemMessage(hdlg, IDC_EDIT_SHOW_INFO, WM_GETFONT, 0, 0);
	int retbytes = GetObject(hFont, sizeof(logfont), &logfont);
	(void)retbytes;
	assert(retbytes==sizeof(logfont));
	logfont.lfPitchAndFamily = pr->isFixedWidthFont ? FIXED_PITCH : VARIABLE_PITCH; 
	//logfont.lfWeight = FW_BOLD;
	if(pr->isFixedWidthFont)
	{
		logfont.lfFaceName[0] = 0; // We don't know the exact fixed-width font name, so let System re-pick it.
		// Tried on Win81: If lfFaceName is "MS Shell Dlg 2" and .lfPitchAndFamily==FIXED_PITCH,
		// FIXED_PITCH does not overwrite the proportional Tahoma font.
	}
	pr->hfontEditbox = CreateFontIndirect(&logfont);
	assert(pr->hfontEditbox);

	SendDlgItemMessage(hdlg, IDC_EDIT_SHOW_INFO, WM_SETFONT, (WPARAM)(pr->hfontEditbox), TRUE);
		// Note: If using fixed-width font, it only influence the editbox, not the whole dialogbox.

	Rect_st &rectMax = pr->rectNewboxVisualMax;
	rectMax = dsi_CalNewboxTextMax(hdlg, pr);

	MoveWindow(hdlg, rectMax.left, rectMax.top, RECTcx(rectMax), RECTcy(rectMax), TRUE);

	// Hide hEdit's ugly scrollbar, because we do not need it now.
	// It will be shown later when the message box is shrunk, when processing WM_SIZE.
	HWND hEdit = GetDlgItem(hdlg, IDC_EDIT_SHOW_INFO);
	ShowScrollBar(hEdit, SB_VERT, pr->ScrollbarAlways?TRUE:FALSE);

	// Hide unwanted UI elements

	if(pr->szOK) {
		SetDlgItemText(hdlg, IDOK, pr->szOK);
	} else {
		ShowWindow(GetDlgItem(hdlg, IDOK), SW_HIDE);
		SetFocus(NULL);
	}
	
	SetDlgItemText(hdlg, IDC_BTN_REFRESH, 
		pr->szRefreshBtnText ? pr->szRefreshBtnText : _T("&Refresh")
		);
	SetDlgItemText(hdlg, IDC_CHK_AUTOREFRESH,
		pr->szAutoChkText ? pr->szAutoChkText : _T("&Auto")
		);

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

	if(pr->hfontEditbox)
		DeleteObject(pr->hfontEditbox);
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
	
	// Return maximum size of dialog box
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
	case IDI_SHOW_INFO:
	{
		TCHAR text[64000];
		GetDlgItemText(hwnd, IDC_EDIT_SHOW_INFO, text, GetEleQuan(text));
		ggt_SetClipboardText(text, -1, hwnd);
		//MessageBox(hwnd, L"copied", NULL, MB_OK);
		break;
	}

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
dsi_DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
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
in_dlg_showinfo(HINSTANCE hinstExeDll, 
	LPCTSTR resIdDlgbox, DLGTEMPLATE *pDlgTemplate,
	HWND hwndRealParent, const dlg_showinfo_st *p_usr_opt, const TCHAR *pszInfo)
{
	// Choose only one between resIdDlgbox and *pDlgTemplate.
	if(!resIdDlgbox && !pDlgTemplate)
		return Dsie_BadParam;

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

	INT_PTR dlgret = 0;
	if(resIdDlgbox)
	{
		dlgret = DialogBoxParam(hinstExeDll, resIdDlgbox,
			hwndRealParent, dsi_DlgProc, (LPARAM)&dsi);
	}
	else
	{
		dlgret = DialogBoxIndirectParam(hinstExeDll, pDlgTemplate,
			hwndRealParent, dsi_DlgProc, (LPARAM)&dsi);
	}
	
	if(dlgret==-1)
		return Dsie_NoMem;
	else
		return (dlg_showinfo_ret)dlgret;
}

#define ALIGN_TO_DWORD(pword) if((INT_PTR)(pword) & 0x3) *(pword)++ = 0;

dlg_showinfo_ret 
ggt_dlg_showinfo(HWND hwndRealParent, const dlg_showinfo_st *p_usr_opt, const TCHAR *pszInfo)
{
/*
	// NOTE: We need to manually sync the code here with DIALOGEX resource statements.

IDD_SHOW_INFO DIALOGEX 0, 0, 156, 65
STYLE DS_SETFONT | DS_FIXEDSYS | WS_MINIMIZEBOX | WS_POPUP | WS_VISIBLE | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME
CAPTION "Showinfo Dialog"
FONT 9, "MS Shell Dlg", 400, 0, 0x1
BEGIN
DEFPUSHBUTTON   "OK",IDOK,56,46,50,14
ICON            "",IDI_SHOW_INFO,14,6,20,20,SS_NOTIFY
EDITTEXT        IDC_EDIT_SHOW_INFO,47,7,102,34,ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY | NOT WS_BORDER | WS_VSCROLL
PUSHBUTTON      "&Refresh",IDC_BTN_REFRESH,6,28,36,12
CONTROL         "&Auto",IDC_CHK_AUTOREFRESH,"Button",BS_AUTOCHECKBOX | WS_TABSTOP,7,42,36,10
END
*/
	char memblock[4000];

	// Prepare DLG template header:

	DLGTEMPLATE &dt = *(DLGTEMPLATE*)memblock;
	dt.style = DS_SETFONT|DS_FIXEDSYS|WS_MINIMIZEBOX|WS_POPUP|WS_VISIBLE|WS_CAPTION|WS_SYSMENU|WS_THICKFRAME;
	dt.dwExtendedStyle = 0;
	dt.cdit = 0; // count of child controls, increase later
	dt.x=0, dt.y=0, dt.cx=156, dt.cy=65;

	// menu, class, title

	WORD *pword = (WORD*)(memblock+sizeof(DLGTEMPLATE));
	*pword++ = 0x0000; // this dlgbox has no menu

	*pword++ = 0x0000; // use system default dlgbox "window class"

	const WCHAR szTitle[] = L"ABC"; // set title to "ABC", will get update in DlgProc (WM_INITDIALOG)
	wcscpy((wchar_t*)pword, szTitle);
	pword += GetEleQuan(szTitle);

	// dialog font size and typeface
	//
	if(dt.style & DS_SETFONT)
	{
		int fontsize = p_usr_opt->fontsize>0 ? p_usr_opt->fontsize : 9;

		*pword++ = (WORD)_MAX_(6, fontsize); // minimum 6pt, but 6 will crop the icon above, why?
		const WCHAR szFontface[] = L"MS Shell Dlg 2";
			// Note: On Win7/81,
			// Using "MS Shell Dlg 2" instead of "MS Shell Dlg", we get the same result as
			// using .rc-created dialog. That's Microsoft's trick, don't know why.
		wcscpy((WCHAR*)pword, szFontface);
		pword += GetEleQuan(szFontface);
	}

	// DLGITEMTEMPLATE elements

	ALIGN_TO_DWORD(pword);

	DLGITEMTEMPLATE *pitem = NULL;

	// [OK] button
	//
	pitem = (DLGITEMTEMPLATE*)pword;
	++dt.cdit;
	pitem->style = WS_VISIBLE|WS_TABSTOP;
	pitem->dwExtendedStyle = 0;
	pitem->x=56, pitem->y=46, pitem->cx=50, pitem->cy=14;
	pitem->id = IDOK;
	//
	pword = (WORD*)(pitem+1);
	*pword++ = 0xFFFF; *pword++ = 0x0080; // ctrl class is "button"
	wcscpy((wchar_t*)pword, L"-"); pword+=2; // button text, change later in DlgProc
	*pword++ = 0x0000; // no creation data

	ALIGN_TO_DWORD(pword);

	// the icon
	//
	pitem = (DLGITEMTEMPLATE*)pword;
	++dt.cdit;
	pitem->style = WS_VISIBLE|SS_ICON|SS_NOTIFY; // | SS_BLACKFRAME;
	pitem->dwExtendedStyle = 0;
	pitem->x=14, pitem->y=6, pitem->cx=20, pitem->cy=20;
	pitem->id = IDI_SHOW_INFO;
	//
	pword = (WORD*)(pitem+1);
	*pword++ = 0xFFFF; *pword++ = 0x0082; // ctrl class is "static"
	*pword++ = 0x0000; // no initial text or icon, set later in WM_INITDIALOG
	*pword++ = 0x0000; // no creation data

	ALIGN_TO_DWORD(pword);

	// the edit box
	//
	pitem = (DLGITEMTEMPLATE*)pword;
	++dt.cdit;
	pitem->style = WS_VISIBLE|ES_MULTILINE|ES_AUTOVSCROLL|ES_READONLY|WS_VSCROLL|WS_TABSTOP;
	pitem->dwExtendedStyle = 0;
	pitem->x=47, pitem->y=7, pitem->cx=102, pitem->cy=34;
	pitem->id = IDC_EDIT_SHOW_INFO;
	//
	pword = (WORD*)(pitem+1);
	*pword++ = 0xFFFF; *pword++ = 0x0081; // ctrl class is "edit"
	*pword++ = 0x0000; // no initial text, set later in WM_INITDIALOG
	*pword++ = 0x0000; // no creation data

	ALIGN_TO_DWORD(pword);

	// the [Refresh] button
	//
	pitem = (DLGITEMTEMPLATE*)pword;
	++dt.cdit;
	pitem->style = WS_VISIBLE|WS_TABSTOP;
	pitem->dwExtendedStyle = 0;
	pitem->x=6, pitem->y=28, pitem->cx=36, pitem->cy=12;
	pitem->id = IDC_BTN_REFRESH;
	//
	pword = (WORD*)(pitem+1);
	*pword++ = 0xFFFF; *pword++ = 0x0080; // ctrl class is "button"
	wcscpy((wchar_t*)pword, L"-"); pword+=2; // button text, change later in DlgProc
	*pword++ = 0x0000; // no creation data

	ALIGN_TO_DWORD(pword);

	// the [Auto] check box
	//
	pitem = (DLGITEMTEMPLATE*)pword;
	++dt.cdit;
	pitem->style = WS_VISIBLE|BS_AUTOCHECKBOX|WS_TABSTOP;
	pitem->dwExtendedStyle = 0;
	pitem->x=7, pitem->y=42, pitem->cx=36, pitem->cy=10;
	pitem->id = IDC_CHK_AUTOREFRESH;
	//
	pword = (WORD*)(pitem+1);
	*pword++ = 0xFFFF; *pword++ = 0x0080; // ctrl class is "button"
	wcscpy((wchar_t*)pword, L"-"); pword+=2; // button text, change later in DlgProc
	*pword++ = 0x0000; // no creation data

	HINSTANCE hinstExe = (HINSTANCE)GetWindowLongPtr(hwndRealParent, GWLP_HINSTANCE);
	return in_dlg_showinfo(hinstExe, NULL, &dt, hwndRealParent, p_usr_opt, pszInfo);
}

dlg_showinfo_ret 
ggt_dlg_showinfo_userc(HINSTANCE hinstExeDll, LPCTSTR resIdDlgbox,
	HWND hwndRealParent, const dlg_showinfo_st *p_usr_opt, const TCHAR *pszInfo)
{
	return in_dlg_showinfo(hinstExeDll, resIdDlgbox, NULL, hwndRealParent, p_usr_opt, pszInfo);
}
