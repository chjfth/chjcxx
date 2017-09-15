#ifndef __dlg_showinfo_h_20170717_
#define __dlg_showinfo_h_20170717_

#include <RECTxy.h>
#include <gadgetlib/FlexiInfobox_ids.h>

#ifdef __cplusplus
extern"C"{
#endif

#ifndef DLLEXPORT_gadgetlib
# define DLLEXPORT_gadgetlib
#endif

enum FibCallback_ret
{
	FIBcb_OK = 0,
	FIBcb_Fail = 1,
	FIBcb_FailIcon = 2,
	FIBcb_Fail_StopAutoRefresh = 3,
	FIBcb_FailIcon_StopAutoRefresh = 4,
	FIBcb_CloseDlg = 5, // the dialog will be closed
};

enum FibCallbackReason_et
{
	FIBReason_Timer = 0,
	FIBReason_OKBtn = IDOK,
	FIBReason_CancelBtn = IDCANCEL,
	FIBReason_RefreshBtn = 3,
};

struct FibCallback_st
{
	HWND hDlg; // for user tweak, change [OK] button text etc.

	FibCallbackReason_et reason;

	bool isAutoRefreshOn;      // whether the [x]Auto check box is checked now

	bool isOKBtnRequested; // user has clicked [OK] sometime ago
	DWORD msecOKBtnRequested; // the value from GetTickCount()

	bool isCancelRequested; // user has clicked close-window or [Cancel] nib sometime ago
	DWORD msecCancelRequested; // the value from GetTickCount()

	int ret_replace_offset; // use later for scroll-to-end optimization
};

//#define FIB_NoButton _T("")

typedef FibCallback_ret (*PROC_FibCallback_GetText)(void *ctx, 
	const FibCallback_st &cb_info,
	TCHAR *textbuf, int bufchars);
	// textbuf = dlg_showinfo()'s pszInfo param
	// bufchars = dlg_showinfo_st.bufchars

struct FibInput_st
{
	int StructSize;

	const TCHAR *title; // optional
	bool fixedwidth_font;

	HICON hIcon;        // optional, =LoadIcon(NULL, IDI_INFORMATION)
	HICON hIconFail;    // optional, =LoadIcon(NULL, IDI_EXCLAMATION)

	// If procGetText!=NULL, I'll 
	PROC_FibCallback_GetText procGetText;
	void *ctxGetText;
	int bufchars;
	
	bool isShowRefreshBtn; // show a [Refresh] btn so that info text can be refreshed by user.
//	bool isRefreshNow; // deprecated
		// -- true means calling procGetText() right now once at entrance
	bool isAutoRefreshNow; 
		// true means start auto-refresh automatically, 
		// otherwise,user should enable a check-box to start auto-refresh.

	int msecAutoRefresh; // non-zero will enable auto-refresh feature

	int fontsize; // if 0, default to 9

	int msecDelayClose; // to avoid closing dlgbox too quickly(probably accidentally)

	const TCHAR *szBtnOK; 
		// Text for the bottom OK button.
		// If NULL, default to "OK". If "", OK and Btm2 will not be shown,
		// and the dlgbox can only be closed via FIB_CloseDlg.
	const TCHAR *szBtnCancel; 
		// Text for the second button, oftenly used to present Yes/No choice.
		// Note: If szBtnOK==NULL && szBtnCancel==NULL, there will be no button.

	const TCHAR *szBtnRefresh; // user can customize button text
	const TCHAR *szAutoChkbox;

	int idDefaultFocus;
	
	bool isScrollToEnd;  // let final line of text always visible

	int maxVisualCharsX; // limit max dialog-box width (implement later)
	int maxVisualLines;  // limit max dialog-box height (implement later)

//	isOnlyClosedByProgram = false; // deprecated


	FibInput_st()
	{
		memset(this, 0, sizeof(*this));
		StructSize = sizeof(*this);

		title = NULL;
		fixedwidth_font = false;

		hIcon = hIconFail = NULL;

		procGetText = NULL; 
		ctxGetText = NULL;

		msecAutoRefresh = 0;
		isAutoRefreshNow = false;

//		isOnlyClosedByProgram = false;

		fontsize = 0;

		msecDelayClose = 0;

		szBtnOK = szBtnCancel = szBtnRefresh = szAutoChkbox = NULL;

		idDefaultFocus = 0;

		isScrollToEnd = false;

		maxVisualCharsX = maxVisualLines = 0;
		isScrollToEnd = false;
	}
};


enum FIB_ret 
{
	FIB_Success = 1,
	FIB_OK = 1,
	FIB_Yes = 1,

	FIB_No = 2,
	FIB_Cancel = 2,

	FIB_Fail = 4,
	FIB_NoMem = 5,
	FIB_BadParam = 6,

	// Invalid param error:

//	FIB_OnlyClosedByProgram_but_NoCallback = 10,
};

DLLEXPORT_gadgetlib
FIB_ret ggt_FlexiInfobox(
	HWND hwndParent, const FibInput_st *p_usr_opt, const TCHAR *pszInfo);

DLLEXPORT_gadgetlib
void ggt_FlexiInfo(HWND hwndParent, const TCHAR *pszInfo);

DLLEXPORT_gadgetlib
FIB_ret ggt_vaFlexiInfobox(
	HWND hwndParent, const FibInput_st *p_usr_opt, const TCHAR *fmtInfo, ...);

DLLEXPORT_gadgetlib
void ggt_vaFlexiInfo(HWND hwndParent, const TCHAR *fmtInfo, ...);


DLLEXPORT_gadgetlib
FIB_ret ggt_FlexiInfobox_userc(HINSTANCE hinstExeDll, LPCTSTR resIdDlgbox,
	HWND hwndParent, const FibInput_st *p_usr_opt, const TCHAR *pszInfo);



#ifdef __cplusplus
} // extern"C"{
#endif


#endif
