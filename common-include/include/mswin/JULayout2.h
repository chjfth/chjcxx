#ifndef __JULayout2_h_20241207_
#define __JULayout2_h_20241207_

/******************************************************************************
Original: UILayout.h
Notices: Copyright (c) 2000 Jeffrey Richter
Purpose: This class manages child window positioning and sizing when a parent 
         window is resized.
         See Appendix B of his book PSSA2000.

Updates by Jimm Chen:
[2017-06-18] Now we can anchor a control with any proportion(pct 0~100).
[2022-11-07] Add JULayout::PropSheetProc() to support dialog-box inside property-sheet.
[2024-12-07] Cope with Groupbox background painting issue. Works OK now.
	Note: To have groupbox painted correctly, you must add groupbox to AnchorControl(),
	      even if the groupbox does not need to change size.

[Usage] To use this lib, pick one and only one of your xxx.cpp, write at its start:
	
	#define JULAYOUT_IMPL
	#include "JULayout2.h"

// In WM_INITDIALOG:
	
	JULayout *jul = JULayout::EnableJULayout(hdlg);

	jul->AnchorControl(0,0, 100,100, IDC_EDIT_WHOLE_AREA);
	jul->AnchorControl(...);

******************************************************************************/

#include <assert.h>

// #define JULAYOUT_DEBUG_INFO

#define JULAYOUT_MAX_CONTROLS 200 

class JULayout 
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
	//      ... other hook actions you apply ...
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

	bool PatchWndProc();

	static void PixelFromAnchorPoint(int cxParent, int cyParent, int xAnco, int yAnco, PPOINT ppt)
	{
		ppt->x = cxParent*xAnco/100;
		ppt->y = cyParent*yAnco/100;
	}

private:
	static LRESULT CALLBACK JulWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	static LRESULT CALLBACK PrshtWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	static bool IsGroupBox(HWND hwnd);
	static void SubClassTheGroupbox(HWND hwndGroupbox);
	static LRESULT CALLBACK GroupboxWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

private:    
	CtrlInfo_st m_CtrlInfo[JULAYOUT_MAX_CONTROLS]; // Max controls allowed in a dialog template
	int     m_nNumControls;
	HWND    m_hwndParent;
	POINT   m_ptMinParentDims;
	POINT   m_ptMaxParentDims;

	WNDPROC m_prevWndProc;
}; 


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


#ifdef JULAYOUT_IMPL

#include <windows.h>
#include <windowsx.h>
#include <tchar.h>
#include <assert.h>

//#include "dbgprint.h" // temp debug

#define JULAYOUT_STR          _T("JULayout")
#define JULAYOUT_PRSHT_STR    _T("JULayout.Prsht")
#define JULAYOUT_PRSHT2_STR   _T("JULayout.Prsht2")
#define JULAYOUT_GROUPBOX_STR _T("JULayout.GroupBox")
// -- Will use these strings to call SetProp()/GetProp(),
//    to associate JULayout object with an HWND.

#define ADD_PREV_WINPROC_SUFFIX(stem) stem _T(".PrevWndProc")

UINT g_WM_JULAYOUT_DO_INIT = 0;

JULayout::JULayout()
{
	ZeroMemory(m_CtrlInfo, sizeof(m_CtrlInfo));
	m_nNumControls = 0;
	m_hwndParent = NULL;
	m_ptMinParentDims.x = m_ptMinParentDims.y = 0;
	m_ptMaxParentDims.x = m_ptMaxParentDims.y = 0;
	m_prevWndProc = NULL;
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
	
	// Force WS_CLIPCHILDEN on hwndParent(the dlgbox) to reduce repaint flickering.
	UINT ostyle = GetWindowStyle(hwndParent);
	SetWindowLong(hwndParent, GWL_STYLE, ostyle | WS_CLIPCHILDREN);

	return true;
}

JULayout* JULayout::GetJULayout(HWND hwndParent)
{
	if(!IsWindow(hwndParent))
		return NULL;

	JULayout *jul = (JULayout*)GetProp(hwndParent, JULAYOUT_STR);
	return jul;
}

JULayout* JULayout::EnableJULayout(HWND hwndParent,
	int nMinWidth, int nMinHeight, int nMaxWidth, int nMaxHeight)
{
	if(!IsWindow(hwndParent))
		return NULL;

	// First check whether a JULayout object has been associated with hwndParent.
	// If so, just return that associated object.
	JULayout *jul = GetJULayout(hwndParent);
	if(jul)
		return jul;

	jul = new JULayout();
	if(!jul)
		return NULL;
	
	bool succ = jul->Initialize(hwndParent, nMinWidth, nMinHeight, nMaxWidth, nMaxHeight);
	if(!succ)
	{
		delete jul;
		return NULL;
	}

	SetProp(hwndParent, JULAYOUT_STR, (HANDLE)jul);

	jul->PatchWndProc();

	return jul;
}

bool JULayout::PatchWndProc()
{
	// Patch WndProc so that we can handle WM_SIZE and WM_GETMINMAX automatically.

	if(!IsWindow(m_hwndParent))
		return false;

	m_prevWndProc = SubclassWindow(m_hwndParent, JulWndProc);

	return true;
}


LRESULT CALLBACK 
JULayout::JulWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	JULayout *jul = (JULayout*)GetProp(hwnd, JULAYOUT_STR);
	if(!jul)
	{
		// This implies we had got WM_NCDESTROY sometime ago.
		// We have no "user data" now, so just fetch and call orig-WndProc.
		//
		// memo: I can see msg==WM_NOTIFY, wParam==1 multiple times here.

		WNDPROC orig = (WNDPROC)GetProp(hwnd, ADD_PREV_WINPROC_SUFFIX(JULAYOUT_STR));
		assert(orig);
		return orig(hwnd, msg, wParam, lParam);
	}

	if(msg==WM_SIZE)
	{
		jul->AdjustControls(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
	}
	else if(msg==WM_GETMINMAXINFO)
	{
		MINMAXINFO *pMinMaxInfo = (MINMAXINFO*)lParam;
		jul->HandleMinMax(pMinMaxInfo);
	}

	LRESULT ret = CallWindowProc(jul->m_prevWndProc, hwnd, msg, wParam, lParam);

	if(msg==WM_NCDESTROY)
	{
		// Delete our "user data" but preserve orig-WndProc at a specific place.

		RemoveProp(hwnd, JULAYOUT_STR);

		SetProp(hwnd, ADD_PREV_WINPROC_SUFFIX(JULAYOUT_STR), jul->m_prevWndProc);

		delete jul;
	}

	return ret;
}

struct JULPrsht_st
{
	WNDPROC prev_winproc;
	UINT xdiff, ydiff; // width,height diff of the Prsht and its containing page

	JULPrsht_st()
	{
		prev_winproc = NULL;
		xdiff = ydiff = 0;
	}
};

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
		bool succ = JULayout::in_PropSheetPrepare(hwndPrsht);
		return succ;
	}
	
	return true;
}

bool JULayout::in_PropSheetPrepare(HWND hwndPrsht)
{
	//
	// Subclass hwndPrsht for WM_SIZE processing
	//

	JULPrsht_st *jprsht = (JULPrsht_st*)GetProp(hwndPrsht, JULAYOUT_PRSHT_STR);
	if(jprsht)
	{
		// EnableForPrsht already called.
		return false; 
	}

	jprsht = new JULPrsht_st;
	jprsht->prev_winproc = SubclassWindow(hwndPrsht, PrshtWndProc);

	SetProp(hwndPrsht, JULAYOUT_PRSHT_STR, jprsht);

	// Next, we'll call JULayout::EnableJULayout(), but, we have to postpone it
	// with a PostMessage. If we call JULayout::EnableJULayout() here, there is
	// at least two problems:
	// 1. The stock buttons of [OK], [Cancel], [Apply] etc has not been placed at their
	//	  final position(=right-bottom corner), so JULayout would not place them correctly
	//	  either.
	// 2. The first *page* inside the Prsht has not been created(=cannot find it by 
	//	  FindWindowEx), so we lack some coord params for auto-layout.

	if(!g_WM_JULAYOUT_DO_INIT)
	{
		g_WM_JULAYOUT_DO_INIT = RegisterWindowMessage(JULAYOUT_PRSHT_STR);
	}
	//
	::PostMessage(hwndPrsht, g_WM_JULAYOUT_DO_INIT, 0, 0);

	return true;
}

LRESULT CALLBACK 
JULayout::PrshtWndProc(HWND hwndPrsht, UINT msg, WPARAM wParam, LPARAM lParam)
{
	JULPrsht_st *jprsht = (JULPrsht_st*)GetProp(hwndPrsht, JULAYOUT_PRSHT_STR);
	if(!jprsht)
	{
		// This implies we had got WM_NCDESTROY sometime ago.
		// We have no "user data" now, so just fetch and call orig-WndProc.

		WNDPROC orig = (WNDPROC)GetProp(hwndPrsht, ADD_PREV_WINPROC_SUFFIX(JULAYOUT_PRSHT_STR));
		assert(orig);
		return orig(hwndPrsht, msg, wParam, lParam);
	}

	if(msg==g_WM_JULAYOUT_DO_INIT)
	{
		JULayout *jul = JULayout::EnableJULayout(hwndPrsht);

		// Find child windows of the Prsht and anchor them to JULayout.

		HWND hwndPrevChild = NULL;
		for(; ;)
		{
			HWND hwndNowChild = FindWindowEx(hwndPrsht, hwndPrevChild, NULL, NULL);

			if(hwndNowChild==NULL)
				break;

			UINT id = GetWindowID(hwndNowChild);
			TCHAR classname[100] = {};
			GetClassName(hwndNowChild, classname, 100);
//			dbgprint("See id=0x08%X , class=%s", id, classname);

			if(_tcsicmp(classname, _T("button"))==0)
			{
				// Meet the bottom-right buttons like OK, Cancel, Apply.
				jul->AnchorControl(100,100, 100,100, id);
			}
			else
			{
				// The SysTabControl32 and those #32770 substantial dlgbox.
				// Anchor all these to top-left and bottom-right(fill their container)

				jul->AnchorControl(0,0, 100,100, id);

				if(_tcsicmp(classname, _T("#32770"))==0) // a page
				{
					RECT rcPrsht = {};
					RECT rcPage = {}; 
					GetClientRect(hwndPrsht, &rcPrsht);
					GetClientRect(hwndNowChild, &rcPage);

					jprsht->xdiff = rcPrsht.right - rcPage.right;
					jprsht->ydiff = rcPrsht.bottom - rcPage.bottom;
					assert(jprsht->xdiff>0); // e.g. 20 on Win7
					assert(jprsht->ydiff>0); // e.g. 69 on Win7
				}
			}

			hwndPrevChild = hwndNowChild;
		}
	}
	else if(msg==WM_SIZE)
	{
//		dbgprint("Prsht sizing: %d * %d", GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		RECT rcPrsht = {};
		GetClientRect(hwndPrsht, &rcPrsht); // we need only its width & height
		
		HWND hwndPrevChild = NULL;
		for(; ;)
		{
			HWND hwndNowChild = FindWindowEx(hwndPrsht, hwndPrevChild, NULL, NULL);
			if(hwndNowChild==NULL)
				break;
			
			TCHAR classname[100] = {};
			GetClassName(hwndNowChild, classname, 100);
//			dbgprint("See id=0x08%X , class=%s", id, classname);

			if(_tcsicmp(classname, _T("#32770"))==0) 
			{
				// Now we meet a substantial dlgbox(=page), and we move it
				// to fill all empty area of the Prsht(container).

				RECT rc = {};
				GetClientRect(hwndNowChild, &rc);
				MapWindowPoints(hwndNowChild, hwndPrsht, (POINT*)&rc, 2);

				// Now rc is relative to Prsht's client area.
				// rc.left & rc.top are always ok, but rc.right & rc.bottom need to adjust.

				MoveWindow(hwndNowChild, rc.left, rc.top, 
					rcPrsht.right - jprsht->xdiff,
					rcPrsht.bottom - jprsht->ydiff,
					FALSE // FALSE: no need to force repaint
					);
			}
			
			hwndPrevChild = hwndNowChild;
		}
	}

	LRESULT ret = CallWindowProc(jprsht->prev_winproc, hwndPrsht, msg, wParam, lParam);

	if(msg==WM_NCDESTROY)
	{
		RemoveProp(hwndPrsht, JULAYOUT_PRSHT_STR);

		SetProp(hwndPrsht, ADD_PREV_WINPROC_SUFFIX(JULAYOUT_PRSHT_STR), jprsht->prev_winproc);
		
		delete jprsht;
	}

	return ret;
}

///////////////////////////////////////////////////////////////////////////////


bool JULayout::AnchorControl(int x1Anco, int y1Anco, int x2Anco, int y2Anco, int nID) 
{
	if(m_nNumControls>=JULAYOUT_MAX_CONTROLS)
		return false;

	HWND hwndControl = GetDlgItem(m_hwndParent, nID);
	if(hwndControl == NULL) 
		return false;
	
	CtrlInfo_st &cinfo = m_CtrlInfo[m_nNumControls];

	cinfo.m_nID = nID;

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

	// Need special processing for GroupBox

	bool isGroupbox = IsGroupBox(hwndControl);
	if(isGroupbox)
	{
		SubClassTheGroupbox(hwndControl);
	}

	return true;
}

bool JULayout::AnchorControls(int x1Anco, int y1Anco, int x2Anco, int y2Anco, ...) 
{
	bool fOk = true;

	va_list arglist;
	va_start(arglist, y2Anco);
	int nID = va_arg(arglist, int);
	while (fOk && (nID != -1)) 
	{
		fOk = fOk && AnchorControl(x1Anco, y1Anco, x2Anco, y2Anco, nID);
		nID = va_arg(arglist, int);
	}           
	va_end(arglist);
	return(fOk);
}

bool JULayout::AdjustControls(int cx, int cy) 
{
	int i;
	HDWP hdwp = ::BeginDeferWindowPos(m_nNumControls);

	for(i=0; i<m_nNumControls; i++) 
	{
		CtrlInfo_st &cinfo = m_CtrlInfo[i];

		// Get control's upper/left position w/respect to parent's width/height
		RECT rcControl = {};
		PixelFromAnchorPoint(cx, cy, cinfo.pt1x.Anco, cinfo.pt1y.Anco, (PPOINT)&rcControl);
		rcControl.left += cinfo.pt1x.Offset; 
		rcControl.top  += cinfo.pt1y.Offset; 

		// Get control's lower/right position w/respect to parent's width/height
		PixelFromAnchorPoint(cx, cy, cinfo.pt2x.Anco, cinfo.pt2y.Anco, (PPOINT)&rcControl.right);
		rcControl.right  += cinfo.pt2x.Offset;
		rcControl.bottom += cinfo.pt2y.Offset;

		// Position/size the control
		HWND hwndControl = GetDlgItem(m_hwndParent, cinfo.m_nID);

		DeferWindowPos(hdwp, hwndControl,
			NULL, // no use bcz of SWP_NOZORDER
			rcControl.left, 
			rcControl.top, 
			rcControl.right - rcControl.left, 
			rcControl.bottom - rcControl.top, 
			SWP_NOZORDER);		

#ifdef JULAYOUT_DEBUG_INFO
		vaDbgTs(_T("JUL: HWND 0x%08X LT[%d,%d],RB[%d,%d] => LT[%d,%d],RB[%d,%d]"),
			hwndControl,
			cinfo.rectnow.left, cinfo.rectnow.top, cinfo.rectnow.right, cinfo.rectnow.bottom,
			rcControl.left, rcControl.top, rcControl.right, rcControl.bottom
			);
#endif
		cinfo.rectnow = rcControl;
	}
	
	::EndDeferWindowPos(hdwp);

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

void JULayout::SubClassTheGroupbox(HWND hwndGroupbox)
{
	WNDPROC prevWndproc = SubclassWindow(hwndGroupbox, GroupboxWndProc);
	assert(prevWndproc);

	if(prevWndproc==GroupboxWndProc)
	{
		// BAD: should not anchor twice on the same Uic
		assert(prevWndproc != GroupboxWndProc);
		return;
	}

	BOOL succ = SetProp(hwndGroupbox, JULAYOUT_GROUPBOX_STR, (HANDLE)prevWndproc);
	assert(succ);
}

static POINT GetOffset_from_child1_to_child2(HWND hwnd1, HWND hwnd2)
{
	RECT rc1 = {}, rc2 = {};
	GetWindowRect(hwnd1, &rc1);
	GetWindowRect(hwnd2, &rc2);

	POINT ofs = {rc2.left-rc1.left, rc2.top-rc1.top};
	return ofs;
}

LRESULT CALLBACK 
JULayout::GroupboxWndProc(HWND hwndGroupbox, UINT msg, WPARAM wParam, LPARAM lParam)
{
	WNDPROC prevWndproc = (WNDPROC)GetProp(hwndGroupbox, JULAYOUT_GROUPBOX_STR);

	if(msg!=WM_ERASEBKGND)
	{
		return CallWindowProc(prevWndproc, hwndGroupbox, msg, wParam, lParam);
	}

	// We only care for WM_ERASEBKGND. We paint the background for the groupbox.
	// To avoid/minimize flickering other Uic's face, we need to exclude
	// all sibling Uic from painting.

	HDC hdc = (HDC)wParam;

	RECT rccligb = {};
	GetClientRect(hwndGroupbox, &rccligb);

	int excludes = 0;
	HRGN hrgnClip = CreateRectRgn(0, 0, rccligb.right, rccligb.bottom) ; // init the whole groupbox

	HWND hwndSib = GetFirstSibling(hwndGroupbox);
	for(;;)
	{
		if(!hwndSib)
			break;
		
		if(hwndSib!=hwndGroupbox) // only if not groupbox itself
		{
			RECT rcScrn = {};
			GetWindowRect(hwndSib, &rcScrn); // we need its width & height

			POINT ofs = GetOffset_from_child1_to_child2(hwndGroupbox, hwndSib);
			RECT rcSib = {}; // will be relative to hwndGroupbox
			rcSib.left = ofs.x;
			rcSib.top = ofs.y;
			rcSib.right = rcSib.left + (rcScrn.right-rcScrn.left);
			rcSib.bottom = rcSib.top + (rcScrn.bottom-rcScrn.top);

			HRGN hrgnSib = CreateRectRgn(rcSib.left, rcSib.top, rcSib.right, rcSib.bottom);
		
			int rgntype = CombineRgn(hrgnClip, hrgnClip, hrgnSib, RGN_DIFF);
			(void)rgntype;

			DeleteObject(hrgnSib);

			excludes++;

#ifdef JULAYOUT_DEBUG_INFO
			vaDbgS(_T("JUL: Exclude sibling from groupbox bg-painting #%d, hwnd=0x%08X, LT[%d,%d],RB[%d,%d]."),
				excludes, hwndSib, rcSib.left, rcSib.top, rcSib.right, rcSib.bottom);
#endif
		}

		hwndSib = GetNextSibling(hwndSib);
	}

	SelectClipRgn(hdc, hrgnClip); // set new clip region

	FillRect(hdc, &rccligb, GetSysColorBrush(COLOR_BTNFACE));

	SelectClipRgn(hdc, NULL);     // restore orig

	DeleteObject(hrgnClip);

	return TRUE;
}


#endif   // JULAYOUT_IMPL

///////////////////////////////////////////////////////////////////////////////


#endif // __JULayout_h_
