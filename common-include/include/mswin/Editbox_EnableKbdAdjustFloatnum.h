#ifndef CHHI__Editbox_EnableKbdAdjustFloatnum_h_20260309_
#define CHHI__Editbox_EnableKbdAdjustFloatnum_h_20260309_

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
	const TCHAR *fmt=_T("%f"), // %f, %g, %.3f etc
	bool is_wrap_around=false);

EditboxKAF_err Editbox_DisableKbdAdjustFloatnum(HWND hEdit);
// -- optional, Editbox's WM_NCDESTROY will call this automatically



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
#define CxxWindowSubclass_IMPL
#include <mswin/CxxWindowSubclass.h>
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
	void ctor_params(double min_val, double max_val, double step_val, 
		const TCHAR *fmt, bool is_wrap_around);

	virtual LRESULT WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

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
	const TCHAR *fmt;
	bool is_wrap_around;
	
	bool is_cleanup_ready;
	bool is_mouse_hidden;
};

void
EditboxPeeker::ctor_params(double min_val, double max_val, double step_val, 
	const TCHAR *fmt, bool is_wrap_around)
{
	this->min_val = min_val;
	this->max_val = max_val;
	this->step_val = step_val;
	this->fmt = fmt;
	this->is_wrap_around = is_wrap_around;

	this->is_cleanup_ready = false;
	this->is_mouse_hidden = false;
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

static bool Is_valid_decimal(const TCHAR *p, int nchars)
{
	int i = 0;
	if(nchars>0 && p[0]=='-')
	{
		// Allow minus sign at start.
		i = 1;
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
	TCHAR szText[TBUFSIZE] = {};
	Edit_GetText(hEdit, szText, TBUFSIZE-1);
	int textlen = (int)_tcslen(szText);
	if(textlen<=0)
		return Relay_yes;
	
	// Send EM_GETSEL to retrieve the selection range (or caret position if no selection)
	int startPos = 0, endPos = 0;
	SendMessage(hEdit, EM_GETSEL, (WPARAM)&startPos, (LPARAM)&endPos);

	vaDBG2(_T("hEdit 0x%08X: [#%d~%d) %s | %.*s"), 
		hEdit, 
		startPos, endPos, szText, // [#%d~%d) %s
		endPos-startPos, szText+startPos // the substring after |
		);

	if(startPos>endPos) // then swap, not seen this case yet
	{
		DWORD tmp = startPos;
		startPos = endPos;
		endPos = tmp;
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
	//       Down keys will adjust the nubmer to be 1.0134, 1.0034, 1.9934, 1.9834 ...

	int startHot = startPos, endHot_ = endPos; // The hot section is what we operate.
	bool as_int_ring = false;
	
	// Special for edge-case of pure caret(=no text-selection) at VB's end_
	if( startHot==endHot_ && startHot>0)
	{
		if(!Is_decidigit(szText[startHot]) && Is_0_9(szText[startHot-1])) 
			startHot--;
	}

	// Looking left-side:
	while(startHot>0 && Is_decidigit(szText[startHot-1]))
		startHot--;

	if(this->min_val < 0) 
	{
		// try to include an extra minus sign at left-side
		if(startHot>0 && szText[startHot-1]=='-')
			startHot--;
	}

	// Looking right-size:
	while(endHot_<textlen && Is_decidigit(szText[endHot_]))
		endHot_++;

	if(startHot==endHot_)
	{
		vaDBG2(_T("Caret pos NOT on numeric string, do nothing."));
		return Relay_yes;
	}
	else if(!Is_valid_decimal(szText+startHot, endHot_-startHot))
	{
		vaDBG2(_T("Caret selection '%.*s' NOT a numeric string, do nothing."), 
			endHot_-startHot, szText+startHot);
		return Relay_yes;
	}

	if(startPos==endPos)
	{
		// Case[1], startHot & endHot_ already settled.
		as_int_ring = false;
	}
	else
	{
		if(endPos-startPos == endHot_-startHot)
		{
			// Case[2.1], do nothing, startHot & endHot_ already settled.
			as_int_ring = false;
		}
		else
		{
			// Case[2.2]
			as_int_ring = true;
		
			if(!Is_all_0_9(szText+startPos, endPos-startPos))
			{
				vaDBG2(_T("Partial selection '%.*s' contains non 0-9 chars, do nothing."), 
					endPos-startPos, szText+startPos);
				return Relay_no;
			}
			
			// Tweak VB's meaning to be user's text-selection
			startHot = startPos; endHot_ = endPos;
		}
	}

	int hotlen = (int)(endHot_-startHot);

	TCHAR szHot[TBUFSIZE] = {};
	_sntprintf_s(szHot, _TRUNCATE, _T("%.*s"), hotlen, szText+startHot);
	if(hotlen<=MaxUpDownDigits)
	{
		vaDBG2(_T("hEdit 0x%08X: hot [@%d~%d) %s"), hEdit,	startHot, endHot_, szHot);
	}
	else
	{
		vaDBG2(_T("hEdit 0x%08X: bad [@%d~%d) %s (exceed %d)"), hEdit, startHot, endHot_, szHot, MaxUpDownDigits);
		return Relay_no;
	}

	// Now we increase/decrease the hot number.

	TCHAR szNewHot[TBUFSIZE] = {};
	const TCHAR *pszNewHot = szNewHot; // may adjust later

	if(as_int_ring)
	{
		int numHot = _tcstoul(szHot, 0, 10);
		int newHot = is_inc ? numHot+1 : numHot-1;
		if(newHot<0) 
		{
			// If newHot goes down from 02, 01, 00 to -01, we rewind it to 99.
			newHot += Int_power(10, endHot_-newHot);
		}

		assert(hotlen>0);
		_sntprintf_s(szNewHot, _TRUNCATE, _T("%0*d"), hotlen, newHot);
		int hotlenv = (int)_tcslen(szNewHot); // v: (this len could be) verbose

		if(hotlenv > hotlen)
		{
			// For example, there is "52" at caret, but user select only "5"(hotlen==1) and then increase it.
			// Then, when hotstring goes from 5,6,7... and reaches "10", we should chop off the "1"
			// and preserve only the "0", bcz strlen("10") has exceeded hotlen.
			pszNewHot = szNewHot + hotlenv - hotlen;
		}

		vaDBG2(_T("    %s : %s -> %s"), is_inc?_T("KAF:Inc"):_T("KAF:Dec"), szHot, pszNewHot);
	}
	else // as float number
	{
		double numHot = _ttof(szHot);
		double newHot = is_inc ? numHot+step_val : numHot-step_val;

		if(newHot > max_val)
		{
			newHot = is_wrap_around ? min_val : max_val;
		}
		else if(newHot < min_val)
		{
			newHot = is_wrap_around ? max_val : min_val;
		}

		_sntprintf_s(szNewHot, _TRUNCATE, this->fmt, newHot);
		// --TODO: Check this->fmt's validity, avoid user-input attack.

		hotlen = (int)_tcslen(szNewHot);

		vaDBG2(_T("    KAF (%c%f): %f -> %f (output string: %s)"), 
			is_inc?_T('+'):_T('-'), step_val,
			numHot, newHot,  // %f -> %f
			szNewHot);
	}

	
	TCHAR szNewText[TBUFSIZE] = {};
	_sntprintf_s(szNewText, _TRUNCATE, _T("%.*s%s%s"), 
		startHot, szText,
		pszNewHot,
		szText + endHot_);

	Edit_SetText(hEdit, szNewText);
	Edit_SetSel(hEdit, startHot, startHot+hotlen);
	
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


EditboxKAF_err _Editbox_EnableKbdAdjustFloatnum(HWND hEdit,
	double min_val, double max_val, double step_val, const TCHAR *fmt,
	bool is_wrap_around)
{
	if(!IsWindow(hEdit))
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

	peeker->ctor_params(min_val, max_val, step_val, fmt, is_wrap_around);

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
	double min_val, double max_val, double step_val, const TCHAR *fmt,
	bool is_wrap_around)
{
	return EditboxKAF::_Editbox_EnableKbdAdjustFloatnum(hEdit,
		min_val, max_val, step_val, fmt, is_wrap_around);
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
