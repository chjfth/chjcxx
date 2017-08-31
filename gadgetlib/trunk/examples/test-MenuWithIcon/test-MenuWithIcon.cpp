#include <Windows.h>
#include <WindowsX.h>
#include <Uxtheme.h>
#include <tchar.h>
#include <stdio.h>
#include "resource.h"

#include <mswin/CmnHdr-Jeffrey.h>

#include <gadgetlib/MenuWithIcon.h>

//#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
// -- set in .rc

HINSTANCE g_hinst;

///////////////////////////////////////////////////////////////////////////////

struct DlgPrivate_st
{
	const WCHAR *mystr;
};

BOOL Dlg_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam) 
{
	chSETDLGICONS(hwnd, IDI_VISTAICONMENU);

	DlgPrivate_st *pr = new DlgPrivate_st;
	pr->mystr = (const WCHAR*)lParam;

	SetWindowLongPtr(hwnd, DWLP_USER, (LONG_PTR)pr);

	TCHAR tmsg[40] = {0};
	_sntprintf_s(tmsg, 40, _T("IsThemeActive()=%d , IsAppThemed()=%d"), 
		IsThemeActive(), IsAppThemed());

	SetDlgItemText(hwnd, IDC_TEXT_THEME_INFO, tmsg);

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
	case IDM_INFORMATION:
		MessageBox(hwnd, _T("Selected  ABC info"), NULL, 0);
		break;
	case IDM_NO_ICON:
		MessageBox(hwnd, _T("Selected  No icon"), NULL, 0);
		break;
	case IDM_NEWLAND:
		MessageBox(hwnd, _T("Selected  NEWLAND"), NULL, 0);
		break;
	case IDM_ELEVATE:
		MessageBox(hwnd, _T("Selected IDM_ELEVATE"), NULL, 0);
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



void test_PopupMyMenu(HWND _hdlg, int x, int y)
{
//	SetDlgItemText(_hdlg, IDC_TEXT_ICOLOADMODE, lr==LeftClick?_T("LIM_LARGE"):_T("LIM_SMALL"));

	POINT ptClient = { x, y };
	ClientToScreen(_hdlg, &ptClient);
	
	HMENU hmenu = LoadMenu(g_hinst, MAKEINTRESOURCE(IDM_CONTEXTMENU));
	if (hmenu)
	{
		bool isShieldIco = ggt_TrackPopupMenuIsVistaStyle();

		ICONMENUENTRY aIcons[] = 
		{
			{ IDM_INFORMATION, NULL, IDI_INFORMATION },
			{ IDM_NEWLAND, g_hinst, MAKEINTRESOURCE(IDI_NEWLAND) },
#if (_WIN32_WINNT > 0x0600) // only for Vista
			{ IDM_ELEVATE, NULL, isShieldIco?IDI_SHIELD:IDI_EXCLAMATION },
#else
			{ IDM_ELEVATE, NULL, IDI_EXCLAMATION },
#endif
		};
		HMENU hmenuPopup = GetSubMenu(hmenu, 0);

		LoadIconErr_et lie = LIE_Fail;
#if 1
		UINT mret = ggt_TrackPopupMenuWithIcon(hmenuPopup, 
			0, // flags: TPM_RETURNCMD etc
			ptClient.x, ptClient.y, _hdlg, NULL, 
			aIcons, ARRAYSIZE(aIcons), &lie); // visual-style menu
#else
													UINT mret = TrackPopupMenuEx(hmenuPopup, 
														0, ptClient.x, ptClient.y, _hdlg, NULL); // pristine menu
#endif
		
		DestroyMenu(hmenu);

		if(lie!=LIE_Succ)
		{
			const int bufsize=100;
			TCHAR tbuf[bufsize];
			_sntprintf_s(tbuf, bufsize, bufsize, 
				_T("ggt_TrackPopupMenuWithIcon reports error: %d   (mret=%u)"), lie, mret);
			MessageBox(_hdlg, tbuf, NULL, MB_ICONEXCLAMATION);
		}
	}
}


void Dlg_OnRButtonUp(HWND hwnd, int x, int y, UINT flags)
{
	test_PopupMyMenu(hwnd, x, y);
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
		chHANDLE_DLGMSG(hwnd, WM_RBUTTONUP,     Dlg_OnRButtonUp);
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

