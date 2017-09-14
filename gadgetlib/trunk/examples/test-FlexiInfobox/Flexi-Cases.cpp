#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <ps_TCHAR.h>
#include <commdefs.h>
#include <mm_snprintf.h>
#include <EnsureClnup_mswin.h>
#include <gadgetlib/FlexiInfobox.h>

#include "resource.h"

extern HINSTANCE g_hinst;


typedef FIB_ret (*PROC_DoCase)(HWND hwnd, const TCHAR *ptext);

struct Case_st
{
	PROC_DoCase func;
	const TCHAR *text;
	int id;
};

FIB_ret fcSimplest(HWND hwnd, const TCHAR *ptext)
{
	ggt_FlexiInfo(hwnd, ptext);
	return FIB_OK;
}

FIB_ret fcOKandCancel(HWND hwnd, const TCHAR *ptext)
{
	FibInput_st si;
	si.szBtnOK = _T("OK");
	si.szBtnCancel = _T("Cancel");
	return ggt_FlexiInfobox(hwnd, &si, ptext);
}

FIB_ret fcYESandNO(HWND hwnd, const TCHAR *ptext)
{
	FibInput_st si;
	si.szBtnOK = _T("YES");
	si.szBtnCancel = _T("NO");
	return ggt_FlexiInfobox(hwnd, &si, ptext);
}

FIB_ret fcYESandNO_default_No(HWND hwnd, const TCHAR *ptext)
{
	FibInput_st si;
	si.szBtnOK = _T("YES");
	si.szBtnCancel = _T("NO");
	si.idDefaultFocus = IDC_BTN_CANCEL;
	return ggt_FlexiInfobox(hwnd, &si, ptext);
}

FIB_ret fcUseMonoFont(HWND hwnd, const TCHAR *ptext)
{
	FibInput_st si;
	si.szBtnOK = _T("OK");
	si.fixedwidth_font = true;
	return ggt_FlexiInfobox(hwnd, &si, ptext);
}

FIB_ret fcUseBiggerMonoFont(HWND hwnd, const TCHAR *ptext)
{
	FibInput_st si;
	si.szBtnOK = _T("OK");
	si.fixedwidth_font = true;
	si.fontsize = 12;
	return ggt_FlexiInfobox(hwnd, &si, ptext);
}

FIB_ret fcUseBiggerDefaultFont(HWND hwnd, const TCHAR *ptext)
{
	FibInput_st si;
	si.szBtnOK = _T("OK");
	si.fontsize = 12;
	return ggt_FlexiInfobox(hwnd, &si, ptext);
}

FIB_ret fcCustomizeIcon(HWND hwnd, const TCHAR *ptext)
{
	FibInput_st si;
	si.szBtnOK = _T("OK");
	si.hIcon = LoadIcon(g_hinst, MAKEINTRESOURCE(IDI_NEWLAND));
	return ggt_FlexiInfobox(hwnd, &si, ptext);
}


Case_st gar_FlexiCases[] = 
{
	{ fcSimplest, _T("Simplest") },
	{ fcOKandCancel, _T("Both OK can Cancel") },
	{ fcYESandNO, _T("Customize button text to YES and NO") },
	{ fcYESandNO_default_No, _T("Set default button to NO") },
	
	{ fcUseMonoFont, _T("Use mono font (fixed-width font)") },
	{ fcUseBiggerMonoFont, _T("Use bigger mono font") },
	{ fcUseBiggerDefaultFont, _T("Use bigger default font") },

	{ fcCustomizeIcon, _T("Customize icon") },

};

void do_Cases(HWND hwnd)
{
	Cec_DestroyMenu hmenu = CreatePopupMenu();

	// Initialize case data
	int i=0;
	for(; i<GetEleQuan_i(gar_FlexiCases); i++)
	{
		int idnow = i+1;
		gar_FlexiCases[i].id = idnow;

		TCHAR szItem[80];
		mm_snprintf(szItem, GetEleQuan_i(szItem), _T("%d. %s"), idnow, gar_FlexiCases[i].text);

		AppendMenu(hmenu, MF_STRING, idnow, szItem);
	}

	POINT pt;
	GetCursorPos(&pt);
	int mret = TrackPopupMenu(hmenu, TPM_RETURNCMD , pt.x, pt.y, 0, hwnd, NULL);
	if(mret>0)
	{
		bool isShiftDown = GetAsyncKeyState(VK_SHIFT)<0 ? true:false;

		FIB_ret fibret = gar_FlexiCases[mret-1].func(
			isShiftDown ? NULL : hwnd, 
			gar_FlexiCases[mret-1].text
			);
		
		TCHAR hint[40];
		mm_snprintf(hint, GetEleQuan_i(hint), _T("FIB_ret=%d"), fibret);

		SetDlgItemText(hwnd, IDC_EDIT_HINT, hint);
	}
}
