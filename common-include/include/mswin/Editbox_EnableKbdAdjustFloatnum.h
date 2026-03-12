#ifndef __CHHI__Editbox_EnableKbdAdjustFloatnum_h_20260312_
#define __CHHI__Editbox_EnableKbdAdjustFloatnum_h_20260312_

// This file is modified from Editbox_EnableKbdAdjustNumber.h

#include <tchar.h>
#include <windows.h>

enum EditboxKAF_err 
{
	EditboxKAF_Succ = 0,

	// Error on enable
	EditboxKAF_Unknown = 1,
	EditboxKAF_NoMem = 2,
	EditboxKAF_AlreadyEnabled = 3,
	EditboxKAF_BadParam = 4, 

	// Error on disable
	EditboxKAF_NotEnabledYet = 10,
	EditboxKAF_ChainMoved = 11,
};

// Purpose: Add extra keyboard functionality to a standard editbox.
// When the caret is on a float number, user pressing Up-key will increase the number
// by a preset step-value, pressing Down will decrease the number. 
// This is an alternative way to using additional spin control(UPDOWN_CLASS) on the UI.

EditboxKAF_err Editbox_EnableKbdAdjustFloatnum(HWND hEdit,
	double min_val, double max_val, double step_val=1.0, 
	const TCHAR *szfmt=_T("%g"), // %f, %g, %.3f etc
	bool is_wrap_around=false,
	const TCHAR *szHelpText=nullptr // Can be multiline
	);
// -- Hint: If szHelpText[] ends in '\n', library's stock extra keyboard hint will be appended.

EditboxKAF_err Editbox_DisableKbdAdjustFloatnum(HWND hEdit);
// -- optional, Editbox's WM_NCDESTROY will call this automatically, or you do it explicitly.



/*
////////////////////////////////////////////////////////////////////////////
 ___                 _                           _        _   _             
|_ _|_ __ ___  _ __ | | ___ _ __ ___   ___ _ __ | |_ __ _| |_(_) ___  _ __  
 | || '_ ` _ \| '_ \| |/ _ \ '_ ` _ \ / _ \ '_ \| __/ _` | __| |/ _ \| '_ \ 
 | || | | | | | |_) | |  __/ | | | | |  __/ | | | || (_| | |_| | (_) | | | |
|___|_| |_| |_| .__/|_|\___|_| |_| |_|\___|_| |_|\__\__,_|\__|_|\___/|_| |_|
              |_|                                                           
////////////////////////////////////////////////////////////////////////////
*/
// ++++++++++++++++++ Implementation Below ++++++++++++++++++

#if defined(Editbox_EnableKbdAdjustFloatnum_IMPL) || (defined CHHI_ALL_IMPL && !defined CHHI_ALL_IMPL_HIDE_Editbox_EnableKbdAdjustFloatnum) // [IMPL]

// >>> Include headers required by this lib's implementation
#include <assert.h>
#include <stdarg.h>
#include <tchar.h>
#include <WindowsX.h>
#include <mswin/WindowsX2.h> // chj: for SUBCLASS_FILTER_MSG0()
//
#include <sdring.h>
#include <mswin/CxxWindowSubclass.h>
#include <mswin/DlgTooltipEasy.h>
// <<< Include headers required by this lib's implementation


#ifndef Editbox_EnableKbdAdjustFloatnum_DEBUG
#include <CHHI_vaDBG_hide.h>
#endif


/*
    ____       _             __                                                                
   / __ \_____(_)   ______ _/ /____       ____  ____ _____ ___  ___  _________  ____ _________ 
  / /_/ / ___/ / | / / __ `/ __/ _ \     / __ \/ __ `/ __ `__ \/ _ \/ ___/ __ \/ __ `/ ___/ _ \
 / ____/ /  / /| |/ / /_/ / /_/  __/    / / / / /_/ / / / / / /  __(__  ) /_/ / /_/ / /__/  __/
/_/   /_/  /_/ |___/\__,_/\__/\___/    /_/ /_/\__,_/_/ /_/ /_/\___/____/ .___/\__,_/\___/\___/ 
                                                                      /_/                      
*/
// We should enclose this lib's implementation into private namespace.

////////////////////////////////////////////////////////////////////////////
namespace EditboxKAF {
////////////////////////////////////////////////////////////////////////////
// (private namespace 'EditboxKAF') 


//enum { SZFMT_MAX_= 8 };
enum { MaxUpDownDigits = 12 };

const TCHAR *s_sigSubclass = _T("sig_EditboxKAF");

class EditboxPeeker : public CxxWindowSubclass
{
public:
	EditboxPeeker() {}
	EditboxKAF_err ctor_params(double min_val, double max_val, double step_val, 
		const TCHAR *fmt, bool is_wrap_around, const TCHAR *szHelpText);

	virtual LRESULT WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	const TCHAR* GetTooltipText();

private:
	void HideMouse()
	{
		// When user starts adjusting numbers with keyboard, I'll hide the mouse 
		// temporarily so that mouse cursor does not obscure the ticking numbers.
		if(! is_mouse_hidden) {
			ShowCursor(FALSE);
			is_mouse_hidden = true;
		}
	}

	void ShowMouse()
	{
		if(is_mouse_hidden) {
			ShowCursor(TRUE);
			is_mouse_hidden = false;
		}
	}

	MsgRelay_et Edit_OnKey(HWND hwnd, UINT vk, BOOL fDown, int cRepeat, UINT flags);

	MsgRelay_et Edit_OnMouseMove(HWND hEdit, int x, int y, UINT keyFlags);

	MsgRelay_et Edit_OnNCDestroy(HWND hEdit);

private:
	double min_val;
	double max_val;
	double step_val;
	sdring<TCHAR> fmt;  
	bool is_wrap_around;
	sdring<TCHAR> helptext;
	
	bool is_cleanup_ready;
	bool is_mouse_hidden;
};

EditboxKAF_err
EditboxPeeker::ctor_params(double min_val, double max_val, double step_val, 
	const TCHAR *szfmt, bool is_wrap_around, const TCHAR *szHelpText)
{
	if(min_val>max_val)
		return EditboxKAF_BadParam;

	this->min_val = min_val;
	this->max_val = max_val;
	this->step_val = step_val;
	fmt = szfmt;
	this->is_wrap_around = is_wrap_around;
	this->helptext = szHelpText;

	this->is_cleanup_ready = false;
	this->is_mouse_hidden = false;

	return EditboxKAF_Succ;
}

inline bool Is_0_9(int c)
{
	return (c>='0' && c<='9') ? true : false;
}

static bool Is_all_0_9(const TCHAR *p, int nchars)
{
	for(int i=0; i<nchars; i++)
	{
		if(!Is_0_9(p[i]))
			return false;
	}
	return true;
}

inline bool Is_decidigit(int c)
{
	if(c>0xff) {
		// This can be an Unicode-char value.
		// VC2010: Passing 0x3011 to isdigit will assert() fail.
		return false;
	}

	return isdigit(c) || c=='.';
}

static bool Is_valid_decimal(const TCHAR *p, int nchars, bool want_neg)
{
	assert(nchars>0);

	int i = 0;
	if(want_neg)
	{
		if(nchars>0 && p[0]=='-')
		{
			// Allow minus sign at start.
			i = 1;
		}
	}

	int dot_count = 0;
	for(; i<nchars; i++)
	{
		if(! Is_decidigit(p[i]))
			return false;

		if(p[i]=='.')
			dot_count++;
	}

	// Note: "3." and ".3" are all value float values in the eye of atof().

	if(dot_count==0 || dot_count==1)
	{
		if(dot_count==nchars)
			return false;   // "."
		else
			return true;
	}
	else
		return false;
}

static int Int_power(int base, int pow)
{
	int ret = 1;
	for(int i=0; i<pow; i++)
		ret *= base;
	return ret;
}

bool IsValidFmt_fg(const TCHAR *szfmt)
{
	int slen = (int)_tcslen(szfmt);
	if(slen<2)
		return false;

	if(szfmt[0]!='%')
		return false;

	if(szfmt[slen-1]!='f' && szfmt[slen-1]!='g')
		return false;

	if(slen==2) // '%f' or '%g'
		return true;

	if(Is_valid_decimal(szfmt+1, slen-2, false))
		return true;
	else
		return false;
}


MsgRelay_et
EditboxPeeker::Edit_OnKey(HWND hwnd, UINT vk, BOOL fDown, int cRepeat, UINT flags)
{
	HWND hEdit = hwnd;

	// We only process WM_KEYDOWN of Up/Down key.
	// If current caret is on a number, we will increase/decrease the number,
	// otherwise, do nothing and relay the message(to its old wndproc).
	
	if(!fDown)
		return Relay_yes;

	bool is_inc = false;
	if(vk==VK_UP)
		is_inc = true;
	else if(vk==VK_DOWN)
		is_inc = false;
	else
		return Relay_yes;

	// Get editbox text length.
	
	const int TBUFSIZE = 100;
	TCHAR szOldText[TBUFSIZE] = {};
	Edit_GetText(hEdit, szOldText, TBUFSIZE-1);
	int textlen = (int)_tcslen(szOldText);
	if(textlen<=0)
		return Relay_yes;
	
	// Send EM_GETSEL to retrieve the selection range (or caret position if no selection)
	int startSel = 0, endSel_ = 0;
	SendMessage(hEdit, EM_GETSEL, (WPARAM)&startSel, (LPARAM)&endSel_);

	vaDBG2(_T("hEdit 0x%08X: [#%d~%d) %s | %.*s"), 
		hEdit, 
		startSel, endSel_, szOldText, // [#%d~%d) %s
		endSel_-startSel, szOldText+startSel // the substring after |
		);

	if(startSel>endSel_) // then swap, not seen this case yet
	{
		DWORD tmp = startSel;
		startSel = endSel_;
		endSel_ = tmp;
	}

	// Now, we check these cases.
	// [1] Caret is placed somewhere in editbox , no text-selection(no invert-color chars).
	//     We search left-and-right to find the value-boundary(VB), and adjust the whole
	//     number inside VB as a float-number.
	//
	// [2] If there is text-selection already, we still search left-and-right(based on 
	//     the selection) to identify the VB.
	//
	// [2.1] If VB equals text-selection, we adjust the whole number as float-number
	//
	// [2.3] If VB is larger than text-selection, that means only partial digits are 
	//       selected, then we consider the selected digits as integer-ring.
	//       For example, editbox has 1.0234, and "02" is select, then 
	//       Up keys will adjust the number to be 1.0334, 1.0434, 1.0534 ...
	//       Down keys will adjust the number to be 1.0134, 1.0034, 1.9934, 1.9834 ...

	int startVB = startSel, endVB_ = endSel_; 
	bool as_int_ring = false;
	bool want_neg = (this->min_val < 0); // is consider '-' as negative-sign
	
	// Special for edge-case of pure caret(=no text-selection) at VB's end_ :
	// We consider the caret is within VB by startVB-- .
	if( startVB==endVB_ && startVB>0)
	{
		if( !Is_decidigit(szOldText[startVB]) && Is_0_9(szOldText[startVB-1]) )
			startVB--;
	}

	// Looking left-side:
	while(startVB>0 && Is_decidigit(szOldText[startVB-1]))
		startVB--;

	if(want_neg) 
	{
		// try to include an extra minus sign at left-side
		if(startVB>0 && szOldText[startVB-1]=='-')
			startVB--;
	}

	// Looking right-size:
	while(endVB_<textlen && Is_decidigit(szOldText[endVB_]))
		endVB_++;

	int lenVB = endVB_-startVB;

	if(startVB==endVB_)
	{
		vaDBG2(_T("Caret pos NOT on valid numeric string, do editbox default."));
		return Relay_yes;
	}
	else if(!Is_valid_decimal(szOldText+startVB, lenVB, want_neg))
	{
		vaDBG2(_T("Caret selection '%.*s' NOT a valid numeric string, do nothing."), 
			lenVB, szOldText+startVB);
		return Relay_no; // Relay_no so that user text-selection is preserved.
	}

	if(startSel==endSel_)
	{
		// Case[1], startVB & endVB_ as probed.
		as_int_ring = false;
	}
	else
	{
		if(endSel_-startSel == lenVB)
		{
			// Case[2.1], do the same as Case[1]
			as_int_ring = false;
		}
		else
		{
			// Case[2.2]
			as_int_ring = true;
		
			if(!Is_all_0_9(szOldText+startSel, endSel_-startSel))
			{
				vaDBG2(_T("Partial selection '%.*s' contains non 0-9 chars, do nothing."), 
					endSel_-startSel, szOldText+startSel);
				return Relay_no;
			}
			
			// Tweak VB's meaning to be user's text-selection
			startVB = startSel; endVB_ = endSel_;
			lenVB = endVB_-startVB;
		}
	}

	TCHAR szOldHot[TBUFSIZE] = {};
	_sntprintf_s(szOldHot, _TRUNCATE, _T("%.*s"), lenVB, szOldText+startVB);
	if(lenVB<=MaxUpDownDigits)
	{
		vaDBG2(_T("hEdit 0x%08X: VB [@%d~%d) %s"), hEdit,  startVB, endVB_,  szOldHot);
	}
	else
	{
		vaDBG2(_T("hEdit 0x%08X: bad VB [@%d~%d) %s (exceed %d)"), hEdit,  startVB, endVB_,  szOldHot, MaxUpDownDigits);
		return Relay_no;
	}

	// Now we increase/decrease the hot number.

	TCHAR szNewHot[TBUFSIZE] = {};
//	const TCHAR *pszNewHot = szNewHot; // pszNewHot may adjust later

	if(as_int_ring)
	{
		assert(lenVB>0);
		int ring_mod = Int_power(10, lenVB); // 10, 100, 1000 etc

		int numHot = _tcstoul(szOldHot, 0, 10);
		int newHot = is_inc ? numHot+1 : numHot-1;
		if(newHot<0) 
		{
			// If newHot goes down from 02, 01, 00 to -01, we rewind -01 to 99(make it positive).
			newHot += ring_mod;
		}

		newHot %= ring_mod; 

		_sntprintf_s(szNewHot, _TRUNCATE, _T("%0*d"), lenVB, newHot);

		vaDBG2(_T("    %s : %s -> %s"), 
			is_inc?_T("IntRing:Inc"):_T("IntRing:Dec"), 
			szOldHot, szNewHot    // %s -> %s
			);
	}
	else // as float number
	{
		double oldHot = _ttof(szOldHot);
		double newHot = is_inc ? oldHot+step_val : oldHot-step_val;

		if(newHot > max_val)
		{
			newHot = is_wrap_around ? min_val : max_val;
		}
		else if(newHot < min_val)
		{
			newHot = is_wrap_around ? max_val : min_val;
		}

		_sntprintf_s(szNewHot, _TRUNCATE, fmt.c_str(), newHot);

		vaDBG2(_T("    KAF (%c%g): %g -> %g (output string: %s)"), 
			is_inc?_T('+'):_T('-'), step_val,
			oldHot, newHot,  // %g -> %g
			szNewHot);
	}

	
	TCHAR szNewText[TBUFSIZE] = {};
	_sntprintf_s(szNewText, _TRUNCATE, _T("%.*s%s%s"), 
		startVB, szOldText,
		szNewHot,
		szOldText + endVB_);

	int hotlen = (int)_tcslen(szNewHot);

	Edit_SetText(hEdit, szNewText);
	Edit_SetSel(hEdit, startVB, startVB+hotlen);
	
	if(is_cleanup_ready)
	{
		HideMouse();
	}
	
	return Relay_no;
}

MsgRelay_et
EditboxPeeker::Edit_OnMouseMove(HWND hEdit, int x, int y, UINT keyFlags)
{
	if(! is_cleanup_ready)
	{
		// Establish WM_MOUSELEAVE tracking.
		TRACKMOUSEEVENT tme = {sizeof(tme), TME_LEAVE, hEdit};
		TrackMouseEvent(&tme);

		is_cleanup_ready = true;
	}

	return Relay_yes;
}

MsgRelay_et
EditboxPeeker::Edit_OnNCDestroy(HWND hEdit)
{
	EditboxKAF_err err = Editbox_DisableKbdAdjustFloatnum(hEdit);
    assert(!err);

	return Relay_yes;
}

// Custom window procedure for the edit control
LRESULT 
EditboxPeeker::WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HWND hEdit = hwnd;
	
    switch (uMsg)
	{
		SUBCLASS_FILTER_MSG0(hwnd, WM_KEYDOWN, Edit_OnKey);
		SUBCLASS_FILTER_MSG0(hwnd, WM_MOUSEMOVE, Edit_OnMouseMove);
    	SUBCLASS_FILTER_MSG0(hwnd, WM_NCDESTROY, Edit_OnNCDestroy);
	
	case WM_MOUSELEAVE: 
		ShowMouse();
		is_cleanup_ready = false;
		break;
	}

    // Call into existing WndProc chain.
    return DefSubclassProc(hwnd, uMsg, wParam, lParam);
}


const TCHAR* KAF_GetTooltipText(HWND hEdit, void *userctx)
{
	(void)userctx;
	CxxWindowSubclass::ReCode_et err = CxxWindowSubclass::E_Fail;
	EditboxKAF::EditboxPeeker *peeker = 
		CxxWindowSubclass::FetchCxxobjFromHwnd<EditboxKAF::EditboxPeeker>(
		hEdit, EditboxKAF::s_sigSubclass, 
		FALSE, // FALSE: bcz hEdit must have been created
		&err
		);
	assert(err==CxxWindowSubclass::E_Existed);

	return peeker->GetTooltipText();
}


EditboxKAF_err _Editbox_EnableKbdAdjustFloatnum(HWND hEdit,
	double min_val, double max_val, double step_val, const TCHAR *szfmt,
	bool is_wrap_around, const TCHAR *szHelpText)
{
	if(!IsWindow(hEdit))
		return EditboxKAF_BadParam;

	if(!IsValidFmt_fg(szfmt))
		return EditboxKAF_BadParam;

	CxxWindowSubclass::ReCode_et err = CxxWindowSubclass::E_Fail;
	EditboxPeeker *peeker = CxxWindowSubclass::FetchCxxobjFromHwnd<EditboxPeeker>(
		hEdit, s_sigSubclass, 
		TRUE, 
		&err);

	if(err==CxxWindowSubclass::E_Existed)
		return EditboxKAF_AlreadyEnabled;

	if(!peeker)
		return EditboxKAF_Unknown;

	EditboxKAF_err err2 =
		peeker->ctor_params(min_val, max_val, step_val, szfmt, is_wrap_around, szHelpText);
	if(err2)
		return err2;

	Dlgtte_err tterr = Dlgtte_EnableTooltip(hEdit, NULL, 0, KAF_GetTooltipText, 0,
		Dlgtte_BalloonDown|Dlgtte_AutoContentTipOnFocus);
	assert(!tterr);

	return EditboxKAF_Succ;
}

EditboxKAF_err _Editbox_DisableKbdAdjustFloatnum(HWND hEdit)
{
	if(!IsWindow(hEdit))
		return EditboxKAF_BadParam;

	CxxWindowSubclass::ReCode_et err = CxxWindowSubclass::E_Fail;
	EditboxPeeker *peeker = CxxWindowSubclass::FetchCxxobjFromHwnd<EditboxPeeker>(
		hEdit, s_sigSubclass, 
		FALSE, 
		&err);

	if(err==CxxWindowSubclass::E_NotExist)
		return EditboxKAF_NotEnabledYet;
	
	peeker->DetachHwnd(true);

	return EditboxKAF_Succ;
}


const TCHAR* EditboxPeeker::GetTooltipText()
{
	static TCHAR s_sztooltip[4000];

	TCHAR szfmtParamHint[100];
	_sntprintf_s(szfmtParamHint, _TRUNCATE,
		_T("Step: %%g\n")
		_T("Min: %s , Max: %s\n")
		,
		fmt.c_str(), fmt.c_str());

	_sntprintf_s(s_sztooltip, _TRUNCATE, szfmtParamHint,
		step_val,
		min_val, max_val);

	int htlen = this->helptext.len();
	const TCHAR *pszUser = this->helptext.c_str();
	if(htlen>0 && !(htlen==1 && pszUser[0]=='\n'))
	{
		_sntprintf_s(s_sztooltip, _TRUNCATE, _T("%s\n%s"), s_sztooltip, pszUser);
	}

	if(htlen>0 && pszUser[htlen-1]=='\n')
	{
		const TCHAR *szKbdHint =
			_T("Use Up/Down key to adjust value step-by-step.\n")
			_T("You can select partial digits and adjust them only.");
		_sntprintf_s(s_sztooltip, _TRUNCATE, _T("%s\n%s"), s_sztooltip, szKbdHint);
	}

	return s_sztooltip;
}


////////////////////////////////////////////////////////////////////////////
} // namespace EditboxKAF
////////////////////////////////////////////////////////////////////////////



/*
 ,-. .     .       .                          ,.  ;-.  ,
/    |     |       |                         /  \ |  ) |
| -. | ,-. |-. ,-: |   ,-. ;-. ,-: ,-. ,-.   |--| |-'  |
\  | | | | | | | | |   `-. | | | | |   |-'   |  | |    |
 `-' ' `-' `-' `-` '   `-' |-' `-` `-' `-'   '  ' '    '
*/

// Global space API implementation wrapper:

EditboxKAF_err Editbox_EnableKbdAdjustFloatnum(HWND hEdit,
	double min_val, double max_val, double step_val, const TCHAR *szfmt,
	bool is_wrap_around, const TCHAR *szHelpText)
{
	return EditboxKAF::_Editbox_EnableKbdAdjustFloatnum(hEdit,
		min_val, max_val, step_val, szfmt, is_wrap_around, szHelpText);
}

EditboxKAF_err Editbox_DisableKbdAdjustFloatnum(HWND hEdit)
{
	return EditboxKAF::_Editbox_DisableKbdAdjustFloatnum(hEdit);
}



#ifndef Editbox_EnableKbdAdjustFloatnum_DEBUG
#include <CHHI_vaDBG_show.h>
#endif

#endif // [IMPL]

#endif
