#ifndef __dlg_showinfo_h_20170717_
#define __dlg_showinfo_h_20170717_

#include <RECTxy.h>

#ifdef __cplusplus
extern"C"{
#endif

enum DlgShowinfoCallback_ret
{
	DSICB_OK = 0,
	DSICB_Fail = 1,
	DSICB_Fail_StopAutoRefresh = 2,
	DSICB_CloseDlg = 3, // the dialog will be closed
};

struct dlg_showinfo_callback_st
{
	HWND hDlg; // for user tweak, change [OK] button text etc.

	bool isOKBtnRequested; // user has clicked [OK]
	DWORD msecOKBtnRequested; // the value from GetTickCount()

	bool isCloseRequested; // user has clicked close-window nib
	DWORD msecCloseRequested;
};


typedef DlgShowinfoCallback_ret (*PROC_DlgShowinfo_GetText)(void *ctx, 
	const dlg_showinfo_callback_st &cb_info,
	TCHAR *textbuf, int bufchars);
	// textbuf = dlg_showinfo()'s pszInfo param
	// bufchars = dlg_showinfo_st.bufchars

struct dlg_showinfo_st
{
	const TCHAR *title; // optional
	HICON hIcon; // optional, =LoadIcon(NULL, IDI_INFORMATION)
	bool fixedwidth_font;

	// If procGetText!=NULL, I'll show a [Refresh] button so that info text can be refreshed.
	PROC_DlgShowinfo_GetText procGetText;
	void *ctxGetText;
	int bufchars;

	int msecAutoRefresh; // non-zero will enable auto-refresh feature
	
	bool isRefreshNow; 
		// true means calling procGetText() right now once at entrance
	bool isAutoRefreshNow; 
		// true means start auto-refresh automatically, 
		// otherwise,user should enable a check-box to start auto-refresh.

	bool isOnlyClosedByProgram;
		// If true, user cannot use [OK] or [X] to close the dialog,
		// only a DSICB_CloseDlg return from callback can close it.

	const TCHAR *szOK; // text for the bottom OK button, hidden if null
	const TCHAR *szRefreshBtnText; // user can customize button text
	const TCHAR *szAutoChktext;

	int maxVisualCharsX; // limit max dialog-box width (implement later)
	int maxVisualLines;  // limit max dialog-box height (implement later)
	bool isScrollToEnd;  // let final line of text always visible (implement later)


	dlg_showinfo_st()
	{
		title = NULL;
		hIcon = NULL;
		fixedwidth_font = false;

		procGetText = NULL; 
		ctxGetText = NULL;

		msecAutoRefresh = 0;
		isRefreshNow = isAutoRefreshNow = false;

		isOnlyClosedByProgram = false;

		szOK = szRefreshBtnText = szAutoChktext = NULL;

		maxVisualCharsX = maxVisualLines = 0;
		isScrollToEnd = false;
	}
};


enum dlg_showinfo_ret 
{
	Dsie_Success = 0,
	Dsie_Fail = 1,
	Dsie_NoMem = 2,

	// Invalid param error:

	Dsie_OnlyClosedByProgram_but_NoCallback = 10,
};

dlg_showinfo_ret dlg_showinfo(HWND hwndParent, const dlg_showinfo_st *opt, const TCHAR *info);

dlg_showinfo_ret dlg_showinfo_t2(HINSTANCE hinstExeDll, LPCTSTR resIdDlgbox,
	HWND hwndRealParent, const dlg_showinfo_st *p_usr_opt, const TCHAR *pszInfo);



#ifdef __cplusplus
} // extern"C"{
#endif


#endif
