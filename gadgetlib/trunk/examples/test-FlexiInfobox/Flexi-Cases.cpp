#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <ps_TCHAR.h>
#include <commdefs.h>
#include <mm_snprintf.h>
#include <EnsureClnup_mswin.h>
#include <gadgetlib/FlexiInfobox.h>
#include <gadgetlib/timefuncs.h>

#include "resource.h"

extern HINSTANCE g_hinst;


typedef FIB_ret (*PROC_DoCase)(HWND hwnd, LPCTSTR ptext);

struct Case_st
{
	PROC_DoCase func;
	LPCTSTR text;
	int id;
};

FIB_ret fcSimplest(HWND hwnd, LPCTSTR ptext)
{
	ggt_FlexiInfo(hwnd, ptext);
	return FIB_OK;
}

FIB_ret fcCustomizeTitle(HWND hwnd, LPCTSTR ptext)
{
	FibInput_st si;
	si.title = _T("Your message"); // !
	si.szBtnOK = _T("Close");
	return ggt_FlexiInfobox(hwnd, &si, ptext);
}

FIB_ret fcWantCancelOnly(HWND hwnd, LPCTSTR ptext)
{
	// A small probleM here. User tabs to the editbox and Enter, it will return IDOK.
	FibInput_st si;
	si.szBtnCancel = _T("Cancel");
	return ggt_FlexiInfobox(hwnd, &si, ptext); 
}

FIB_ret fcOKandCancel(HWND hwnd, LPCTSTR ptext)
{
	FibInput_st si;
	si.szBtnOK = _T("OK");
	si.szBtnCancel = _T("Cancel");
	return ggt_FlexiInfobox(hwnd, &si, ptext);
}

FIB_ret fcYESandNO(HWND hwnd, LPCTSTR ptext)
{
	FibInput_st si;
	si.szBtnOK = _T("&YES");    // !
	si.szBtnCancel = _T("&NO"); // !
	return ggt_FlexiInfobox(hwnd, &si, ptext);
}

FIB_ret fcYESandNO_default_No(HWND hwnd, LPCTSTR ptext)
{
	FibInput_st si;
	si.szBtnOK = _T("&YES");
	si.szBtnCancel = _T("&NO");
	si.idDefaultFocus = IDC_BTN_CANCEL; // !
	return ggt_FlexiInfobox(hwnd, &si, ptext);
}

FIB_ret fcUseMonoFont(HWND hwnd, LPCTSTR ptext)
{
	FibInput_st si;
	si.szBtnOK = _T("OK");
	si.fixedwidth_font = true; // !
	return ggt_FlexiInfobox(hwnd, &si, ptext);
}

FIB_ret fcUseBiggerMonoFont(HWND hwnd, LPCTSTR ptext)
{
	FibInput_st si;
	si.szBtnOK = _T("OK");
	si.fixedwidth_font = true; // !
	si.fontsize = 12;          // !
	return ggt_FlexiInfobox(hwnd, &si, ptext);
}

FIB_ret fcUseBiggerDefaultFont(HWND hwnd, LPCTSTR ptext)
{
	FibInput_st si;
	si.szBtnOK = _T("OK");
	si.fontsize = 12;  // !
	return ggt_FlexiInfobox(hwnd, &si, ptext);
}

FIB_ret fcCustomizeIcon(HWND hwnd, LPCTSTR ptext)
{
	FibInput_st si;
	si.szBtnOK = _T("OK");
	si.hIcon = LoadIcon(g_hinst, MAKEINTRESOURCE(IDI_NEWLAND)); // !
	return ggt_FlexiInfobox(hwnd, &si, ptext);
}

FIB_ret fcDelayClose(HWND hwnd, LPCTSTR ptext)
{
	FibInput_st si;
	si.szBtnOK = _T("&Yes");
	si.szBtnCancel = _T("&No");
	si.msecDelayClose = 1000;
	return ggt_FlexiInfobox(hwnd, &si, ptext);
}

FIB_ret fcUseNoButtons(HWND hwnd, LPCTSTR ptext)
{
	FibInput_st si;
	return ggt_vaFlexiInfobox(hwnd, &si, _T("%s\r\n\r\n%s"), ptext,
		_T("Use Close nib or Alt+F4 to close this infobox."));
}

FibCallback_ret fcDenyCancel_GetText(void *ctx, 
	const FibCallback_st &cb_info,
	TCHAR *textbuf, int bufchars)
{
	if(cb_info.reason==FIBReason_CancelBtn)
		return FIBcb_Fail;
	else
		return FIBcb_OK;
}
FIB_ret fcDenyCancel(HWND hwnd, LPCTSTR ptext)
{
	FibInput_st si;
	si.szBtnOK = _T("&OK");
	si.szBtnCancel = _T("No &Cancel");
	si.procGetText = fcDenyCancel_GetText;
	return ggt_vaFlexiInfobox(hwnd, &si, _T("%s\r\n\r\n%s"), ptext,
		_T("You can only click OK to close this infobox."));
}

FibCallback_ret fcDenyCancelWithPrompt_GetText(void *ctx, 
	const FibCallback_st &cb_info,
	TCHAR *textbuf, int bufchars)
{
	// textbuf will equal to your input buffer to ggt_FlexiInfobox()
	// bufchars will equal to si.bufchars.
	if(cb_info.reason==FIBReason_CancelBtn)
	{
		mm_snprintf(textbuf, bufchars, _T("You see, you cannot Cancel it."));
		return FIBcb_FailIcon; // will show a fail-icon
	}
	else
		return FIBcb_OK;
}
//
FIB_ret fcDenyCancelWithPrompt(HWND hwnd, LPCTSTR ptext)
{
	const int bufsize = 400;
	TCHAR mytext[bufsize]= _T("");
	mm_strcat(mytext, bufsize, ptext);

	FibInput_st si;
	si.szBtnOK = _T("&OK");
	si.szBtnCancel = _T("No &Cancel");
	si.procGetText = fcDenyCancelWithPrompt_GetText;
	si.bufchars = bufsize; // !
	return ggt_FlexiInfobox(hwnd, &si, mytext);
}

struct fcRefreshable_st
{
	int count;
	const TCHAR *ptn;
};
//
FibCallback_ret fcRefreshable_GetText(void *_ctx, 
	const FibCallback_st &cb_info,
	TCHAR *textbuf, int bufchars)
{
	fcRefreshable_st &ctx = *(fcRefreshable_st*)_ctx;

	if(cb_info.reason==FIBReason_RefreshBtn) // !
	{
		if(ctx.count==0)
			textbuf[0] = _T('\0');

		ctx.count++;
		mm_strcat(textbuf, bufchars, 
			_T("[%2d]%.*s\r\n"), 
			ctx.count, ctx.count, ctx.ptn);
	}
	return FIBcb_OK;
}
//
FIB_ret fcRefreshable(HWND hwnd, LPCTSTR ptext)
{
	const int bufsize = 5500;
	TCHAR mytext[bufsize]= _T("");
	mm_strcat(mytext, bufsize, ptext);

	TCHAR stars[100] = {};
	for(int i=0; i<99; i++) stars[i] = _T('*');

	fcRefreshable_st ctx = {0, stars};

	FibInput_st si;
	si.szBtnOK = _T("&OK");
	si.procGetText = fcRefreshable_GetText;
	si.ctxGetText = &ctx;       // !
	si.isShowRefreshBtn = true; // !
	si.bufchars = bufsize;      // !
//	si.isScrollToEnd = true;
	return ggt_FlexiInfobox(hwnd, &si, mytext);
}


FibCallback_ret fcTimedRefresh_GetText(void *_ctx, 
	const FibCallback_st &cb_info,
	TCHAR *textbuf, int bufchars)
{
	(void)cb_info; (void)_ctx;
	SYSTEMTIME st;
	GetLocalTime(&st);
	mm_snprintf(textbuf, bufchars, 
		_T("Now time %02d:%02d:%02d"),
		st.wHour, st.wMinute, st.wSecond);
	return FIBcb_OK;
}
//
FIB_ret fcTimedRefresh(HWND hwnd, LPCTSTR ptext)
{
	const int bufsize = 50;
	TCHAR mytext[bufsize]= _T("");

	FibInput_st si;
	si.szBtnOK = _T("&OK");
	si.procGetText = fcTimedRefresh_GetText;
	si.msecAutoRefresh = 1000;  // ! unit: millisec
	si.isAutoRefreshNow = true;
	si.bufchars = bufsize;      // !
	return ggt_FlexiInfobox(hwnd, &si, mytext);
}


struct fcCountDownClose_st
{
	DWORD tkmsec_end;
};
//
FibCallback_ret fcCountDownClose_GetText(void *_ctx, 
	const FibCallback_st &cb_info,
	TCHAR *textbuf, int bufchars)
{
	fcCountDownClose_st &ctx = *(fcCountDownClose_st*)_ctx;

	DWORD tkmsec_now = GetTickCount();
	int msec_remain = ctx.tkmsec_end - tkmsec_now;

	if(msec_remain>0)
	{
		mm_snprintf(textbuf, bufchars, 
			_T("Wait %d seconds before this infobox closes itself."), 
//			_T("\r\n\r\n\r\n\r\n\r\n\r\n\r\nWait %d seconds before this infobox closes itself."), 
			msec_remain/1000+1);
		return FIBcb_Fail;
	}
	else
		return FIBcb_CloseDlg; // ! This auto-close the infobox
}
//
FIB_ret fcCountDownClose(HWND hwnd, LPCTSTR ptext)
{
	const int bufsize = 80;
	TCHAR mytext[bufsize]= _T("");

	fcCountDownClose_st ctx = { GetTickCount()+5000 };

	FibInput_st si;
	si.procGetText = fcCountDownClose_GetText;
	si.ctxGetText = &ctx;
	si.msecAutoRefresh = 500;  // need it to trigger auto-close
	si.isAutoRefreshNow = true;
	si.bufchars = bufsize;
	si.isNarrowTitle = true;          // optional
	si.isForceHideRefreshCtrl = true; // optional
	si.hIcon = LoadIcon(g_hinst, MAKEINTRESOURCE(IDI_CLOCK));
	return ggt_FlexiInfobox(hwnd, &si, mytext);
}


enum { FIBcmd_DisplayUTC = 100  };
//
struct fcTimeLocalOrUTC_st
{
	bool isShowUTC;
};
//
FibCallback_ret fcTimeLocalOrUTC_GetText(void *_ctx, 
	const FibCallback_st &cb_info,
	TCHAR *textbuf, int bufchars)
{
	bool &isutc = ((fcTimeLocalOrUTC_st*)_ctx)->isShowUTC;

	if(cb_info.reason==FIBReason_UserCmd) // !
	{
		if(cb_info.idUserCmd==FIBcmd_DisplayUTC)
		{
			isutc = cb_info.isTickOn ? true : false;
		}
	}

	SYSTEMTIME st;
	if(isutc)
	{
		GetSystemTime(&st);
		mm_snprintf(textbuf, bufchars, 
			_T("Now UTC %02d:%02d:%02d"),
			st.wHour, st.wMinute, st.wSecond);
	}
	else
	{
		GetLocalTime(&st);
		mm_snprintf(textbuf, bufchars, 
			_T("Now time %02d:%02d:%02d"),
			st.wHour, st.wMinute, st.wSecond);
	}
	return FIBcb_OK;
}
//
FIB_ret fcTimeLocalOrUTC(HWND hwnd, LPCTSTR ptext)
{
	const int bufsize = 50;
	TCHAR mytext[bufsize]= _T("");

	fcTimeLocalOrUTC_st ctx = {false};
	FibInput_st si;
	si.szBtnOK = _T("&OK");
	si.procGetText = fcTimeLocalOrUTC_GetText;
	si.ctxGetText = &ctx;
	si.msecAutoRefresh = 1000; 
	si.isAutoRefreshNow = true;
	si.bufchars = bufsize;
	// ! Define user cmds activated by right-clicking blank area
	FibUserCmds_st arUserCmds[] =
	{
		{FIBcmd_CopyInfo, FIBcst_Raw, _T("Copy to clipboard")},
		{FIBcmd_DisplayUTC, FIBcst_TickOff, _T("Display time in UTC")},
	};
	si.arUserCmds = arUserCmds;
	si.nUserCmds = GetEleQuan_i(arUserCmds);
	//
	return ggt_FlexiInfobox(hwnd, &si, mytext);
}

struct fcTitleShowTime_st
{
	__int64 uesecLastRefresh;
};
//
FibCallback_ret fcTitleShowTime_GetText(void *_ctx, 
	const FibCallback_st &cb_info,
	TCHAR *textbuf, int bufchars)
{
	(void)cb_info; 
	__int64 &uesecLastRefresh = ((fcTitleShowTime_st*)_ctx)->uesecLastRefresh;

	__int64 uesecNow = ggt_time64();
	
	if(uesecNow-uesecLastRefresh<5)
		return FIBcb_OK_NoChange;

	uesecLastRefresh = uesecNow;

	struct tm tmnow;
	ggt_localtime(uesecNow, &tmnow);
	mm_snprintf(textbuf, bufchars, 
		_T("Now local time %02d:%02d:%02d (refresh every 5 seconds)")
		_T("\r\n\r\n(hint: right click blank area to toggle title time stamp)"),
		tmnow.tm_hour, tmnow.tm_min, tmnow.tm_sec);
	return FIBcb_OK;
}
//
FIB_ret fcTitleShowTime(HWND hwnd, LPCTSTR ptext)
{
	const int bufsize = 200;
	TCHAR mytext[bufsize]= _T("");

	fcTitleShowTime_st ctx = {0};
	FibInput_st si;
	si.szBtnOK = _T("&OK");
	si.procGetText = fcTitleShowTime_GetText;
	si.ctxGetText = &ctx;
	si.msecAutoRefresh = 1000;
	si.isAutoRefreshNow = true;
	si.bufchars = bufsize;
	// Define user cmds activated by right-clicking blank area
	FibUserCmds_st arUserCmds[] =
	{
		{FIBcmd_LastTextTimeOnTitle, FIBcst_TickOn, _T("Title show last update time")},
		{FIBcmd_MenuSeparator}, // only a visual separator
		{FIBcmd_CopyInfo, FIBcst_Raw, _T("Copy to clipboard")},
	};
	si.arUserCmds = arUserCmds;
	si.nUserCmds = GetEleQuan_i(arUserCmds);
	//
	return ggt_FlexiInfobox(hwnd, &si, mytext);
}


Case_st gar_FlexiCases[] = 
{
	{ fcSimplest, _T("Simplest") },
	{ fcCustomizeTitle, _T("Customize title and button text") },
	{ fcWantCancelOnly, _T("Want Cancel button only") },
	{ fcOKandCancel, _T("Both OK can Cancel") },
	{ fcYESandNO, _T("Customize button text to YES and NO") },
	{ fcYESandNO_default_No, _T("Set default button to NO") },
	
	{ fcUseMonoFont, _T("Use mono font (fixed-width font)") },
	{ fcUseBiggerMonoFont, _T("Use bigger mono font size") },
	{ fcUseBiggerDefaultFont, _T("Use bigger font size with default fontface") },

	{ fcCustomizeIcon, _T("Customize icon") },

	{ fcDelayClose, _T("Not allow to close infobox or make choice within 1000 milliseconds") },
	{ fcUseNoButtons, _T("Deliberately no buttons") },

	{ NULL }, // a menu item separator

	{ fcDenyCancel, _T("Deny Cancel button's closing infobox") },
	{ fcDenyCancelWithPrompt, _T("Deny Cancel button and customize prompt") },
	{ fcRefreshable, _T("Use Refresh button for new info") },
	{ fcTimedRefresh, _T("Auto refresh to show clock time") },
	{ fcCountDownClose, _T("Countdown close: Only closeable by program") },
	{ fcTimeLocalOrUTC, _T("Infobox with context menu (right click blank area)") },
	{ fcTitleShowTime, _T("Title shows last text update time") },
};

void do_Cases(HWND hwnd)
{
	Cec_DestroyMenu hmenu = CreatePopupMenu();

	// Initialize case data
	int i=0;
	for(; i<GetEleQuan_i(gar_FlexiCases); i++)
	{
		Case_st &thiscase = gar_FlexiCases[i];
		thiscase.id = i+1;

		TCHAR szItem[80];
		mm_snprintf(szItem, GetEleQuan_i(szItem), _T("%d. %s"), thiscase.id, thiscase.text);

		if(thiscase.func)
			AppendMenu(hmenu, MF_STRING, thiscase.id, szItem);
		else
			AppendMenu(hmenu, MF_SEPARATOR, 0, 0);
	}

	RECT rectBtn;
	GetWindowRect(GetDlgItem(hwnd, IDC_BTN_CASES), &rectBtn);
	int mret = TrackPopupMenu(hmenu, TPM_RETURNCMD, rectBtn.left, rectBtn.bottom, 0, hwnd, NULL);
	if(mret>0)
	{
		static int s_count = 0;
		bool isShiftDown = GetAsyncKeyState(VK_SHIFT)<0 ? true:false;

		FIB_ret fibret = gar_FlexiCases[mret-1].func(
			isShiftDown ? NULL : hwnd, 
			gar_FlexiCases[mret-1].text
			);
		
		TCHAR hint[40];
		mm_snprintf(hint, GetEleQuan_i(hint), _T("[count=%d] FIB_ret=%d"), ++s_count, fibret);

		SetDlgItemText(hwnd, IDC_EDIT_HINT, hint);
	}
}
