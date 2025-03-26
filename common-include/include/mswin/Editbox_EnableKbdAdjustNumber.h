#ifndef __Editbox_EnableKbdAdjustNumber_h_20250326_
#define __Editbox_EnableKbdAdjustNumber_h_20250326_

#include <windows.h>

enum EditboxKAN_err 
{
	EditboxKAN_Succ = 0,

	// Error on enable
	EditboxKAN_Unknown = 1,
	EditboxKAN_NoMem = 2,
	EditboxKAN_AlreadyEnabled = 3,
	EditboxKAN_SetProp = 4, // WinAPI SetProp() error

	// Error on disable
	EditboxKAN_NotEnabledYet = 10,
	EditboxKAN_ChainMoved = 11,
};

// Purpose: Add extra keyboard functionality to a standard editbox.
// When the caret is a number, user pressing Up will increase the number,
// pressing Down will decrease the number. This is convenient for adjusting
// date/time value on an editbox.

EditboxKAN_err Editbox_EnableKbdAdjustNumber(HWND hEdit,
	int min_val, int max_val, bool is_wrap_around, bool is_pad_0);

EditboxKAN_err Editbox_DisableKbdAdjustNumber(HWND hEdit);
// -- optional, Editbox's WM_NCDESTROY will call this automatically



///////////////////////////////////////////////////////////////
// Implementation Below:
///////////////////////////////////////////////////////////////

#ifdef Editbox_EnableKbdAdjustNumber_IMPL

#include <assert.h>
#include <stdarg.h>
#include <tchar.h>
#include <WindowsX.h>
#include <mswin/WindowsX2.h> // chj: for SUBCLASS_FILTER_MSG0()

#ifdef Editbox_EnableKbdAdjustNumber_DEBUG
/*
#define vaDBG vaDbgTs
#include <mswin/winuser.itc.h>
*/
#else
#ifndef vaDBG
# define vaDBG(...)
#endif
#ifndef ITCSv
# define ITCSv(...) 0
#endif
#endif // not Editbox_EnableKbdAdjustNumber_DEBUG


namespace EditboxKAN
{

enum { HEXMAGIC = 0x20241024 };
enum { SZFMT_MAX_= 8 };
enum { MaxUpDownDigits = 6 };

// Keyword used to store the original WndProc in the window's property-list
const TCHAR* MYEDITBOX_PROP_STR = _T("WndProc_before_EnableKbdAdjustNumber");

struct EditCustProp_st
{
	UINT hexmagic;
	WNDPROC wndproc_was;
	int min_val;
	int max_val;
	bool is_wrap_around;
	bool is_pad_0; // "5" becomes "05" or "005" etc, according to hot length

	bool is_cleanup_ready;
	bool is_mouse_hidden;

	EditCustProp_st(WNDPROC wndproc_was,
		int min_val, int max_val, bool is_wrap_around, bool is_pad_0)
	{
		this->hexmagic = HEXMAGIC;
		this->wndproc_was = wndproc_was;
		this->min_val = min_val;
		this->max_val = max_val;
		this->is_wrap_around = is_wrap_around;
		this->is_pad_0 = is_pad_0;

		this->is_cleanup_ready = false;
		this->is_mouse_hidden = false;
	}

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
};


static bool is_all_number_digits(const TCHAR *p, int nchars)
{
	int i;
	for(i=0; i<nchars; i++)
	{
		if(! isdigit(p[i]))
			return false;
	}
	return true;
}

static MsgRelay_et Edit_OnKey(HWND hwnd, UINT vk, BOOL fDown, int cRepeat, UINT flags)
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

	vaDBG(_T("hEdit 0x%08X: [#%d~%d) %s | %.*s"), hEdit, startPos, endPos, szText, 
		endPos-startPos, szText+startPos // the substring after |
		);

	if(startPos>endPos) // then swap, not seen this case yet
	{
		DWORD tmp = startPos;
		startPos = endPos;
		endPos = tmp;
	}

	// Now, we check two cases.
	// Case 1: If user has explicitly select(higlighted) some numbered text,
	//         we process only that selected text.
	// Case 2: If user has selected nothing, we find a whole number string
	//         around the caret and process that whole number.

	int startHot = startPos, endHot_ = endPos; // The hot section is what we operate.
	
	if(startPos<endPos) // user has explicit selection
	{
		if(! is_all_number_digits(szText+startPos, endPos-startPos))
		{
			return Relay_no;
		}
	}
	else // find number around caret
	{
		// caret pos or pos[-1] needs to be a digit
		if(! (
			isdigit(szText[startPos]) || (startPos>0 && isdigit(szText[startPos-1]))
			) ) 
		{
			vaDBG(_T("Caret pos NOT on a digit, do nothing."));
			return Relay_yes;
		}
		
		// Looking left-side:
		while(startHot>0 && isdigit(szText[startHot-1]))
			startHot--;

		// Looking right-size:
		while(endHot_<textlen && isdigit(szText[endHot_]))
			endHot_++;
	}

	int hotlen = (int)(endHot_-startHot);

	TCHAR szHot[TBUFSIZE] = {};
	_sntprintf_s(szHot, _TRUNCATE, _T("%.*s"), hotlen, szText+startHot);
	
	if(hotlen<=MaxUpDownDigits)
	{
		vaDBG(_T("hEdit 0x%08X: hot [@%d~%d) %s"), hEdit,	startHot, endHot_, szHot);
	}
	else
	{
		vaDBG(_T("hEdit 0x%08X: bad [@%d~%d) %s (exceed %d)"), hEdit, startHot, endHot_, szHot, MaxUpDownDigits);
		return Relay_yes;
	}

	// Now we increase/decrease the hot number.

	EditCustProp_st *myprop = (EditCustProp_st*)GetProp(hEdit, MYEDITBOX_PROP_STR);
	
	int numHot = _tcstoul(szHot, 0, 10);
	int newHot = is_inc ? numHot+1 : numHot-1;
	if(newHot > myprop->max_val)
	{
		newHot = myprop->is_wrap_around ? myprop->min_val : myprop->max_val;
	}
	else if(newHot < myprop->min_val)
	{
		newHot = myprop->is_wrap_around ? myprop->max_val : myprop->min_val;
	}

	assert(hotlen>0);
	TCHAR szFmt[20] = _T("%d");
	if(myprop->is_pad_0)
		_sntprintf_s(szFmt, _TRUNCATE, _T("%%0%ud"), hotlen);
	
	TCHAR szNewHot[TBUFSIZE] = {};
	const TCHAR *pszNewHot = szNewHot; // may adjust
	_sntprintf_s(szNewHot, _TRUNCATE, szFmt, newHot);
	int hotlenv = (int)_tcslen(szNewHot); // v: (this len could be) verbose

	if(hotlenv > hotlen)
	{
		// For example, there is "52" at carent, but user select only "5"(hotlen==1) and then increase it.
		// Then, when hotstring goes from 5,6,7... and reaches "10", we should chop off the "1"
		// and preserve only the "0", bcz strlen("10") has exceeded hotlen.
		pszNewHot = szNewHot + hotlenv - hotlen;
	}

	vaDBG(_T("    %s : %s -> %s"), is_inc?_T("INC"):_T("DEC"), szHot, pszNewHot);

	TCHAR szNewText[TBUFSIZE] = {};
	
	_sntprintf_s(szNewText, _TRUNCATE, _T("%.*s%s%s"), 
		startHot, szText,
		pszNewHot,
		szText + endHot_);

	Edit_SetText(hEdit, szNewText);
	Edit_SetSel(hEdit, startHot, endHot_);
	
	if(myprop->is_cleanup_ready)
	{
		myprop->HideMouse();
	}
	
	return Relay_no;
}

MsgRelay_et Edit_OnMouseMove(HWND hEdit, int x, int y, UINT keyFlags)
{
	EditCustProp_st *myprop = (EditCustProp_st*)GetProp(hEdit, MYEDITBOX_PROP_STR);

	if(! myprop->is_cleanup_ready)
	{
		// Establish WM_MOUSELEAVE tracking.
		TRACKMOUSEEVENT tme = {sizeof(tme), TME_LEAVE, hEdit};
		TrackMouseEvent(&tme);

		myprop->is_cleanup_ready = true;
	}

	return Relay_yes;
}

MsgRelay_et Edit_OnNCDestroy(HWND hEdit)
{
	EditboxKAN_err err = Editbox_DisableKbdAdjustNumber(hEdit);
    assert(!err);
	return Relay_yes;
}

// Custom window procedure for the edit control
LRESULT CALLBACK Edit_MyWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HWND hEdit = hwnd;
	
    // Retrieve the original WndProc from the window's properties
	//
	EditCustProp_st *myprop = (EditCustProp_st*)GetProp(hEdit, MYEDITBOX_PROP_STR);
    WNDPROC oldWndProc = myprop->wndproc_was;
	bool succ = false;

    switch (uMsg)
	{
		SUBCLASS_FILTER_MSG0(hwnd, WM_KEYDOWN, Edit_OnKey);
		SUBCLASS_FILTER_MSG0(hwnd, WM_MOUSEMOVE, Edit_OnMouseMove);
    	SUBCLASS_FILTER_MSG0(hwnd, WM_NCDESTROY, Edit_OnNCDestroy);
	
	case WM_MOUSELEAVE: 
		myprop->ShowMouse();
		myprop->is_cleanup_ready = false;
		break;
	}

    // Call the original WndProc for default processing
    return CallWindowProc(oldWndProc, hEdit, uMsg, wParam, lParam);
}


EditboxKAN_err _Editbox_EnableKbdAdjustNumber(HWND hEdit,
	int min_val, int max_val, bool is_wrap_around, bool is_pad_0)
{
	EditCustProp_st *myprop = (EditCustProp_st*)GetProp(hEdit, MYEDITBOX_PROP_STR);
	if(myprop)
		return EditboxKAN_AlreadyEnabled;

	myprop = new EditCustProp_st(nullptr,
		min_val, max_val, is_wrap_around, is_pad_0);
	if(!myprop)
		return EditboxKAN_NoMem;

    WNDPROC oldWndProc = SubclassWindow(hEdit, Edit_MyWndProc);

	myprop->wndproc_was = oldWndProc;
	
    BOOL succ = SetProp(hEdit, MYEDITBOX_PROP_STR, myprop); // Store custom props into hEdit.
	if(succ)
	{
		return EditboxKAN_Succ;
	}
	else
	{	// restore old state
		SubclassWindow(hEdit, oldWndProc);
		delete myprop;
		return EditboxKAN_SetProp;
	}
}

EditboxKAN_err _Editbox_DisableKbdAdjustNumber(HWND hEdit)
{
	WNDPROC currentWndProc = (WNDPROC)GetWindowLongPtr(hEdit, GWLP_WNDPROC);
	EditCustProp_st *myprop = (EditCustProp_st*)GetProp(hEdit, MYEDITBOX_PROP_STR);

	if(!myprop)
		return EditboxKAN_NotEnabledYet;

	if(currentWndProc != Edit_MyWndProc)
	{
		// Someone else has further subclassed this hEdit, so we should not unsubclass it.
		return EditboxKAN_ChainMoved;
	}

	assert(myprop->hexmagic==HEXMAGIC);
	assert(myprop->wndproc_was);

	// Restore original WndProc for editbox
	SubclassWindow(hEdit, myprop->wndproc_was);

	// Remove our custom wnd-prop
	RemoveProp(hEdit, MYEDITBOX_PROP_STR);

	// Destroy our custom struct
	delete myprop;
	
	return EditboxKAN_Succ;
}


} // namespace

// non-namespace wrapper:
//
EditboxKAN_err Editbox_EnableKbdAdjustNumber(HWND hEdit,
	int min_val, int max_val, bool is_wrap_around, bool is_pad_0)
{
	return EditboxKAN::_Editbox_EnableKbdAdjustNumber(hEdit,
		min_val, max_val, is_wrap_around, is_pad_0);
}

EditboxKAN_err Editbox_DisableKbdAdjustNumber(HWND hEdit)
{
	return EditboxKAN::_Editbox_DisableKbdAdjustNumber(hEdit);
}

#endif // Editbox_EnableKbdAdjustNumber_IMPL

#endif // __Editbox_EnableKbdAdjustNumber_h_
