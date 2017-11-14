#include "stdafx.h"

#include <_MINMAX_.h>
#include <TScalableArray.h>
#include <EnsureClnup_common.h>
#include <EnsureClnup_mswin.h>
#include <AutoBuf.h>

#include <mswin/JULayout.h>
#include <mswin/CmnHdr-Jeffrey.h>

#include <gadgetlib/clipboard.h>
#include <gadgetlib/timefuncs.h>
#include <gadgetlib/enum_monitors.h>
#include <gadgetlib/ReposNewbox.h>
#include <gadgetlib/unstraddle_dlgbox.h>
#include <gadgetlib/wintooltip.h>
#include <gadgetlib/textcrlf.h>

#include <gadgetlib/FlexiInfobox_ids.h>
#include <gadgetlib/FlexiInfobox.h>

#define DLL_AUTO_EXPORT_STUB
extern"C" void gadgetlib_lib__dlg_showinfo__DLL_AUTO_EXPORT_STUB(void){}

const int timerId_AutoRefresh = 1;
const int timerId_AllowClose = 2;
const int timerId_HideTooltip = 3;

static const POINT ptTooltipCorner = {4, -4}; // lower-left corner

inline bool FIBcb__IsFail(FibCallback_ret ret)
{
	if(ret==FIBcb_Fail || ret==FIBcb_FailIcon || ret==FIBcb_Fail_StopAutoRefresh || ret==FIBcb_FailIcon_StopAutoRefresh)
		return true;
	else
		return false;
}

inline bool FIBcb__IsSetFailIcon(FibCallback_ret ret)
{
	if(ret==FIBcb_FailIcon || ret==FIBcb_FailIcon_StopAutoRefresh)
		return true;
	else
		return false;
}

inline bool FIBcb__IsStopAutoRefresh(FibCallback_ret ret)
{
	if(ret==FIBcb_Fail_StopAutoRefresh || ret==FIBcb_FailIcon_StopAutoRefresh)
		return true;
	else
		return false;
}

#define HideWindow(hwnd) ShowWindow((hwnd), SW_HIDE)

static void Hide_DlgItem(HWND hwnd, int idCtl)
{
	HWND hctl = GetDlgItem(hwnd, idCtl);
	assert(hctl);
	ShowWindow(hctl, SW_HIDE);
}

// enum FibAgain_et // whether to display infobox again
// {
// 	FibNoAgain = 0,
// 	FibAgainAttached = 1,
// 	FibAgainDetached = 2, // use NULL as owner
// };

struct FibDlgParams_st
{
	// Input params from caller:
	const TCHAR *input_title;
	TCHAR *textbuf; // can use \n as line-separator
	int bufchars;
	HICON hIconUser;
	HICON hIconFail;
	bool isFixedWidthFont;
	//
	PROC_FibCallback_GetText procGetText;
	void *ctxGetText;
	//
	bool isShowRefreshBtn;
	int msecAutoRefresh; // non-zero will enable auto-refresh feature
	bool isAutoRefreshNow; 
	//
	// user can customize button text
	const TCHAR *szBtnOK;
	const TCHAR *szBtnCancel;
	const TCHAR *szBtnRefresh; 
	const TCHAR *szAutoChkbox;

	int idDefaultFocus;
	bool isNarrowTitle;
	bool isScrollToEnd;
	bool isForceHideRefreshCtrl;

	//
	// Internal data for Showinfo-dialog below:
	//

	HWND hwndOwnerFibIn; // Fib user passed in HWND
	HWND hwndRealOwner;  // may equal to hwndOwnerFibin or NULL.

	HFONT hfontEditbox;

	Rect_st rectNewboxVisualMax; 
		// This Rect-size is enough to hold all our info text.
		// This Rect will inflate along with user text increasing, not deflating.
		// The rect is from GetWindowRect.
	bool ScrollbarAlways;

	bool isTimerOn;
	HICON hIconNow; // may switch between hIcon and an "error icon"

	FibCallback_st cb_info;

	DWORD msecDelayClose;
	DWORD tkmsecStart;

	__int64 uesecLastText; // the time recent text got updated(FIBcb_OK from user callback)

	FibUserCmds_st *arUserCmds;
	int nUserCmds; // If any UserCmds, right-click context menu will appear

	bool isLastTextTimeOnTitle;

	int secTooltip;
	const TCHAR *szTooltipText;
	TooltipHandle_gt hTooltip;
	bool isTooltipShowing;

//	FibAgain_et again;

	JULayout jul; 

	TScalableArray<TCHAR> m_saNormText; // Text here will have \r\n as line-separator.

public:
	FibDlgParams_st(const FibInput_st &in, HWND hwndOwner, const TCHAR *pszInfo); // called in in_FlexiInfobox()

	void SetCustomFocus(HWND hdlg);
	void SetMyText(HWND hdlg);

	void HideTooltip();
};

static void FixDefaultOnOffCmd(FIBcmdstate_et &cmdState, FIBcmdstate_et default_val)
{
	if( cmdState!=FIBcst_TickOn && cmdState!=FIBcst_TickOff )
		cmdState = default_val; 
}

FibDlgParams_st::FibDlgParams_st(const FibInput_st &in, HWND hwndOwner, const TCHAR *pszInfo)
{
	memset(this, 0, sizeof(*this));

	input_title = in.title;
	textbuf = (TCHAR*)pszInfo;
	bufchars = in.bufchars;
	hIconUser = in.hIcon;
	hIconFail = in.hIconFail;
	isFixedWidthFont = in.fixedwidth_font;
	//
	procGetText = in.procGetText;
	ctxGetText = in.ctxGetText;
	//
	isShowRefreshBtn = in.isShowRefreshBtn;
	msecAutoRefresh = in.msecAutoRefresh;
	isAutoRefreshNow = in.isAutoRefreshNow;
	//
	msecDelayClose = in.msecDelayClose;
	tkmsecStart = 0;
	//
	szBtnOK = in.szBtnOK;
	szBtnCancel = in.szBtnCancel;
	szBtnRefresh = in.szBtnRefresh;
	szAutoChkbox = in.szAutoChkbox;
	//
	idDefaultFocus = in.idDefaultFocus;
	isNarrowTitle = in.isNarrowTitle;
	isScrollToEnd = in.isScrollToEnd;
	isForceHideRefreshCtrl = in.isForceHideRefreshCtrl;
	//
	arUserCmds = in.arUserCmds;
	nUserCmds = in.nUserCmds;
	//
	secTooltip = in.secTooltip;
	szTooltipText = in.szTooltipText;
	hTooltip = NULL;

	hwndOwnerFibIn = hwndOwner;
	hwndRealOwner = in.isInitDetached ? NULL : hwndOwner;

	int i;
	for(i=0; i<nUserCmds; i++)
	{
		FibUserCmds_st &ucmd = arUserCmds[i];

		// fix erroneous/irrational/dangling params for user input

		if(ucmd.idCmd==FIBcmd_CopyText)
			ucmd.cmdState = FIBcst_Raw;

		if(ucmd.idCmd==FIBcmd_LastTextTimeOnTitle)
		{
			FixDefaultOnOffCmd(ucmd.cmdState, FIBcst_TickOff);

			isLastTextTimeOnTitle = ucmd.cmdState==FIBcst_TickOn ? true : false;
		}

		if(ucmd.idCmd==FIBcmd_DetachFromParent)
		{
			if(hwndOwnerFibIn==NULL)
			{
				// No choice. We can only present a detached infobox.
				ucmd.cmdState = FIBcst_Invalid;
			}
			else
			{
				ucmd.cmdState = hwndRealOwner ? FIBcst_TickOff : FIBcst_TickOn;
			}
		}
	}

	m_saNormText.Init(FlexiInfobox_TextMax, 84, 2048, 4096);
}

void FibDlgParams_st::SetCustomFocus(HWND hdlg)
{
	HWND hctlOK = GetDlgItem(hdlg, IDC_BTN_OK);
	HWND hctlCancel = GetDlgItem(hdlg, IDC_BTN_CANCEL);

	if(!idDefaultFocus)
	{
		if(szBtnOK)
			idDefaultFocus = IDC_BTN_OK;
		else if(szBtnCancel)
			idDefaultFocus = IDC_BTN_CANCEL;
	}

	if(idDefaultFocus)
	{
		HWND hFocus = GetDlgItem(hdlg, idDefaultFocus);
		SetFocus(hFocus);

		// and set default-push-button as well
		//
		HWND hDefaultPushBtn = NULL;
		if(idDefaultFocus==IDC_BTN_OK)
			hDefaultPushBtn = hctlOK;
		else if(idDefaultFocus==IDC_BTN_CANCEL)
			hDefaultPushBtn = hctlCancel;

		if(hDefaultPushBtn)
		{
			UINT style = GetWindowLong(hDefaultPushBtn, GWL_STYLE);
			SetWindowLong(hDefaultPushBtn, GWL_STYLE, style|BS_DEFPUSHBUTTON);
		}
	}
}

template<typename T>
bool Is_TsaErr(T err) // change to IsTsaErr for new common-include 
{ 
	return err ? true : false; 
}

void FibDlgParams_st::SetMyText(HWND hdlg)
{
	assert(textbuf);

	HWND hEdit = GetDlgItem(hdlg, IDC_EDIT_SHOW_INFO);
	assert(hEdit);
	
	int nspace = m_saNormText.CurrentEles();
	int nreq_ = 1 + ggt_normalize_crlf(textbuf, m_saNormText, nspace, _T("\r\n"));
	if(nreq_>nspace)
	{
		if( Is_TsaErr(m_saNormText.SetEleQuan(nreq_)) )
		{
			mm_snprintf(textbuf, bufchars, _T("No memory!"));
			return;
		}
		ggt_normalize_crlf(textbuf, m_saNormText, nspace, _T("\r\n"));
	}

	SetWindowText(hEdit, m_saNormText);

	if(isScrollToEnd)
	{
		int lines = Edit_GetLineCount(hEdit);
		SNDMSG(hEdit, EM_LINESCROLL, 0, lines);
	}
}

void FibDlgParams_st::HideTooltip()
{
	ggt_TooltipShow(hTooltip, false);
	isTooltipShowing = false;
}

static void 
fib_SetIcon(HWND hwnd, HICON hNewIcon, FibDlgParams_st *pr)
{
	if(hNewIcon==pr->hIconNow)
		return; // no need to redraw the icon

	pr->hIconNow = hNewIcon; // remember to avoid redundant Static_SetIcon call

	HWND hctlIcon = GetDlgItem(hwnd, IDI_SHOW_INFO);
	Static_SetIcon(hctlIcon, pr->hIconNow);
}

static void 
fib_StartTimer(HWND hwnd, FibDlgParams_st *pr)
{
	if(pr->isTimerOn)
		return;

	UINT_PTR succ = SetTimer(hwnd, timerId_AutoRefresh, pr->msecAutoRefresh, NULL);
	if(succ)
		pr->isTimerOn = true;
}

static void 
fib_StopTimer(HWND hwnd, FibDlgParams_st *pr)
{
	if(pr->isTimerOn==false)
		return;

	KillTimer(hwnd, timerId_AutoRefresh);
	pr->isTimerOn = false;
}

static Rect_st 
fib_CalNewboxTextMax(HWND hdlg, FibDlgParams_st *pr, Size_st *pIdealDrawsize=NULL)
{
	// Determine display-area of the input string(probably multi-line), 
	// then we know(return) the corresponding dialogbox-window size.
	//
	// The return size is not of the imaginary-ideal draw-size, but the expected real-visual size.

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

	if(pIdealDrawsize)
		pIdealDrawsize->cx = RECTcx(drawarea), pIdealDrawsize->cy = RECTcy(drawarea);

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
	if(pr->hwndRealOwner)
		GetWindowRect(pr->hwndRealOwner, (RECT*)&nbi.rectParent);
	else {
		POINT pt;
		GetCursorPos(&pt);
		SetRect((RECT*)&nbi.rectParent, pt.x, pt.y, pt.x, pt.y);
	}
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
fib_UpdateDlgTitle(HWND hdlg, const FibDlgParams_st *pr)
{
	if(pr->isLastTextTimeOnTitle) // todo: move to a function
	{
		struct tm t;
		ggt_localtime(pr->uesecLastText, &t);
		TCHAR sztitle[40];
		mm_snprintf(sztitle, GetEleQuan(sztitle), _T("%04d-%02d-%02d %02d:%02d:%02d"),
			t.tm_year+1900, t.tm_mon+1, t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec);
		SetWindowText(hdlg, sztitle);
	}
	else
	{
		SetWindowText(hdlg, pr->input_title);
	}
}

static FibCallback_ret  
fib_CallbackFetchUserText(HWND hdlg, FibCallbackReason_et reason, FibDlgParams_st *pr, 
	bool isAdjustWindowNow, int idUserCmd=0, bool isTickOn=false)
{
	pr->cb_info.hDlg = hdlg;
	pr->cb_info.reason = reason;
	pr->cb_info.idUserCmd = 0;
	if(reason==FIBReason_UserCmd)
	{
		pr->cb_info.idUserCmd = idUserCmd;
		pr->cb_info.isTickOn = isTickOn;
	}
	pr->cb_info.ret_replace_offset = 0;
	pr->cb_info.isAutoRefreshOn = IsDlgButtonChecked(hdlg, IDC_CHK_AUTOREFRESH)?true:false;

	FibCallback_ret ret = pr->procGetText(pr->ctxGetText, pr->cb_info,
		pr->textbuf, pr->bufchars); 
		// Will update pr->textbuf[]
	
	if(ret==FIBcb_OK_NoChange)
	{
		// pr->SetMyText(hdlg);
		return ret;
	}
	else if(ret==FIBcb_OK)
	{
		fib_SetIcon(hdlg, pr->hIconUser, pr);
	}
	else if(ret==FIBcb_CloseDlg)
	{
		EndDialog(hdlg, FIB_Success);
	}
	else 
	{
		if(FIBcb__IsSetFailIcon(ret))
			fib_SetIcon(hdlg, pr->hIconFail, pr);

		if(FIBcb__IsStopAutoRefresh(ret))
		{
			fib_StopTimer(hdlg, pr);
			CheckDlgButton(hdlg, IDC_CHK_AUTOREFRESH, BST_UNCHECKED);
		}
	}

	pr->uesecLastText = ggt_time64();

	fib_UpdateDlgTitle(hdlg, pr);

	if(!isAdjustWindowNow)
	{
		// pr->SetMyText(hdlg); // seems un-necessary
		return ret;
	}

	const int px_torrent = 14;
	Rect_st rectNow;
	GetWindowRect(hdlg, (RECT*)&rectNow);
	//bool wasAtVisualMax = RECT_IsSameSize(rectNow, pr->rectNewboxVisualMax) ? true : false; // see secret 6px bias, give up accurate size checking
	bool cxSame = ( RECTcx(pr->rectNewboxVisualMax)-RECTcx(rectNow) <= px_torrent );
	bool cySame = ( RECTcy(pr->rectNewboxVisualMax)-RECTcy(rectNow) <= px_torrent );
	bool wasAtVisualMax = cxSame && cySame;

	bool isGoBigger = false;
	Size_st idealEditSize = {0};
	Rect_st rectTextMax = fib_CalNewboxTextMax(hdlg, pr, &idealEditSize); // return Dlg rect, not Editbox's

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

		MoveWindow(hdlg, rect_new.left, rect_new.top, RECTcx(rect_new), RECTcy(rect_new), TRUE);
	}

	// Re-check to see whether we need to re-show scrollbar
	HWND hEdit = GetDlgItem(hdlg, IDC_EDIT_SHOW_INFO); assert(hEdit);
	RECT rectEditboxNow;
	GetWindowRect(hEdit, &rectEditboxNow);
	if( idealEditSize.cy>RECTcy(rectEditboxNow) || idealEditSize.cx>RECTcx(rectEditboxNow) )
		ShowScrollBar(hEdit, SB_VERT, TRUE);

	pr->SetMyText(hdlg);

	return ret;
}

static FibCallback_ret 
fib_Dumb_GetText(void *ctx, const FibCallback_st &cb_info, TCHAR *textbuf, int bufchars)
{
	(void)ctx; (void)cb_info; (void)textbuf; (void)bufchars;
	return FIBcb_OK;
}


BOOL fib_OnInitDialog(HWND hdlg, HWND hwndFocus, LPARAM lParam) 
{
	BOOL b = 0;
	FibDlgParams_st *pr = (FibDlgParams_st*)lParam;
	SetWindowLongPtr(hdlg, DWLP_USER, (LONG_PTR)pr); // save our dlg private data

	// Hide unwanted UI elements (must do this before JUL)

	HWND hctlOK = GetDlgItem(hdlg, IDC_BTN_OK);
	HWND hctlCancel = GetDlgItem(hdlg, IDC_BTN_CANCEL);
	assert(hctlOK && hctlCancel);

	if(pr->szBtnOK)
		SetWindowText(hctlOK, pr->szBtnOK);
	else
		HideWindow(hctlOK);
	
	if(pr->szBtnCancel)
		SetWindowText(hctlCancel, pr->szBtnCancel);
	else
		HideWindow(hctlCancel);

	if(!pr->szBtnOK && !pr->szBtnCancel)
		SetFocus(NULL);

	JULayout &jul = pr->jul;
	jul.Initialize(hdlg);
	//
	jul.AnchorControl(0, 0, 100, 100, IDC_EDIT_SHOW_INFO); 
	//
	if(pr->szBtnOK && pr->szBtnCancel)
	{	// place OK & Cancel both at right-bottom corner
		jul.AnchorControl(100, 100, 100, 100, IDC_BTN_OK);
		jul.AnchorControl(100, 100, 100, 100, IDC_BTN_CANCEL);
	}
	else if(pr->szBtnOK || pr->szBtnCancel)
	{	// place the single button at middle-bottom
		HWND hBtn = pr->szBtnOK ? hctlOK : hctlCancel;
		RECT rectDlg, rectBtn;
		b = GetClientRect(hdlg, &rectDlg);
		assert(b);
		b = GetClientRect(hBtn, &rectBtn);
		assert(b);
		int xBtn = (RECTcx(rectDlg)-RECTcx(rectBtn))/2;
		//
		POINT ptBtn = {0, 0};
		MapWindowPoints(hBtn, hdlg, (LPPOINT)&ptBtn, 1);
		int yBtn = ptBtn.y;
		MoveWindow(hctlOK, xBtn, yBtn, RECTcx(rectBtn), RECTcy(rectBtn), FALSE);

		jul.AnchorControl(50, 100, 50, 100, IDOK);
	}

	// Set sysmenu icon
	HWND hctlIcon = GetDlgItem(hdlg, IDI_SHOW_INFO);
	Static_SetIcon(hctlIcon, pr->hIconUser);
	if(pr->hIconUser)
	{
		SendMessage(hdlg, WM_SETICON, TRUE,  (LPARAM)pr->hIconUser);
		SendMessage(hdlg, WM_SETICON, FALSE, (LPARAM)pr->hIconUser);
	}

	SetWindowText(hdlg, pr->input_title);

	if(pr->isAutoRefreshNow)
		fib_CallbackFetchUserText(hdlg, FIBReason_Timer, pr, false); // pr->textbuf[] filled
	
	pr->SetMyText(hdlg); // SetDlgItemText(hdlg, IDC_EDIT_SHOW_INFO, pr->textbuf);

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
	rectMax = fib_CalNewboxTextMax(hdlg, pr);

	MoveWindow(hdlg, rectMax.left, rectMax.top, RECTcx(rectMax), RECTcy(rectMax), TRUE);

	// Hide hEdit's ugly scrollbar, because we do not need it now.
	// It will be shown later when the message box is shrunk, when processing WM_SIZE.
	HWND hEdit = GetDlgItem(hdlg, IDC_EDIT_SHOW_INFO);
	ShowScrollBar(hEdit, SB_VERT, pr->ScrollbarAlways?TRUE:FALSE);

	SetDlgItemText(hdlg, IDC_BTN_REFRESH, 
		pr->szBtnRefresh ? pr->szBtnRefresh : _T("&Refresh")
		);
	SetDlgItemText(hdlg, IDC_CHK_AUTOREFRESH,
		pr->szAutoChkbox ? pr->szAutoChkbox : _T("&Auto")
		);

	if(! (pr->isShowRefreshBtn || pr->msecAutoRefresh>0) )
	{
		Hide_DlgItem(hdlg, IDC_BTN_REFRESH); 
	}

	if(pr->msecAutoRefresh>0) // user need auto-refresh feature
	{
		if(pr->isAutoRefreshNow)
		{
			CheckDlgButton(hdlg, IDC_CHK_AUTOREFRESH, BST_CHECKED);
			fib_StartTimer(hdlg, pr);
		}
		else
		{
			// User leaves the []Auto chkbox unchecked initially
		}
	}
	else
	{
		// hide []Auto chkbox
		Hide_DlgItem(hdlg, IDC_CHK_AUTOREFRESH);
	}

	SetFocus(NULL);

	if(pr->msecDelayClose>0)
	{
		pr->tkmsecStart = GetTickCount();
		EnableWindow(GetDlgItem(hdlg, IDOK), FALSE);
		EnableWindow(GetDlgItem(hdlg, IDCANCEL), FALSE);

		SetTimer(hdlg, timerId_AllowClose, pr->msecDelayClose, NULL);
	}
	else
	{
		pr->SetCustomFocus(hdlg);
	}

	if(pr->isNarrowTitle)
	{
		LONG_PTR exstyle = GetWindowLongPtr(hdlg, GWL_EXSTYLE);
		SetWindowLongPtr(hdlg, GWL_EXSTYLE, exstyle|WS_EX_TOOLWINDOW);

		LONG_PTR style = GetWindowLongPtr(hdlg, GWL_STYLE);
		SetWindowLongPtr(hdlg, GWL_STYLE, style &  ~WS_SYSMENU); // turn off the small close nib
	}

	if(pr->isForceHideRefreshCtrl)
	{
		Hide_DlgItem(hdlg, IDC_BTN_REFRESH);
		Hide_DlgItem(hdlg, IDC_CHK_AUTOREFRESH);
	}

	if(pr->nUserCmds>0 && pr->secTooltip>=0)
	{
		if(pr->secTooltip==0)
			pr->secTooltip = 2; // default to show 2 seconds

		pr->hTooltip = ggt_CreateManualTooltip(hdlg, true);
		ggt_TooltipShow(pr->hTooltip, true, &ptTooltipCorner, 
			pr->szTooltipText ? pr->szTooltipText : _T("Right-click blank area to get context menu.")
			);
		pr->isTooltipShowing = true;

		SetTimer(hdlg, timerId_HideTooltip, 1000*pr->secTooltip, NULL);
	}

	return FALSE; // will customize the focus
}

void fib_OnDestroy(HWND hwnd)
{
	FibDlgParams_st *pr = (FibDlgParams_st*)GetWindowLongPtr(hwnd, DWLP_USER);

	ggt_TooltipDelete(pr->hTooltip);

	fib_StopTimer(hwnd, pr); 
		// key purpose: to set pr->isTimerOn=false
		// so that a re-displayed detached/attached infobox will start from a reset value.

	if(pr->hfontEditbox)
		DeleteObject(pr->hfontEditbox);
}

void fib_OnSize(HWND hdlg, UINT state, int cx, int cy) 
{
	FibDlgParams_st *pr = (FibDlgParams_st*)GetWindowLongPtr(hdlg, DWLP_USER);

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


void fib_OnGetMinMaxInfo(HWND hdlg, PMINMAXINFO pMinMaxInfo) 
{
	FibDlgParams_st *pr = (FibDlgParams_st*)GetWindowLongPtr(hdlg, DWLP_USER);

	// Return minimum size of dialog box
	pr->jul.HandleMinMax(pMinMaxInfo);
	
	// Return maximum size of dialog box
	pMinMaxInfo->ptMaxTrackSize.x = RECTcx(pr->rectNewboxVisualMax);
	pMinMaxInfo->ptMaxTrackSize.y = RECTcy(pr->rectNewboxVisualMax);
}

void fib_OnTimer(HWND hwnd, UINT id)
{
	FibDlgParams_st *pr = (FibDlgParams_st*)GetWindowLongPtr(hwnd, DWLP_USER);

	if(id==timerId_AutoRefresh)
	{
		fib_CallbackFetchUserText(hwnd, FIBReason_Timer, pr, true);
	}
	else if(id==timerId_AllowClose)
	{
		HWND hctlOK = GetDlgItem(hwnd, IDC_BTN_OK);
		HWND hctlCancel = GetDlgItem(hwnd, IDC_BTN_CANCEL);
		EnableWindow(hctlOK, TRUE);
		EnableWindow(hctlCancel, TRUE);
		KillTimer(hwnd, timerId_AllowClose);

		pr->SetCustomFocus(hwnd);
	}
	else if(id==timerId_HideTooltip)
	{
		pr->HideTooltip();
	}
	else 
		assert(0);
}

static void fib_CopyToClipboard(HWND hdlg)
{
	TCHAR text[64000];
	text[GetEleQuan(text)-1] = _T('\0');
	GetDlgItemText(hdlg, IDC_EDIT_SHOW_INFO, text, GetEleQuan(text));
	ggt_SetClipboardText(text, -1, hdlg);
}

void fib_OnMove(HWND hdlg, int x, int y)
{
	FibDlgParams_st *pr = (FibDlgParams_st*)GetWindowLongPtr(hdlg, DWLP_USER);
	if(pr->isTooltipShowing)
	{
		ggt_TooltipShow(pr->hTooltip, true, &ptTooltipCorner);
	}
}

void fib_LButtonDown(HWND hdlg, BOOL fDoubleClick, int x, int y, UINT keyFlags)
{
	(void)fDoubleClick;
	FibDlgParams_st *pr = (FibDlgParams_st*)GetWindowLongPtr(hdlg, DWLP_USER);
	pr->HideTooltip();
}

void fib_RButtonDown(HWND hdlg, BOOL fDoubleClick, int x, int y, UINT keyFlags)
{
	(void)fDoubleClick;
	FibDlgParams_st *pr = (FibDlgParams_st*)GetWindowLongPtr(hdlg, DWLP_USER);
	pr->HideTooltip();

	if(pr->nUserCmds<=0)
		return;

	Cec_DestroyMenu hmenu = CreatePopupMenu();
	int i;
	for(i=0; i<pr->nUserCmds; i++)
	{
		FibUserCmds_st &ucmd = pr->arUserCmds[i];

		if(ucmd.cmdState==FIBcst_Invalid)
			continue;

		if(ucmd.idCmd==FIBcmd_MenuSeparator)
			AppendMenu(hmenu, MF_SEPARATOR, NULL,NULL);
		else
			AppendMenu(hmenu, MF_STRING, ucmd.idCmd, ucmd.cmdText);

		// determine whether to place a check-mark on this menu-item.
		if(ucmd.cmdState==FIBcst_TickOn)
			CheckMenuItem(hmenu, ucmd.idCmd, MF_BYCOMMAND|MF_CHECKED);
		else
			CheckMenuItem(hmenu, ucmd.idCmd, MF_BYCOMMAND|MF_UNCHECKED);
	}

	bool isTickOn = false;
	POINT pos = {x,y};
	ClientToScreen(hdlg, &pos);
	int retcmd = TrackPopupMenu(hmenu, TPM_RETURNCMD, pos.x, pos.y, 0, hdlg, NULL);

	// Identify the idCmd GUI-user just selected, then do check-mark toggling.
	for(i=0; i<pr->nUserCmds; i++)
	{
		FibUserCmds_st &ucmd = pr->arUserCmds[i];
		if(ucmd.idCmd==retcmd && ucmd.cmdState!=FIBcst_Raw)
		{
			if(ucmd.cmdState==FIBcst_TickOff)
			{
				ucmd.cmdState = FIBcst_TickOn;
				isTickOn = true;
			}
			else
			{
				ucmd.cmdState = FIBcst_TickOff;
				isTickOn = false;
			}
		}
	}

	if(retcmd==FIBcmd_CopyText)
	{
		fib_CopyToClipboard(hdlg);
	}
	else if(retcmd==FIBcmd_LastTextTimeOnTitle)
	{
		pr->isLastTextTimeOnTitle = isTickOn;
		fib_UpdateDlgTitle(hdlg, pr);
	}
	else if(retcmd==FIBcmd_DetachFromParent) // note: this is a toggle value
	{
		assert(pr->hwndOwnerFibIn);
		
		// toggle the 'again' value
//		pr->again = pr->hwndRealOwner ? FibAgainDetached : FibAgainAttached;
		EndDialog(hdlg, pr->hwndRealOwner ? FIB_AgainDetached : FIB_AgainAttached);
	}

	// Yes, call user callback for FIBcmd_CopyInfo etc as well.
	fib_CallbackFetchUserText(hdlg, FIBReason_UserCmd, pr, true, retcmd, isTickOn);
}

void fib_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify) 
{
	FibDlgParams_st *pr = (FibDlgParams_st*)GetWindowLongPtr(hwnd, DWLP_USER);
//	DWORD winerr;

	switch (id) 
	{{
	case IDI_SHOW_INFO:
	{
		fib_CopyToClipboard(hwnd);
		//MessageBox(hwnd, L"copied", NULL, MB_OK);
		break;
	}

	case IDC_BTN_REFRESH:
	{
		fib_CallbackFetchUserText(hwnd, FIBReason_RefreshBtn, pr, true);
		break;
	}

	case IDC_CHK_AUTOREFRESH:
	{
		UINT chkst = IsDlgButtonChecked(hwnd, IDC_CHK_AUTOREFRESH);
		if(chkst==BST_CHECKED)
		{
			fib_CallbackFetchUserText(hwnd, FIBReason_Timer, pr, true);
			fib_StartTimer(hwnd, pr);
		}
		else
		{
			fib_StopTimer(hwnd, pr);
		}
		break;
	}

	case IDC_BTN_OK:
	{
		pr->cb_info.isOKBtnRequested = true;
		pr->cb_info.msecOKBtnRequested = GetTickCount();
		break;
	}

	case IDC_BTN_CANCEL:
	{
		pr->cb_info.isCancelRequested = true;
		pr->cb_info.msecCancelRequested = GetTickCount();
		break;
	}

	default:
		break;
	}}

	if(id==IDC_BTN_OK || id==IDC_BTN_CANCEL)
	{
		FibCallback_ret cbret = fib_CallbackFetchUserText(hwnd, 
			id==IDOK ? FIBReason_OKBtn : FIBReason_CancelBtn, 
			pr, true);

		if(FIBcb__IsFail(cbret))
		{
			return; // No calling EndDialog()
		}

		if(  pr->msecDelayClose>0 &&
			(GetTickCount() - pr->tkmsecStart < pr->msecDelayClose) )
		{
			Beep(500, 500); // alert user "close action in vain"
			return;
		}

		if(id==IDC_BTN_OK)
			EndDialog(hwnd, FIB_OK);
		else
			EndDialog(hwnd, FIB_Cancel);
	}

}


INT_PTR CALLBACK
fib_DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg) 
	{{
		chHANDLE_DLGMSG(hwnd, WM_INITDIALOG, fib_OnInitDialog);
		chHANDLE_DLGMSG(hwnd, WM_DESTROY, fib_OnDestroy);
		chHANDLE_DLGMSG(hwnd, WM_COMMAND, fib_OnCommand);
		chHANDLE_DLGMSG(hwnd, WM_SIZE,          fib_OnSize); // JULayout
		chHANDLE_DLGMSG(hwnd, WM_GETMINMAXINFO, fib_OnGetMinMaxInfo); // JULayout
		chHANDLE_DLGMSG(hwnd, WM_TIMER, fib_OnTimer);
		chHANDLE_DLGMSG(hwnd, WM_LBUTTONDOWN, fib_LButtonDown);
		chHANDLE_DLGMSG(hwnd, WM_RBUTTONDOWN, fib_RButtonDown);
		chHANDLE_DLGMSG(hwnd, WM_MOVE, fib_OnMove);

	default: 
		return FALSE;
	}}

	return TRUE;
} 


FIB_ret 
in_FlexiInfobox(HINSTANCE hinstExeDll, 
	LPCTSTR resIdDlgbox, DLGTEMPLATE *pDlgTemplate,
	HWND hwndOwner, const FibInput_st *p_usr_opt, const TCHAR *pszInfo)
{
	// Choose only one between resIdDlgbox and *pDlgTemplate.
	if(!resIdDlgbox && !pDlgTemplate)
		return FIB_BadParam;

	FibInput_st opt;
	if(p_usr_opt)
		opt = *p_usr_opt;

// 	if(opt.isOnlyClosedByProgram && !opt.procGetText)
// 		return FIB_OnlyClosedByProgram_but_NoCallback;

	// Set valid "defaults" for opt.

	if(opt.title==NULL)
		opt.title = _T("FlexiInfo");
	
	if(opt.hIcon==NULL)
		opt.hIcon = LoadIcon(NULL, IDI_INFORMATION);
	if(opt.hIconFail==NULL)
		opt.hIconFail = LoadIcon(NULL, IDI_EXCLAMATION);

	TCHAR null_char = _T('\0');
	if(!pszInfo)
	{
		pszInfo = &null_char; // better make it a writable space
		opt.bufchars = 1;
	}

	if(!opt.procGetText) // || opt.bufchars<=0)
	{
		opt.procGetText = fib_Dumb_GetText;
		opt.isAutoRefreshNow = false;
	}
	
	FibDlgParams_st dsi(opt, hwndOwner, pszInfo);

	INT_PTR dlgret = 0;
	for(;;)
	{
		if(resIdDlgbox)
		{
			dlgret = DialogBoxParam(hinstExeDll, resIdDlgbox,
				dsi.hwndRealOwner, fib_DlgProc, (LPARAM)&dsi);
		}
		else
		{
			dlgret = DialogBoxIndirectParam(hinstExeDll, pDlgTemplate,
				dsi.hwndRealOwner, fib_DlgProc, (LPARAM)&dsi);
		}

		if(dlgret==FIB_AgainAttached)
			dsi.hwndRealOwner = hwndOwner;
		else if(dlgret==FIB_AgainDetached)
			dsi.hwndRealOwner = NULL;
		else
			break;
	}
	
	if(dlgret==-1)
		return FIB_NoMem;
	else
		return (FIB_ret)dlgret;
}

#define ALIGN_TO_DWORD(pword) if((INT_PTR)(pword) & 0x3) *(pword)++ = 0;
#define SET_CHILD_RECT(pitem, _x,_y, _cx,_cy) { (pitem)->x=_x, (pitem)->y=_y, (pitem)->cx=_cx, (pitem)->cy=_cy; }

FIB_ret 
ggt_FlexiInfobox(HWND hwndRealParent, const FibInput_st *p_usr_opt, const TCHAR *pszInfo)
{
/*
	// NOTE: We need to manually sync the code here with DIALOGEX resource statements.

IDD_SHOW_INFO DIALOGEX 0, 0, 156, 56
STYLE DS_SETFONT | DS_FIXEDSYS | WS_MINIMIZEBOX | WS_POPUP | WS_VISIBLE | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME
CAPTION "FlexiInfo"
FONT 9, "MS Shell Dlg", 400, 0, 0x1
BEGIN
	PUSHBUTTON      "OK",IDOK,46,38,50,14
	PUSHBUTTON      "Btn2",IDCANCEL,99,38,50,14
	ICON            "",IDI_SHOW_INFO,14,6,20,20,SS_NOTIFY
	EDITTEXT        IDC_EDIT_SHOW_INFO,47,6,102,28,ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY | NOT WS_BORDER | WS_VSCROLL
	PUSHBUTTON      "&Refresh",IDC_BTN_REFRESH,6,28,36,12
	CONTROL         "&Auto",IDC_CHK_AUTOREFRESH,"Button",BS_AUTOCHECKBOX | WS_TABSTOP,7,42,35,10
END
*/
	char memblock[4000];

	// Prepare DLG template header:

	DLGTEMPLATE &dt = *(DLGTEMPLATE*)memblock;
	dt.style = DS_SETFONT|WS_MINIMIZEBOX|WS_POPUP|WS_VISIBLE|WS_CAPTION|WS_SYSMENU|WS_THICKFRAME; // DS_FIXEDSYS
	dt.dwExtendedStyle = 0;
	dt.cdit = 0; // count of child controls, increase later
	dt.x=0, dt.y=0, dt.cx=156, dt.cy=56;

	// Special case for bare-infobox scene: eliminate bottom button spaces.
	if(!p_usr_opt->szBtnOK && !p_usr_opt->szBtnCancel && p_usr_opt->isForceHideRefreshCtrl)
	{
		dt.cy = 34+5; // 34 is IDC_EDIT_SHOW_INFO's bottom y-pos
	}

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
	SET_CHILD_RECT(pitem, 46, 38, 50, 14);
	pitem->id = IDOK;
	//
	pword = (WORD*)(pitem+1);
	*pword++ = 0xFFFF; *pword++ = 0x0080; // ctrl class is "button"
	wcscpy((wchar_t*)pword, L"-"); pword+=2; // button text, change later in DlgProc
	*pword++ = 0x0000; // no creation data

	ALIGN_TO_DWORD(pword);

	// [Cancel/No] button
	pitem = (DLGITEMTEMPLATE*)pword;
	++dt.cdit;
	pitem->style = WS_VISIBLE|WS_TABSTOP;
	pitem->dwExtendedStyle = 0;
	SET_CHILD_RECT(pitem, 99, 38, 50, 14);
	pitem->id = IDC_BTN_CANCEL;
	//
	pword = (WORD*)(pitem+1);
	*pword++ = 0xFFFF; *pword++ = 0x0080; // ctrl class is "button"
	wcscpy((wchar_t*)pword, L"="); pword+=2; // button text, change later in DlgProc
	*pword++ = 0x0000; // no creation data

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
	pitem->x=47, pitem->y=6, pitem->cx=102, pitem->cy=28;
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
	return in_FlexiInfobox(hinstExe, NULL, &dt, hwndRealParent, p_usr_opt, pszInfo);
}

FIB_ret 
ggt_FlexiInfobox_userc(HINSTANCE hinstExeDll, LPCTSTR resIdDlgbox,
	HWND hwndRealParent, const FibInput_st *p_usr_opt, const TCHAR *pszInfo)
{
	return in_FlexiInfobox(hinstExeDll, resIdDlgbox, NULL, hwndRealParent, p_usr_opt, pszInfo);
}

void 
ggt_FlexiInfo(HWND hwndOwner, const TCHAR *pszInfo)
{
	FibInput_st si;
	si.szBtnOK = _T("OK");
	ggt_FlexiInfobox(hwndOwner, &si, pszInfo);
}

FIB_ret 
ggt_vaFlexiInfobox(HWND hwndOwner, const FibInput_st *p_usr_opt, const TCHAR *fmtInfo, ...)
{
	va_list args;
	va_start(args, fmtInfo);

	int reqlen_ = 1 + mm_vsnprintf(NULL, 0, fmtInfo, args);

	Cec_delete_pTCHAR bufInfo = new TCHAR[reqlen_];

	int req2 = mm_vsnprintf(bufInfo, reqlen_, fmtInfo, args);
	assert(req2+1==reqlen_); (void)req2;

	FIB_ret fibret = ggt_FlexiInfobox(hwndOwner, p_usr_opt, bufInfo);

	va_end(args);
	return fibret;
}

void 
ggt_vaFlexiInfo(HWND hwndOwner, const TCHAR *fmtInfo, ...)
{
	va_list args;
	va_start(args, fmtInfo);
	
	FibInput_st si;
	si.szBtnOK = _T("OK");
	ggt_vaFlexiInfobox(hwndOwner, &si, _T("%w"), MM_WPAIR_PARAM(fmtInfo, args));

	va_end(args);
}

