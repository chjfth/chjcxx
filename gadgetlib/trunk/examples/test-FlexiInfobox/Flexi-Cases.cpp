#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <ps_TCHAR.h>
#include <commdefs.h>
#include <mm_snprintf.h>
#include <EnsureClnup_mswin.h>
#include <gadgetlib/FlexiInfobox.h>

#include "resource.h"

typedef FIB_ret (*PROC_DoCase)(HWND hwnd, const TCHAR *ptext);

struct Case_st
{
	int id;
	PROC_DoCase func;
	const TCHAR *text;
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

Case_st gar_FlexiCases[] = 
{
	{ 0, fcSimplest, _T("Simplest") },
	{ 0, fcOKandCancel, _T("Both OK can Cancel") },
	{ 0, fcYESandNO, _T("Custom button text to YES and NO") },
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
		FIB_ret fibret = gar_FlexiCases[mret-1].func(hwnd, gar_FlexiCases[mret-1].text);
		TCHAR hint[40];
		mm_snprintf(hint, GetEleQuan_i(hint), _T("FIB_ret=%d"), fibret);
		SetDlgItemText(hwnd, IDC_EDIT_HINT, hint);
	}
}
