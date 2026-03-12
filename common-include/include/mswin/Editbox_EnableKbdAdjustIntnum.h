#ifndef __CHHI__Editbox_EnableKbdAdjustIntnum_h_20250326_20260312_
#define __CHHI__Editbox_EnableKbdAdjustIntnum_h_20250326_20260312_

#include <windows.h>

enum EditboxKAN_err 
{
	EditboxKAN_Succ = 0,

	// Error on enable
	EditboxKAN_Unknown = 1,
	EditboxKAN_NoMem = 2,
	EditboxKAN_AlreadyEnabled = 3,
	EditboxKAN_BadParam = 4, 

	// Error on disable
	EditboxKAN_NotEnabledYet = 10,
	EditboxKAN_ChainMoved = 11,
};

// Purpose: Add extra keyboard functionality to a standard editbox.
// When the caret is on a digit char, user pressing Up will increase the integer number,
// pressing Down will decrease the integer number. This is convenient for adjusting
// date/time value on an editbox etc.

EditboxKAN_err Editbox_EnableKbdAdjustIntnum(HWND hEdit,
	int min_val, int max_val, unsigned int step_val, bool is_wrap_around, 
	unsigned int pad_zeros_to_width=0);

EditboxKAN_err Editbox_DisableKbdAdjustIntnum(HWND hEdit);
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

#if defined(Editbox_EnableKbdAdjustIntnum_IMPL) || (defined CHHI_ALL_IMPL && !defined CHHI_ALL_IMPL_HIDE_Editbox_EnableKbdAdjustIntnum) // [IMPL]

// >>> Include headers required by this lib's implementation
#include <assert.h>
#include <stdarg.h>
#include <tchar.h>
#include <WindowsX.h>
//
#include <_MINMAX_.h>
#include <mswin/WindowsX2.h> // chj: for SUBCLASS_FILTER_MSG0()
#include <mswin/CxxWindowSubclass.h>
// <<< Include headers required by this lib's implementation


#ifndef Editbox_EnableKbdAdjustIntnum_DEBUG
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
namespace EditboxKAN { 
////////////////////////////////////////////////////////////////////////////
// (private namespace 'EditboxKAN') 


//enum { SZFMT_MAX_= 8 };
enum { MaxUpDownDigits = 9 };

const TCHAR *s_sigSubclass = _T("sig_EditboxKAN");

class EditboxPeeker : public CxxWindowSubclass
{
public:
	EditboxPeeker() {}
	EditboxKAN_err ctor_params(int min_val, int max_val, unsigned int step_val,
		bool is_wrap_around, unsigned int pad_zeros);

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
	int min_val;
	int max_val;
	unsigned int step_val;
	bool is_wrap_around;
	Uint pad_zeros; // if 3, "9" becomes "009"; if 0, no padding zero

	bool is_cleanup_ready;
	bool is_mouse_hidden;
};

EditboxKAN_err
EditboxPeeker::ctor_params(int min_val, int max_val, unsigned int step_val, 
	bool is_wrap_around, unsigned int pad_zeros)
{
	if(min_val>max_val)
		return EditboxKAN_BadParam;

	if(max_val-min_val<0)
		return EditboxKAN_BadParam;

	if((Uint)(max_val-min_val) < step_val)
		return EditboxKAN_BadParam;

	this->min_val = min_val;
	this->max_val = max_val;
	this->step_val = step_val;
	this->is_wrap_around = is_wrap_around;
	this->pad_zeros = pad_zeros;

	this->is_cleanup_ready = false;
	this->is_mouse_hidden = false;

	return EditboxKAN_Succ;
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

static bool Is_valid_integer(const TCHAR *p, int nchars, bool want_neg)
{
	assert(nchars>0);

	int i = 0;
	if(want_neg)
	{
		if(nchars>0 && p[0]=='-')
		{
			// Allow minus sign at start.
			i = 1;

			if(nchars==1)
				return false;
		}
	}

	return Is_all_0_9(p+i, nchars-1);
}

static int Int_power(int base, int pow)
{
	int ret = 1;
	for(int i=0; i<pow; i++)
		ret *= base;
	return ret;
}

static bool _xxx___is_all_number_digits(const TCHAR *p, int nchars)
{
	int i;
	for(i=0; i<nchars; i++)
	{
		if(! isdigit(p[i]))
			return false;
	}
	return true;
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
	//     number inside VB as a integer(maybe >0 or <0).
	//
	// [2] If there is text-selection already, we still search left-and-right(based on 
	//     the selection) to identify the VB.
	//
	// [2.1] If VB equals text-selection, we adjust the whole number as an integer.
	//
	// [2.3] If VB is larger than text-selection, that means only partial digits are 
	//       selected, then we consider the selected digits as integer-ring(as_int_ring=true).
	//       For example, editbox has 10234, and "02" is select, then 
	//       Up keys will adjust the number to be 10334, 10434, 10534 ...
	//       Down keys will adjust the number to be 10134, 10034, 19934, 19834 ...

	int startVB = startSel, endVB_ = endSel_; 
	bool as_int_ring = false;
	bool want_neg = (this->min_val < 0); // is consider '-' as negative-sign

	// Special for edge-case of pure caret(=no text-selection) at VB's end_ :
	// We consider the caret is within VB by startVB-- .
	if( startVB==endVB_ && startVB>0 )
	{
		if( !Is_0_9(szOldText[startVB]) && Is_0_9(szOldText[startVB-1]) )
			startVB--;
	}

	// Looking left-side:
	if(szOldText[startVB]!='-')
	{
		while(startVB>0 && Is_0_9(szOldText[startVB-1]))
			startVB--;

		if(want_neg) 
		{
			// try to include an extra minus sign at left-side
			if(startVB>0 && szOldText[startVB-1]=='-')
				startVB--;
		}
	}

	// Looking right-size:
	while(endVB_<textlen && Is_0_9(szOldText[endVB_]))
		endVB_++;

	int lenVB = endVB_ - startVB;
	int lenSel = endSel_ - startSel;

	if(lenVB==0)
	{
		// no number-string around caret or text-selection
		vaDBG2(_T("Caret pos NOT on valid integer string, do editbox default."));
		return Relay_yes;
	}
// 	else if(!Is_valid_integer(szOldText+startVB, lenVB, want_neg))
// 	{
// 		vaDBG2(_T("Caret selection '%.*s' NOT a valid integer string, do nothing."), 
// 			lenVB, szOldText+startVB);
// 		return Relay_no; // Relay_no so that user text-selection is preserved.
// 	}

	if(startSel==endSel_)
	{
		// Case[1], startVB & endVB_ as probed.
		as_int_ring = false;
	}
	else
	{
		if(lenSel == lenVB)
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
	else // as whole integer
	{
		int oldHot = _ttoi(szOldHot);
		int newHot = 0;

		if(oldHot>=min_val && oldHot<=max_val)
		{
			// initial num in range
			newHot = is_inc ? oldHot+step_val : oldHot-step_val;
		}
		else
		{
			// initial num out of range, now drag it into range
			newHot = _MID_(min_val, oldHot, max_val);
		}

		if(is_wrap_around)
		{
			int range = (max_val - min_val + 1);
			if(newHot < min_val)
				newHot += range;

			newHot = (newHot - min_val) % range + min_val;
		}
		else
		{
			if(newHot>max_val)
				newHot = max_val;
			if(newHot<min_val)
				newHot = min_val;
		}

		if(newHot>=0)
		{
			if(pad_zeros)
				_sntprintf_s(szNewHot, _TRUNCATE, _T("%0*d"), pad_zeros, newHot);
			else
				_sntprintf_s(szNewHot, _TRUNCATE, _T("%d"), newHot);
		}
		else
		{
			if(pad_zeros)
				_sntprintf_s(szNewHot, _TRUNCATE, _T("-%0*d"), pad_zeros, -newHot);
			else
				_sntprintf_s(szNewHot, _TRUNCATE, _T("-%d"), -newHot);
		}

		vaDBG2(_T("    KAN (%c%u): %d -> %d (output string: %s)"), 
			is_inc?_T('+'):_T('-'), step_val,
			oldHot, newHot,  // %d -> %d
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
	EditboxKAN_err err = Editbox_DisableKbdAdjustIntnum(hEdit);
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


EditboxKAN_err _Editbox_EnableKbdAdjustIntnum(HWND hEdit,
	int min_val, int max_val, unsigned int step_val, bool is_wrap_around, 
	unsigned int pad_zeros)
{
	if(!IsWindow(hEdit))
		return EditboxKAN_BadParam;

	CxxWindowSubclass::ReCode_et err = CxxWindowSubclass::E_Fail;
	EditboxPeeker *peeker = CxxWindowSubclass::FetchCxxobjFromHwnd<EditboxPeeker>(
		hEdit, s_sigSubclass, 
		TRUE, 
		&err);

	if(err==CxxWindowSubclass::E_Existed)
		return EditboxKAN_AlreadyEnabled;

	if(!peeker)
		return EditboxKAN_Unknown;

	EditboxKAN_err err2 =
		peeker->ctor_params(min_val, max_val, step_val, is_wrap_around, pad_zeros);
	if(err2)
		return err2;

	return EditboxKAN_Succ;
}

EditboxKAN_err _Editbox_DisableKbdAdjustIntnum(HWND hEdit)
{
	if(!IsWindow(hEdit))
		return EditboxKAN_BadParam;

	CxxWindowSubclass::ReCode_et err = CxxWindowSubclass::E_Fail;
	EditboxPeeker *peeker = CxxWindowSubclass::FetchCxxobjFromHwnd<EditboxPeeker>(
		hEdit, s_sigSubclass, 
		FALSE, 
		&err);

	if(err==CxxWindowSubclass::E_NotExist)
		return EditboxKAN_NotEnabledYet;
	
	peeker->DetachHwnd(true);

	return EditboxKAN_Succ;
}


////////////////////////////////////////////////////////////////////////////
} // namespace EditboxKAN
////////////////////////////////////////////////////////////////////////////


/*
 ,-. .     .       .                          ,.  ;-.  ,
/    |     |       |                         /  \ |  ) |
| -. | ,-. |-. ,-: |   ,-. ;-. ,-: ,-. ,-.   |--| |-'  |
\  | | | | | | | | |   `-. | | | | |   |-'   |  | |    |
 `-' ' `-' `-' `-` '   `-' |-' `-` `-' `-'   '  ' '    '
*/

// Global space API implementation wrapper:

EditboxKAN_err Editbox_EnableKbdAdjustIntnum(HWND hEdit,
	int min_val, int max_val, unsigned int step_val, bool is_wrap_around, 
	unsigned int pad_zeros_to_width)
{
	return EditboxKAN::_Editbox_EnableKbdAdjustIntnum(hEdit,
		min_val, max_val, step_val, is_wrap_around, pad_zeros_to_width);
}

EditboxKAN_err Editbox_DisableKbdAdjustIntnum(HWND hEdit)
{
	return EditboxKAN::_Editbox_DisableKbdAdjustIntnum(hEdit);
}



#ifndef Editbox_EnableKbdAdjustIntnum_DEBUG
#include <CHHI_vaDBG_show.h>
#endif

#endif // [IMPL]

#endif
