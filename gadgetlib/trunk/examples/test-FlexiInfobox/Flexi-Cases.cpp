#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <ps_TCHAR.h>
#include <commdefs.h>
#include <mm_snprintf.h>
#include <EnsureClnup_mswin.h>
#include <gadgetlib/FlexiInfobox.h>

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

	{ fcDelayClose, _T("Not allow to close infobox or make choice within 1000 milliseconds.") },
	{ fcUseNoButtons, _T("Deliberately no buttons") },

	{ NULL }, // a menu item separator

	{ fcDenyCancel, _T("Deny Cancel button's closing infobox") },
	{ fcDenyCancelWithPrompt, _T("Deny Cancel button and customize prompt") },
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
	int mret = TrackPopupMenu(hmenu, TPM_RETURNCMD , rectBtn.left, rectBtn.bottom, 0, hwnd, NULL);
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
