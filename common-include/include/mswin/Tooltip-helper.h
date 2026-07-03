#ifndef __CHHI__TooltipHelper_h_
#define __CHHI__TooltipHelper_h_
#define __CHHI__TooltipHelper_h_created_ 20250612
#define __CHHI__TooltipHelper_h_updated_ 20260703


#include <windows.h>
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


class CTooltipSimple
{
	// Code derived from year 2017:
	// D:\gitw\bookcode-mswin\__chjcxx\gadgetlib\libsrc\mswin\wintooltip.cpp
public:
	CTooltipSimple();
	~CTooltipSimple();

	bool Create(HWND hOwner, bool isBalloon=false);
	bool Show(bool isShow, const POINT *pt=nullptr, const TCHAR *szfmt=nullptr, ...);
	// -- `pt` is screen position. If nullptr, show tooltip beside mouse cursor.
	//    If szfmt==nullptr, previous text is preserved, just show the tooltip.

private:
	HWND m_htt; // tooltip handle
	HWND m_hwndOwner;
	Sdring m_text;
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

bool CTooltipSimple::Show(bool isShow, const POINT *pt_screen, const TCHAR *szfmt, ...)
{
	assert(m_htt);
	if(!m_htt)
		return false;

	WinErr_t winerr = 0;

	TOOLINFO ti = { sizeof(TOOLINFO) };
	LRESULT lret = SendMessage(m_htt, TTM_GETTOOLINFO, 0, (LPARAM)&ti);

	if (!isShow)
	{
		SendMessage(m_htt, TTM_TRACKACTIVATE, FALSE, (LPARAM)&ti);
		return ERROR_SUCCESS;
	}

	if(szfmt)
	{
		va_list args;
		va_start(args, szfmt);
		vlSdringSet(m_text, szfmt, args);
		va_end(args);
	}

	ti.lpszText = (LPTSTR)m_text.c_str();
	lret = SendMessage(m_htt, TTM_SETTOOLINFO, 0, (LPARAM)&ti);

	if(pt_screen)
	{
		lret = SendMessage(m_htt, TTM_TRACKPOSITION, 0,
			MAKELONG(pt_screen->x, pt_screen->y));
	}
	else
	{
		POINT pt = {};
		GetCursorPos(&pt);
		int cursorHeight = GetSystemMetrics(SM_CYCURSOR);
		pt.y += cursorHeight;
		lret = SendMessage(m_htt, TTM_TRACKPOSITION, 0, MAKELONG(pt.x, pt.y));
	}

	lret = SendMessage(m_htt, TTM_TRACKACTIVATE, TRUE, (LPARAM)&ti);

	return true;
}





#ifndef TooltipHelper_DEBUG
#include <CHHI_vaDBG_show.h> // Now restore vaDBG macros
#endif


#endif // [IMPL]


#endif // include once guard
