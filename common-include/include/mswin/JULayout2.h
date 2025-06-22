#ifndef __JULayout2_h_20250622_
#define __JULayout2_h_20250622_

/******************************************************************************
Original: UILayout.h
Notices: Copyright (c) 2000 Jeffrey Richter
Purpose: This class manages child window positioning and sizing when a parent window
         (normally a dialogbox) is resized.
         See Appendix B of his book PSSA2000.

Updates by Jimm Chen (as of 2025-03-01):
1. Simplified user API:
 * User no longer need to define a global CUILayout object. 
   User just creates a dynamic JULayout object by calling EnableJULayout(some_hdlg) 
   and its lifetime is managed with some_hdlg automatically.
 * User no longer need to supply callback function for WM_SIZE and WM_GETMINMAXINFO.
 * For AnchorControl(), user no longer need to pass a fRedraw parameter, bcz
   fRedraw=TRUE is solely used on groupbox(due to its hollow trait). JULayout internally
   consider groupbox fRedraw=TRUE.
2. New feature: AnchorControl() support any [anchor-proportion, offset] pairing value.
3. New feature: Add JULayout::PropSheetProc() to support dialog-box inside property-sheet.
4. New feature: User can assign dialogbox's minsize and maxsize; 0 means initial size.

[Usage] To use this lib, pick one and only one of your xxx.cpp(CHIMPL.cpp for example), 
write at its start:
	
	#define JULAYOUT_IMPL
	#include "JULayout2.h"

// In WM_INITDIALOG:
	
	JULayout *jul = JULayout::EnableJULayout(hdlg);

	jul->AnchorControl(0,0, 100,100, IDC_EDIT_WHOLE_AREA);
	jul->AnchorControl(...);

[Note] If a Uic stay in a groupbox, and the groupbox goes through JULayout, then,
the Uic must go through JULayout as well, even if the Uic keep stationary.
If not doing so for the Uic, the Uic will be erased when the groupbox is resizing.

******************************************************************************/

#include <assert.h>
#include <windows.h> // HWND, POINT, LRESULT etc

#ifdef JULAYOUT_IMPL
#define CxxWindowSubclass_IMPL
#endif
#include <mswin/CxxWindowSubclass.h> // needs comctl32.lib


#define JULAYOUT_MAX_CONTROLS 200 

class JULayout : public CxxWindowSubclass
{
public:
	JULayout();

	static JULayout* EnableJULayout(HWND hwndParent,
		int nMinWidth=0, int nMinHeight=0, int nMaxWidth=32000, int nMaxHeight=32000);
	// -- A new JULayout object is returned to caller, and the lifetime of this object
	// is managed automatically, i.e. the JULayout object is destroyed when the
	// window by HWND is destroyed by the system.
	// -- If nMinWidth=0, the min-width will be dlgbox's initial width. Same for other three.

	static JULayout* GetJULayout(HWND hwndParent);
   
	// Anco: Anchor coefficient, this value should be a percent value 0~100, 
	// 0 means left-most or top-most, 100 means right-most or bottom most.
	bool AnchorControl(int x1Anco, int y1Anco, int x2Anco, int y2Anco, int nCtrlID);
	
	bool AnchorControls(int x1Anco, int y1Anco, int x2Anco, int y2Anco, ...);
	// -- ... is a series of control-IDs, ending by a value of -1.

public:
	static bool PropSheetProc(HWND hwndPrsht, UINT uMsg, LPARAM lParam);
	// -- In order to make PropertySheet() dialog resizable, user(you) should call into
	// JULayout::PropSheetProc() in PropertySheet()'s PFNPROPSHEETCALLBACK, like this:
	//
	// 	static int CALLBACK YourPrshtProc(HWND hwndPrsht, UINT uMsg, LPARAM lParam)
	// 	{
	// 		bool succ = JULayout::PropSheetProc(hwndPrsht, uMsg, lParam);
	//		if(!succ) ... do some logging ...;
	//
	//      ... other hook actions you routinely would do ...
	//
	// 		return 0;
	// 	}
	// 
	// 	INT_PTR YourPreparePropertysheet(...)
	// 	{
	// 		PROPSHEETHEADER psh = {sizeof(PROPSHEETHEADER)};
	// 
	// 		psh.dwFlags     = ... | PSH_USECALLBACK;
	// 		psh.hInstance   = ...;
	// 		psh.hwndParent  = hWnd;
	// 	...
	// 		psh.pfnCallback = YourPrshtProc;
	// 
	// 		return PropertySheet(&psh);
	// 	}
	// 
	// Yes, you relay the three params from Windows to JULayout::PropSheetProc(), 
	// and this PropSheetProc() prepares everything to make it work. Just that simple.
	//
	// Return false on fail, probably due to system running out of resource.

private: // was public, now they are private
	bool Initialize(HWND hwndParent, 
		int nMinWidth=0, int nMinHeight=0, int nMaxWidth=32000, int nMaxHeight=32000);
	// -- If nMinWidth=0, the min-width will be dlgbox's initial width. Same for other three.

	bool AdjustControls(int cx, int cy);
	void HandleMinMax(PMINMAXINFO pMinMax) 
	{ 
		pMinMax->ptMinTrackSize = m_ptMinParentDims; 
		pMinMax->ptMaxTrackSize = m_ptMaxParentDims;
	}

private:
	static bool in_PropSheetPrepare(HWND hwndPrsht);

private:
	struct Ancofs_st {
		int Anco; // Anchor coefficient, this value should be a percent value 0~100, 
		int Offset; // pixel offset added to Anco-baseline
	};

	struct CtrlInfo_st {
		int         m_nID; 
		Ancofs_st pt1x, pt1y; // pt1 means the north-west corner of the control
		Ancofs_st pt2x, pt2y; // pt2 means the south-east corner of the control
		RECT rectnow; // current position cache, debugging purpose
	}; 

//	bool PatchWndProc();

	static void PixelFromAnchorPoint(int cxParent, int cyParent, int xAnco, int yAnco, PPOINT ppt)
	{
		ppt->x = cxParent*xAnco/100;
		ppt->y = cyParent*yAnco/100;
	}

private:
//	static LRESULT CALLBACK JulWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	
	virtual LRESULT WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	static LRESULT CALLBACK PrshtWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	static bool IsGroupBox(HWND hwnd);

private:    
	CtrlInfo_st m_CtrlInfo[JULAYOUT_MAX_CONTROLS]; // Max controls allowed in a dialog template
	int     m_nNumControls;
	HWND    m_hwndParent;
	POINT   m_ptMinParentDims;
	POINT   m_ptMaxParentDims;

	static const TCHAR s_sigSubclass[];
}; 


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


#ifdef JULAYOUT_IMPL

#include <windows.h>
#include <windowsx.h>
#include <tchar.h>
#include <assert.h>


#ifndef JULAYOUT_DEBUG
#include <CHIMPL_vaDBG_hide.h>
#endif


const TCHAR JULayout::s_sigSubclass[] = _T("sig_JULayout");

JULayout::JULayout()
{
	ZeroMemory(m_CtrlInfo, sizeof(m_CtrlInfo));
	m_nNumControls = 0;
	m_hwndParent = NULL;
	m_ptMinParentDims.x = m_ptMinParentDims.y = 0;
	m_ptMaxParentDims.x = m_ptMaxParentDims.y = 0;
}


bool JULayout::Initialize(HWND hwndParent, 
	int nMinWidth, int nMinHeight, int nMaxWidth, int nMaxHeight) 
{
	// User should call this from within WM_INITDIALOG.

	if(!IsWindow(hwndParent))
		return false;

	m_hwndParent = hwndParent;
	m_nNumControls = 0;

	RECT rc = {};
	GetWindowRect(m_hwndParent, &rc);

	if(nMinWidth == 0) 
		m_ptMinParentDims.x = rc.right  - rc.left; 
	else
		m_ptMinParentDims.x = nMinWidth;

	if(nMinHeight == 0)
		m_ptMinParentDims.y = rc.bottom - rc.top;
	else
		m_ptMinParentDims.y = nMinHeight;

	if(nMaxWidth == 0)
		m_ptMaxParentDims.x = rc.right  - rc.left;
	else
		m_ptMaxParentDims.x = nMaxWidth;

	if(nMaxHeight == 0)
		m_ptMaxParentDims.y = rc.bottom - rc.top;
	else
		m_ptMaxParentDims.y = nMaxHeight;
	
	return true;
}

JULayout* JULayout::GetJULayout(HWND hwndParent)
{
	CxxWindowSubclass::ReCode_et serr;
	JULayout *jul = CxxWindowSubclass::FetchCxxobjFromHwnd<JULayout>(
		hwndParent, s_sigSubclass, FALSE, &serr);

	return jul;
}

JULayout* JULayout::EnableJULayout(HWND hwndParent,
	int nMinWidth, int nMinHeight, int nMaxWidth, int nMaxHeight)
{
	CxxWindowSubclass::ReCode_et serr;
	JULayout *jul = CxxWindowSubclass::FetchCxxobjFromHwnd<JULayout>(
		hwndParent, s_sigSubclass, TRUE, &serr);
	
	bool succ = jul->Initialize(hwndParent, nMinWidth, nMinHeight, nMaxWidth, nMaxHeight);
	if(!succ)
	{
		jul->DetachHwnd(true);
		return NULL;
	}

	return jul;
}

LRESULT JULayout::WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	// User overrides this WndProc() to hook into hwnd's message processing.

	if (uMsg == WM_SIZE)
	{
		AdjustControls(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
	}
	else if (uMsg == WM_GETMINMAXINFO)
	{
		MINMAXINFO *pMinMaxInfo = (MINMAXINFO*)lParam;
		HandleMinMax(pMinMaxInfo);
	}

	return DefSubclassProc(hwnd, uMsg, wParam, lParam);
}


class JULPrsht : public CxxWindowSubclass
{
public:
	JULPrsht()
	{
		xdiff = ydiff = 0;
	}

	virtual LRESULT WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	static bool PropSheetPrepare(HWND hwndPrsht);

private:
	UINT xdiff, ydiff; // width,height diff of the Prsht and its containing page

	static UINT s_WM_JULPrsht_DO_INIT;

	static const TCHAR s_sigSubclass[];
};

UINT JULPrsht::s_WM_JULPrsht_DO_INIT = 0;

const TCHAR JULPrsht::s_sigSubclass[] = _T("sig_JULPrsht");

struct DLGTEMPLATEEX_msdn  {
	WORD dlgVer;
	WORD signature;
	DWORD helpID;
	DWORD exStyle;
	DWORD style;
	WORD cDlgItems;
	// remaining members omitted
};

// Duplicate defining SDK constant (PSCB_INITIALIZED and PSCB_PRECREATE) from prsht.h, 
// so that JULayout user don't have to include prsht.h .
#define PSCB_INITIALIZED_1  1
#define PSCB_PRECREATE_2    2

bool JULayout::PropSheetProc(HWND hwndPrsht, UINT uMsg, LPARAM lParam)
{
	if (uMsg==PSCB_PRECREATE_2) // =PSCB_PRECREATE
	{
		DLGTEMPLATE& dt = *(DLGTEMPLATE*)lParam;
		DLGTEMPLATEEX_msdn& dtex = *(DLGTEMPLATEEX_msdn*)lParam;

		auto& style = dtex.signature==0xFF ? dtex.style : dt.style;

		// Enable WS_OVERLAPPEDWINDOW(implies WS_THICKFRAME), so to make it resizable.
		// Note: We have to turn on WS_THIKFRAME *here*. If we do it when processing
		// g_WM_JULAYOUT_DO_INIT message, that WS_THICKFRAME bit is set, however, 
		// the Prsht frame is still not draggable. Can't explain why yet.
		//
		// WS_CLIPCHILDREN is to make the Tab-header less flickering.
		//
		style |= WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS ;

		return 0;
	}
	else if(uMsg==PSCB_INITIALIZED_1) // =PSCB_INITIALIZED
	{
		bool succ = JULPrsht::PropSheetPrepare(hwndPrsht);
		return succ;
	}
	
	return true;
}

bool JULPrsht::PropSheetPrepare(HWND hwndPrsht)
{
	//
	// Subclass hwndPrsht for WM_SIZE processing
	//

	CxxWindowSubclass::ReCode_et serr;
	JULPrsht *jul = CxxWindowSubclass::FetchCxxobjFromHwnd<JULPrsht>(
		hwndPrsht, JULPrsht::s_sigSubclass, TRUE, &serr);

	if(!jul)
		return false;

	// Next, we'll call JULayout::EnableJULayout(), but, we have to postpone it
	// with a PostMessage. If we call JULayout::EnableJULayout() here, there is
	// at least two problems:
	// 1. The stock buttons of [OK], [Cancel], [Apply] etc has not been placed at their
	//	  final position(=right-bottom corner), so JULayout would not place them correctly
	//	  either.
	// 2. The first *page* inside the Prsht has not been created(=cannot find it by 
	//	  FindWindowEx), so we lack some coord params for auto-layout.

	if(!s_WM_JULPrsht_DO_INIT)
	{
		s_WM_JULPrsht_DO_INIT = RegisterWindowMessage(JULPrsht::s_sigSubclass);
	}
	//
	::PostMessage(hwndPrsht, s_WM_JULPrsht_DO_INIT, 0, 0);

	return true;
}

LRESULT JULPrsht::WndProc(HWND hwndPrsht, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == s_WM_JULPrsht_DO_INIT)
	{
		JULayout *jul = JULayout::EnableJULayout(hwndPrsht);

		// Find child windows of the Prsht and anchor them to JULayout.

		HWND hwndPrevChild = NULL;
		for (; ;)
		{
			HWND hwndNowChild = FindWindowEx(hwndPrsht, hwndPrevChild, NULL, NULL);

			if (hwndNowChild == NULL)
				break;

			UINT id = GetWindowID(hwndNowChild);
			TCHAR classname[100] = {};
			GetClassName(hwndNowChild, classname, 100);
//			dbgprint("See id=0x08%X , class=%s", id, classname);

			if (_tcsicmp(classname, _T("button")) == 0)
			{
				// Meet the bottom-right buttons like OK, Cancel, Apply.
				jul->AnchorControl(100, 100, 100, 100, id);
			}
			else
			{
				// The SysTabControl32 and those #32770 substantial dlgbox.
				// Anchor all these to top-left and bottom-right(fill their container)

				jul->AnchorControl(0, 0, 100, 100, id);

				if (_tcsicmp(classname, _T("#32770")) == 0) // a prsht-page
				{
					RECT rcPrsht = {};
					RECT rcPage = {};
					GetClientRect(hwndPrsht, &rcPrsht);
					GetClientRect(hwndNowChild, &rcPage);

					this->xdiff = rcPrsht.right - rcPage.right;
					this->ydiff = rcPrsht.bottom - rcPage.bottom;
					assert(this->xdiff > 0); // e.g. 20 on Win7
					assert(this->ydiff > 0); // e.g. 69 on Win7
				}
			}

			hwndPrevChild = hwndNowChild;
		}
	}
	else if (uMsg == WM_SIZE)
	{
//		dbgprint("Prsht sizing: %d * %d", GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		RECT rcPrsht = {};
		GetClientRect(hwndPrsht, &rcPrsht); // we need only its width & height

		HWND hwndPrevChild = NULL;
		for (; ;)
		{
			HWND hwndNowChild = FindWindowEx(hwndPrsht, hwndPrevChild, NULL, NULL);
			if (hwndNowChild == NULL)
				break;

			TCHAR classname[100] = {};
			GetClassName(hwndNowChild, classname, 100);
//			dbgprint("See id=0x08%X , class=%s", id, classname);

			if (_tcsicmp(classname, _T("#32770")) == 0)
			{
				// Now we meet a substantial dlgbox(=page), and we move it
				// to fill all empty area of the Prsht(container).

				RECT rc = {};
				GetClientRect(hwndNowChild, &rc);
				MapWindowPoints(hwndNowChild, hwndPrsht, (POINT*)&rc, 2);

				// Now rc is relative to Prsht's client area.
				// rc.left & rc.top are always ok, but rc.right & rc.bottom need to adjust.

				MoveWindow(hwndNowChild, rc.left, rc.top,
					rcPrsht.right - this->xdiff,
					rcPrsht.bottom - this->ydiff,
					FALSE // FALSE: no need to force repaint
				);
			}

			hwndPrevChild = hwndNowChild;
		}
	}

	return DefSubclassProc(hwndPrsht, uMsg, wParam, lParam);
}


///////////////////////////////////////////////////////////////////////////////


bool JULayout::AnchorControl(int x1Anco, int y1Anco, int x2Anco, int y2Anco, int nCtrlID) 
{
	if(m_nNumControls>=JULAYOUT_MAX_CONTROLS)
		return false;

	HWND hwndControl = GetDlgItem(m_hwndParent, nCtrlID);
	if(hwndControl == NULL) 
		return false;
	
	CtrlInfo_st &cinfo = m_CtrlInfo[m_nNumControls];

	cinfo.m_nID = nCtrlID;

	cinfo.pt1x.Anco = x1Anco; cinfo.pt1y.Anco = y1Anco;
	cinfo.pt2x.Anco = x2Anco; cinfo.pt2y.Anco = y2Anco; 

	RECT &rcControl = cinfo.rectnow;
	GetWindowRect(hwndControl, &rcControl);  // Screen coords of control
	// Convert coords to parent-relative coordinates
	MapWindowPoints(HWND_DESKTOP, m_hwndParent, (PPOINT) &rcControl, 2);

	RECT rcParent; 
	GetClientRect(m_hwndParent, &rcParent);

	POINT pt; 
	PixelFromAnchorPoint(rcParent.right, rcParent.bottom, x1Anco, y1Anco, &pt);
	cinfo.pt1x.Offset = rcControl.left - pt.x; 
	cinfo.pt1y.Offset = rcControl.top - pt.y;

	PixelFromAnchorPoint(rcParent.right, rcParent.bottom, x2Anco, y2Anco, &pt);
	cinfo.pt2x.Offset = rcControl.right - pt.x;
	cinfo.pt2y.Offset = rcControl.bottom - pt.y;

	m_nNumControls++;

	return true;
}

bool JULayout::AnchorControls(int x1Anco, int y1Anco, int x2Anco, int y2Anco, ...) 
{
	bool AllOk = true;

	va_list arglist;
	va_start(arglist, y2Anco);
	int nID = va_arg(arglist, int);
	while (nID != -1)
	{
		AllOk = AnchorControl(x1Anco, y1Anco, x2Anco, y2Anco, nID) && AllOk;
		nID = va_arg(arglist, int);
	}           
	va_end(arglist);
	return(AllOk);
}

bool JULayout::AdjustControls(int cx, int cy) 
{
	//// Applying Jeffrey's original logic, that is great. ////

	// Create region consisting of all (old)areas occupied by controls.

	HRGN hrgnPaintBg = CreateRectRgn(0, 0, 0, 0); // Bg: background
	int i;
	for(i=0; i<m_nNumControls; i++)
	{
		HWND hwndUic = GetDlgItem(m_hwndParent, m_CtrlInfo[i].m_nID);
		RECT rcUic = {};
		GetWindowRect(hwndUic, &rcUic); // screen-coord of Uic

		// Convert screen-coord to parent-relative-coord (note: both input/output are client-area-coord)
		MapWindowPoints(HWND_DESKTOP, m_hwndParent, (POINT*)&rcUic, 2);

		HRGN hrgnUic = CreateRectRgnIndirect(&rcUic);
		CombineRgn(hrgnPaintBg, hrgnPaintBg, hrgnUic, RGN_OR);
		DeleteObject(hrgnUic);
	}

	// Iterate through each Uic's new position ...

	for(i=0; i<m_nNumControls; i++)
	{
		CtrlInfo_st &cinfo = m_CtrlInfo[i];

		// Get control's upper/left position w/respect to parent's width/height
		RECT rcUic = {};
		PixelFromAnchorPoint(cx, cy, cinfo.pt1x.Anco, cinfo.pt1y.Anco, (PPOINT)&rcUic);
		rcUic.left += cinfo.pt1x.Offset; 
		rcUic.top  += cinfo.pt1y.Offset; 

		// Get control's lower/right position w/respect to parent's width/height
		PixelFromAnchorPoint(cx, cy, cinfo.pt2x.Anco, cinfo.pt2y.Anco, (PPOINT)&rcUic.right);
		rcUic.right  += cinfo.pt2x.Offset;
		rcUic.bottom += cinfo.pt2y.Offset;

		// Position/size the control
		HWND hwndUic = GetDlgItem(m_hwndParent, cinfo.m_nID);

		MoveWindow(hwndUic, rcUic.left, rcUic.top, 
			rcUic.right - rcUic.left, rcUic.bottom - rcUic.top,
			FALSE // FALSE: Don't repaint immediately
			);

		if(IsGroupBox(hwndUic))
		{
			// Special processing for groupbox, bcz it is a *hollow* Uic.

			InvalidateRect(hwndUic, NULL, 
				FALSE // no need to erase bkgnd, bcz hrgnPaintBg will do it right now.
				);
			
			// Do not call UpdateWindow(hwndUic) here, so its WM_PAINT persists until 
			// after AdjustControls() calls FillRgn(hdc, hrgnPaintBg, hbrBg).
		}
		else
		{
			// Exclude Uic's new position from hrgnPaintBg
			HRGN hrgnUicNew = CreateRectRgnIndirect(&rcUic);
			CombineRgn(hrgnPaintBg, hrgnPaintBg, hrgnUicNew, RGN_DIFF);
			DeleteObject(hrgnUicNew);

			// Make the Uic repaint itself.
			InvalidateRect(hwndUic, NULL, 
				TRUE // this Uic should repaint bkgnd by himself
				); 
			SendMessage(hwndUic, WM_NCPAINT, 1, 0); // 1: entire window frame needs to be updated.
			UpdateWindow(hwndUic); // tell this Uic repaint immediately
		}
	}

	// Paint the newly exposed portion of the dialog box's client area
	HDC hdc = GetDC(m_hwndParent);
	HBRUSH hbrBg = CreateSolidBrush(GetSysColor(COLOR_3DFACE)); // =COLOR_BTNFACE
	FillRgn(hdc, hrgnPaintBg, hbrBg); 
	// -- Uic's new positions bypassed, so no flickering for most Uics.
	// The exception is groupbox, whose bkgnd is totally repainted so flickering exists.
	//
	// -- [2025-05-26] Shall we send m_hwndParent WM_ERASEBKGND in favor of wiping bkgnd directly?
	
	DeleteObject(hbrBg);
	ReleaseDC(m_hwndParent, hdc);
	DeleteObject(hrgnPaintBg);

	return true;
}


bool JULayout::IsGroupBox(HWND hwnd)
{
	TCHAR szWndclass[200] = {};
	GetClassName(hwnd, szWndclass, ARRAYSIZE(szWndclass));
	if(_tcscmp(szWndclass, _T("Button"))==0)
	{
		UINT ws = GetWindowStyle(hwnd);
		if(ws & BS_GROUPBOX)
			return true;
	}
	return false;
}


inline POINT GetOffset_from_child1_to_child2(HWND hwnd1, HWND hwnd2)
{
	// Was used by JULayout::GroupboxWndProc(), not used since 20250301.

	RECT rc1 = {}, rc2 = {};
	GetWindowRect(hwnd1, &rc1);
	GetWindowRect(hwnd2, &rc2);

	POINT ofs = {rc2.left-rc1.left, rc2.top-rc1.top};
	return ofs;
}


#ifndef JULAYOUT_DEBUG
#include <CHIMPL_vaDBG_show.h>
#endif

#endif   // JULAYOUT_IMPL

///////////////////////////////////////////////////////////////////////////////


#endif // __JULayout_h_
