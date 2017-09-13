#ifndef __dlg_showinfo_h_20170717_
#define __dlg_showinfo_h_20170717_

#include <RECTxy.h>

#ifdef __cplusplus
extern"C"{
#endif

#ifndef DLLEXPORT_gadgetlib
# define DLLEXPORT_gadgetlib
#endif

enum FibCallback_ret
{
	FIB_OK = 0,
	FIB_Fail = 1,
	FIB_Fail_StopAutoRefresh = 2,
	FIB_CloseDlg = 3, // the dialog will be closed
};

struct FibCallback_st
{
	HWND hDlg; // for user tweak, change [OK] button text etc.

	bool isCallFromRefreshBtn; // callback due to user clicking [Refresh]
	                           // If false, it is due to auto-refresh timer

	bool isAutoRefreshOn;      // whether the [x]Auto check box is checked now

	bool isOKBtnRequested; // user has clicked [OK] sometime ago
	DWORD msecOKBtnRequested; // the value from GetTickCount()

	bool isCloseRequested; // user has clicked close-window nib sometime ago
	DWORD msecCloseRequested; // the value from GetTickCount()
};


typedef FibCallback_ret (*PROC_DlgShowinfo_GetText)(void *ctx, 
	const FibCallback_st &cb_info,
	TCHAR *textbuf, int bufchars);
	// textbuf = dlg_showinfo()'s pszInfo param
	// bufchars = dlg_showinfo_st.bufchars

struct FibInput_st
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

	int fontsize; // if 0, default to 9

	const TCHAR *szOK; // text for the bottom OK button, hidden if null
	const TCHAR *szRefreshBtnText; // user can customize button text
	const TCHAR *szAutoChktext;

	int maxVisualCharsX; // limit max dialog-box width (implement later)
	int maxVisualLines;  // limit max dialog-box height (implement later)
	bool isScrollToEnd;  // let final line of text always visible (implement later)


	FibInput_st()
	{
		memset(this, 0, sizeof(this));

		title = NULL;
		hIcon = NULL;
		fixedwidth_font = false;

		procGetText = NULL; 
		ctxGetText = NULL;

		msecAutoRefresh = 0;
		isRefreshNow = isAutoRefreshNow = false;

		isOnlyClosedByProgram = false;

		fontsize = 0;

		szOK = szRefreshBtnText = szAutoChktext = NULL;

		maxVisualCharsX = maxVisualLines = 0;
		isScrollToEnd = false;
	}
};


enum FIB_ret 
{
	Dsie_Success = 0,
	Dsie_Fail = 1,
	Dsie_NoMem = 2,
	Dsie_BadParam = 3,

	// Invalid param error:

	Dsie_OnlyClosedByProgram_but_NoCallback = 10,
};

DLLEXPORT_gadgetlib
FIB_ret ggt_FlexiInfobox(
	HWND hwndRealParent, const FibInput_st *p_usr_opt, const TCHAR *pszInfo);

DLLEXPORT_gadgetlib
FIB_ret ggt_FlexiInfobox_userc(HINSTANCE hinstExeDll, LPCTSTR resIdDlgbox,
	HWND hwndRealParent, const FibInput_st *p_usr_opt, const TCHAR *pszInfo);



#ifdef __cplusplus
} // extern"C"{
#endif


#endif