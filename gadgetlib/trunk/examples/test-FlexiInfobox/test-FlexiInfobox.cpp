#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <WindowsX.h>
#include <CommCtrl.h>
#include <tchar.h>
#include <stdio.h>

#include <mm_snprintf.h>
#include <mswin/CmnHdr-Jeffrey.h>

#include <gadgetlib/FlexiInfobox.h>

#include "resource.h"


#pragma comment(lib, "comctl32.lib")

//#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
// -- set in .rc.

HINSTANCE g_hinst;

///////////////////////////////////////////////////////////////////////////////

struct DlgPrivate_st
{
	const WCHAR *mystr;
};

BOOL Dlg_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam) 
{
	chSETDLGICONS(hwnd, IDI_INFORMATION, NULL, true);
	//chSETDLGICONS(hwnd, IDI_NEWLAND);

	DlgPrivate_st *pr = new DlgPrivate_st;
	pr->mystr = (const WCHAR*)lParam;

	SetWindowLongPtr(hwnd, DWLP_USER, (LONG_PTR)pr);

	SetDlgItemText(hwnd, IDC_EDIT_HINT, 
		_T("Hint: Press Shift to use NULL as hwndParent.\n")
		_T("Press Ctrl to show both OK/Cancel buttons.\n")
		);

	SetFocus(NULL);
	return(FALSE); // no default focus
}

void Dlg_OnDestroy(HWND hwnd)
{
	DlgPrivate_st *pr = (DlgPrivate_st*)GetWindowLongPtr(hwnd, DWLP_USER);

	// Destroy all resources allocated back in Dlg_OnInitDialog().
	delete pr;
}

struct MyDsiContext_st
{
	bool isFromRC;
};

FibCallback_ret DSI_GetNowTime(void *_ctx, 
	const FibCallback_st &cb_info,
	TCHAR *textbuf, int bufchars)
{
	(void)cb_info;
	MyDsiContext_st &ctx = *(MyDsiContext_st*)_ctx;

	SYSTEMTIME st;
	GetLocalTime(&st);
	mm_snprintf(textbuf, bufchars, 
		_T("%s\r\n\r\nNow time from GetLocalTime(): %04d-%02d-%02d %02d:%02d:%02d.%03d"), 
		ctx.isFromRC ? _T("Msgbox from RC") : _T("MsgBox from DLGITEMTEMPLATE"),
		st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, st.wMilliseconds
		);

	return FIBcb_OK;
}

void do_Flexi(HWND hwndParent, bool isUseRC, bool isMono)
{
	bool isShiftDown = GetKeyState(VK_SHIFT)<0 ? true:false;
	bool isCtrlDown = GetKeyState(VK_CONTROL)<0 ? true:false;

	HWND useParent = isShiftDown ? NULL : hwndParent;
		// Using NULL as hwndParent so that we can pop out one with RC and another without RC
		// for side-by-side comparison. 

	TCHAR szTimeText[400] = _T("to-modify");
	MyDsiContext_st ctx = { isUseRC ? true : false };

	FibInput_st si;
	si.title = _T("FlexiInfobox Sample");
	si.fixedwidth_font = isMono ? true : false;
	si.procGetText = DSI_GetNowTime;
	si.ctxGetText = &ctx;
	si.bufchars = ARRAYSIZE(szTimeText);
	si.msecAutoRefresh = 500;
	si.isAutoRefreshNow = true;
	si.msecDelayClose = 1000;
//	si.fontsize = 12;
	if(!isCtrlDown)
	{
//		si.idDefaultFocus = IDOK; // The button IDOK will defaulty have focus
		si.szBtnOK = _T("Cl&ose");
	}
	else
	{
		si.idDefaultFocus = IDCANCEL;
		si.szBtnOK = _T("&O K");
		si.szBtnCancel = _T("&Cancel");
	}
	// opt.isOnlyClosedByProgram = true;

	FIB_ret fibret;
	if(isUseRC)
	{
		fibret = ggt_FlexiInfobox_userc( 
			(HINSTANCE)GetWindowLongPtr(hwndParent, GWLP_HINSTANCE), 
			MAKEINTRESOURCE(IDD_SHOW_INFO),
			useParent, &si, szTimeText); 
	}
	else
	{
		fibret = ggt_FlexiInfobox(useParent, &si, szTimeText);
	}

	TCHAR szret[80];
	_sntprintf_s(szret, ARRAYSIZE(szret), 
		_T("FIB_ret=%d\r\n")
		_T("1=OK/Yes 2=Cancel/No"), 
		fibret);
	SetDlgItemText(hwndParent, IDC_EDIT_HINT, szret);
}

void Dlg_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify) 
{
//	DlgPrivate_st *pr = (DlgPrivate_st*)GetWindowLongPtr(hwnd, DWLP_USER);

	void do_Cases(HWND);

	switch (id) 
	{{
	case IDC_USE_RC:
		do_Flexi(hwnd, true, false);
		break;
	case IDC_USE_RC_MONO:
		do_Flexi(hwnd, true, true);
		break;
	case IDC_NOT_USE_RC:
		do_Flexi(hwnd, false, false);
		break;
	case IDC_NOT_USE_RC_MONO:
		do_Flexi(hwnd, false, true);
		break;
	case IDC_BTN_CASES:
		do_Cases(hwnd);
		break;
	case IDOK:
	case IDCANCEL:
		EndDialog(hwnd, id);
		break;

	}}
}

void Dlg_OnSize(HWND hwnd, UINT state, int cx, int cy) 
{
	// Reposition the child controls
	//	g_UILayout.AdjustControls(cx, cy);    
}


void Dlg_OnGetMinMaxInfo(HWND hwnd, PMINMAXINFO pMinMaxInfo) 
{
	// Return minimum size of dialog box
	//	g_UILayout.HandleMinMax(pMinMaxInfo);
}




INT_PTR WINAPI Dlg_Proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) 
{
	switch (uMsg) 
	{
		chHANDLE_DLGMSG(hwnd, WM_INITDIALOG,    Dlg_OnInitDialog);
		chHANDLE_DLGMSG(hwnd, WM_DESTROY,       Dlg_OnDestroy);
		chHANDLE_DLGMSG(hwnd, WM_COMMAND,       Dlg_OnCommand);
		chHANDLE_DLGMSG(hwnd, WM_SIZE,          Dlg_OnSize);
		chHANDLE_DLGMSG(hwnd, WM_GETMINMAXINFO, Dlg_OnGetMinMaxInfo);
//		chHANDLE_DLGMSG(hwnd, WM_RBUTTONUP,     Dlg_OnRButtonUp);
	}
	return(FALSE);
}


int WINAPI _tWinMain(HINSTANCE hinstExe, HINSTANCE, PTSTR pszCmdLine, int) 
{
	InitCommonControls(); // WinXP requires this to see any v6 dialogbox.

	g_hinst = hinstExe;
	const WCHAR *mystr = L"My private string";
	DialogBoxParam(hinstExe, MAKEINTRESOURCE(IDD_WINMAIN), NULL, Dlg_Proc, (LPARAM)mystr);
	return(0);
}

