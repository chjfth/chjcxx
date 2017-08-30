#include <Windows.h>
#include <WindowsX.h>
#include <CommCtrl.h>
#include <tchar.h>
#include <stdio.h>
#include "resource.h"

#include <mswin/CmnHdr-Jeffrey.h>

#include <gadgetlib/MenuWithIcon.h>

#pragma comment(lib, "comctl32.lib")

#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

HINSTANCE g_hinst;

///////////////////////////////////////////////////////////////////////////////

struct DlgPrivate_st
{
	const WCHAR *mystr;
};

BOOL Dlg_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam) 
{
	chSETDLGICONS(hwnd, IDI_NEWLAND);

	DlgPrivate_st *pr = new DlgPrivate_st;
	pr->mystr = (const WCHAR*)lParam;

	SetWindowLongPtr(hwnd, DWLP_USER, (LONG_PTR)pr);

	return(TRUE);
}

void Dlg_OnDestroy(HWND hwnd)
{
	DlgPrivate_st *pr = (DlgPrivate_st*)GetWindowLongPtr(hwnd, DWLP_USER);

	// Destroy all resources allocated back in Dlg_OnInitDialog().
	delete pr;
}

void Dlg_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify) 
{
//	DlgPrivate_st *pr = (DlgPrivate_st*)GetWindowLongPtr(hwnd, DWLP_USER);

	switch (id) 
	{{
	case IDC_DO_SHOWINFO:
		xxx;
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
	InitCommonControls(); // WinXP requires this to see any dialogbox.

	// Memo: I do not call BufferedPaintInit()/BufferedPaintUninit() bcz I did not 
	// notice any performance problem yet -- may be the icon is too small to reveal it.

	g_hinst = hinstExe;
	const WCHAR *mystr = L"My private string";
	DialogBoxParam(hinstExe, MAKEINTRESOURCE(IDD_WINMAIN), NULL, Dlg_Proc, (LPARAM)mystr);
	return(0);
}

