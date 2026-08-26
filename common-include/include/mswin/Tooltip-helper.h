#ifndef __CHHI__TooltipHelper_h_
#define __CHHI__TooltipHelper_h_
#define __CHHI__TooltipHelper_h_created_ 20250612
#define __CHHI__TooltipHelper_h_updated_ 20260812


#include <windows.h>
#include <CommCtrl.h>
#include <commdefs.h>

struct HottoolId_st
{
	// A hottool is identified by TOOLINFO.hwnd and TOOLINFO.uId .

	HWND hwndo; // the tooltip's owner

	union
	{
		HWND hUic;
		void *uId;
	};
};

inline BOOL do_TTM_ADDTOOL(HWND htt, const TOOLINFO &ti)
{
	/*
	TOOLINFO ti = {sizeof(TOOLINFO)};
	ti.hwnd = hottool.hwndo;
	ti.uId = (UINT_PTR)hottool.uId;
	*/
	LRESULT lre = SendMessage(htt, TTM_ADDTOOL, 0, (LPARAM)&ti);
	return (BOOL)lre;
}

inline BOOL do_TTM_ADDTOOL_nodup(HWND htt, const TOOLINFO &ti, bool *pExisted = nullptr)
{
	// TTM_ADDTOOL no-duplicate .
	// If the hottool has existed, we just call TTM_SETTOOLINFO.

	SETTLE_OUTPUT_PTR(bool, pExisted, false);

	TOOLINFO tiOld = { sizeof(TOOLINFO) };
	tiOld.hwnd = ti.hwnd;
	tiOld.uId = ti.uId;

	LRESULT lsucc = SendMessage(htt, TTM_GETTOOLINFO, 0, (LPARAM)&tiOld);
	if (lsucc)
	{
		// SendMessage(htt, TTM_SETTOOLINFO, 0, (LPARAM)&ti);
		// -- [2025-06-12] Chj: Do NOT do above TTM_SETTOOLINFO, which is danger.
		// Tooltip syscode may have applied some critical .uFlags into ti.Old, and I'm not
		// sure how to safely merge ti into tiOld.
		// For example, Unicode EXE will exhibit TTF_UNICODE(0x40) in tiOld. If this flag
		// is lost, tooltip will not show up again.

		*pExisted = TRUE;
		return TRUE;
	}
	else
	{
		LRESULT lre = SendMessage(htt, TTM_ADDTOOL, 0, (LPARAM)&ti);
		return (BOOL)lre;
	}
}

inline BOOL do_TTM_SETTITLE_ClearTitle(HWND htt)
{
	LRESULT lsucc = SendMessage(htt, TTM_SETTITLE, TTI_NONE, (LPARAM)_T(""));
	assert(lsucc);

	return lsucc ? TRUE : FALSE;
}


#include <mswin/CHwndTimer.h>

class CTooltipSimple
{
	// Code derived from year 2017:
	// D:\gitw\bookcode-mswin\__chjcxx\gadgetlib\libsrc\mswin\wintooltip.cpp
public:
	CTooltipSimple();
	~CTooltipSimple();

	enum ShowWhere_et 
	{
		Show_Hide = 0,      // hide the tooltip
		Show_ScreenPos = 1, // pt is screen position
		Show_WindowPos = 2, // pt is relative to hwndOwner's window
		Show_ClientPos = 3, // pt is relative to hwndOwner's client area
		Show_AtMouse   = 4, // pt relative to current mouse cursor
		Show_BelowMouse= 5, // similar to AtMouse, but Y+SM_CYCURSOR.
	};

	bool Create(HWND hwndOwner, bool isBalloon=false);

	bool vlShow(ShowWhere_et where, const POINT *pt, const TCHAR *szfmt, va_list args);
	// -- `pt` coord according to `where`.
	//    For ScreenPos/WindowPos/ClientPos, pt is a PoN value(negative means from right/bottom border). 
	//    If `pt` is nullptr, same as pt={0,0}.
	//    If szfmt==nullptr, previous text is preserved, just show the tooltip.

	bool Show(ShowWhere_et where, const POINT *pt=nullptr, const TCHAR *szfmt=nullptr, ...)
	{
		va_list args;
		va_start(args, szfmt);
		bool ret = vlShow(where, pt, szfmt, args);
		va_end(args);
		return ret;
	}

	bool ShowBelowMouse(const TCHAR *szfmt=nullptr, ...)
	{
		va_list args;
		va_start(args, szfmt);
		bool ret = vlShow(Show_BelowMouse, NULL, szfmt, args);
		va_end(args);
		return ret;
	}

	bool Hide(int delay_millisec=0)
	{
		if(delay_millisec<=0)
			return Show(Show_Hide);

		m_timer.m_ptt = this;
		m_timer.StartDelayedWork(m_htt, delay_millisec);
		return true;
	}

protected:

	class CTimerHideTT : public CHwndTimer
	{
	public:
		CTooltipSimple *m_ptt;

		virtual void TimerCallback() cxx11_override
		{
			assert(m_ptt);
			m_ptt->Hide();
		}
	};


private:
	HWND m_htt; // tooltip handle
	HWND m_hwndOwner;
	Sdring m_text;
	CTimerHideTT m_timer;
};



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


#if defined(TooltipHelper_IMPL) || (defined CHHI_ALL_IMPL && !defined CHHI_ALL_IMPL_HIDE_TooltipHelper) // [IMPL]


// >>> Include headers required by this lib's implementation
#include <stdarg.h>
#include <commdefs.h> // for Uint, Uint64, enum bitwise-OR etc
//#include <snTprintf.h>
#include <StringHelper.h>
#include <utils_env.h>
#include <_MINMAX_.h>
// <<< Include headers required by this lib's implementation


#ifndef TooltipHelper_DEBUG
#include <CHHI_vaDBG_hide.h> // Suppress/invalidate vaDBG macros, from now on
#endif


CTooltipSimple::CTooltipSimple()
{
	m_htt = m_hwndOwner = NULL;
}

CTooltipSimple::~CTooltipSimple()
{
	if(m_htt)
		DestroyWindow(m_htt);
}

bool CTooltipSimple::Create(HWND hOwner, bool isBalloon)
{
	WinErr_t winerr = 0;

	if(!hOwner)
		hOwner = GetDesktopWindow();

	m_hwndOwner = hOwner;
	m_htt = CreateWindowEx(WS_EX_TOPMOST, TOOLTIPS_CLASS, NULL,
		TTS_NOPREFIX | TTS_ALWAYSTIP | (isBalloon ? TTS_BALLOON : 0), // implies WS_POPUP
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		hOwner, // set owner so that tooltip object gets destroyed along with owner's destruction
		NULL,
		NULL, // hInstance
		NULL);

	if (!m_htt)
		return false;

	// Set up the tool information. In this case, the "tool" is the entire parent window.

	TOOLINFO ti = { sizeof(TOOLINFO) };
	ti.uFlags = TTF_TRACK | TTF_ABSOLUTE;
	// -- TTF_TRACK: User can use TTM_TRACKPOSITION to set its position.
	// -- TTF_ABSOLUTE: Makes TTM_TRACKPOSITION(X,Y) genuine, no encircling effect.
	ti.hwnd = NULL;
	ti.uId = NULL; // We want "whole screen", so these two are NULL.

	LRESULT lret = SendMessage(m_htt, TTM_ADDTOOL, 0, (LPARAM)&ti);
	assert(lret == TRUE);
	if (lret != TRUE)
		return false; // probably due to commctl32 *v6* DLL is not loaded.

	lret = SendMessage(m_htt, TTM_SETMAXTIPWIDTH, 0, 
		util_SimpleSysDpiScale(1024) // do it simple
		);  // We need TTM_SETMAXTIPWIDTH to display multiline tooltip text.
	return true;
}

bool CTooltipSimple::vlShow(ShowWhere_et where, const POINT *pt, const TCHAR *szfmt, va_list args)
{
	if(!m_htt)
		return false;

	m_timer.StopTimer();

	const POINT pt_zero = {0, 0};
	if(!pt)
		pt = &pt_zero;

	WinErr_t winerr = 0;

	TOOLINFO ti = { sizeof(TOOLINFO) };
	LRESULT lret = SendMessage(m_htt, TTM_GETTOOLINFO, 0, (LPARAM)&ti);

	if (where==Show_Hide)
	{
		SendMessage(m_htt, TTM_TRACKACTIVATE, FALSE, (LPARAM)&ti);
		return true;
	}

	if(szfmt)
		vlSdringSet(m_text, szfmt, args);

	ti.lpszText = (LPTSTR)m_text.c_str();
	lret = SendMessage(m_htt, TTM_SETTOOLINFO, 0, (LPARAM)&ti);

	POINT ptscreen = {0, 0};

	if(where==Show_ScreenPos)
	{
		ptscreen = *pt;
	}
	else if(where==Show_WindowPos)
	{
		RECT rc = {};
		GetWindowRect(m_hwndOwner, &rc);
		ptscreen.x = RangeOffset_pon(rc.left, rc.right, pt->x);
		ptscreen.y = RangeOffset_pon(rc.top, rc.bottom, pt->y);
	}
	else if(where==Show_ClientPos)
	{
		RECT rc = {};
		GetClientRect_ScreenPos(m_hwndOwner, &rc);
		ptscreen.x = RangeOffset_pon(rc.left, rc.right, pt->x);
		ptscreen.y = RangeOffset_pon(rc.top, rc.bottom, pt->y);
	}
	else if(where==Show_AtMouse || where==Show_BelowMouse)
	{
		GetCursorPos(&ptscreen);
		int cursorHeight = GetSystemMetrics(SM_CYCURSOR);

		if(where==Show_BelowMouse)
			ptscreen.y += cursorHeight;

		ptscreen.x += pt->x;
		ptscreen.y += pt->y;
	}
	else
		assert(0);

	lret = SendMessage(m_htt, TTM_TRACKPOSITION, 0, MAKELONG(ptscreen.x, ptscreen.y));

	lret = SendMessage(m_htt, TTM_TRACKACTIVATE, TRUE, (LPARAM)&ti);

	return true;
}





#ifndef TooltipHelper_DEBUG
#include <CHHI_vaDBG_show.h> // Now restore vaDBG macros
#endif


#endif // [IMPL]


#endif // include once guard
