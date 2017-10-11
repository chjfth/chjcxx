#include "stdafx.h"

#include <_MINMAX_.h>
#include <gadgetlib/wintooltip.h>

class CTooltipHandle
{
public:
	CTooltipHandle();
	~CTooltipHandle();

	WinErr_gt CreateManualTip(HWND hOwner, bool isBalloon);
	WinErr_gt TooltipShow(bool isShow, POINT *pt=NULL, const TCHAR *szfmt=NULL, ...);

private:
	HWND m_htt; // tooltip handle
	HWND m_hwndOwner;

	TOOLINFO m_toolinfo;

	TCHAR *m_textbuf; // malloc/realloc
	int m_buflen_;
};

CTooltipHandle::CTooltipHandle()
{
	memset(this, 0, sizeof(CTooltipHandle));
}

CTooltipHandle::~CTooltipHandle()
{
	if(m_textbuf)
		free(m_textbuf);
}

WinErr_gt 
CTooltipHandle::CreateManualTip(HWND hOwner, bool isBalloon)
{
	WinErr_gt winerr = 0;

	m_hwndOwner = hOwner;
	m_htt = CreateWindowEx(WS_EX_TOPMOST, TOOLTIPS_CLASS , NULL, 
		TTS_NOPREFIX | TTS_ALWAYSTIP | (isBalloon?TTS_BALLOON:0) , // implies WS_POPUP
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 
		hOwner, // set owner so that tooltip object gets destroyed along with owner's destruction
		NULL, 
		NULL, // hInstance
		NULL);

	if (!m_htt)
	{
		winerr = GetLastError();
		if(!winerr)
			winerr = ERROR_NOT_ENOUGH_MEMORY;
		return NULL;
	}

	// Set up the tool information. In this case, the "tool" is the entire parent window.

	m_toolinfo.cbSize   = sizeof(TOOLINFO);
	m_toolinfo.uFlags   = TTF_TRACK | TTF_ABSOLUTE;
	m_toolinfo.hwnd     = NULL; // no need to notify for a manual tooltip
	m_toolinfo.hinst    = NULL;
	m_toolinfo.lpszText = NULL; // ok
	m_toolinfo.uId      = NULL; // no need to notify for a manual tooltip

//	GetClientRect (hOwner, &m_toolinfo.rect);
	LRESULT lret = SendMessage(m_htt, TTM_ADDTOOL, 0, (LPARAM)&m_toolinfo); 
	assert(lret==TRUE);
	if(lret!=TRUE)
		return ERROR_MOD_NOT_FOUND; // probably due to commctl32 *v6* DLL is not loaded.

//	lret = SendMessage(hwndTT, TTM_SETMAXTIPWIDTH, 0, 80);
	return ERROR_SUCCESS;
}

WinErr_gt 
CTooltipHandle::TooltipShow(bool isShow, POINT *pt, const TCHAR *fmt, ...)
{
	WinErr_gt winerr = 0;
	bool malloc_error = false;

	if(!isShow)
	{
		SendMessage(m_htt, TTM_TRACKACTIVATE, FALSE, (LPARAM)&m_toolinfo); 
		return ERROR_SUCCESS;
	}

	va_list args;
	va_start(args, fmt);

	POINT pt0 = {0,0};
	if(!pt)
		pt = &pt0;

	RECT rectOwner;
	BOOL Succ = GetClientRect(m_hwndOwner, &rectOwner);

	if(pt->x<0)
		pt->x = rectOwner.right - pt->x;
	if(pt->y<0)
		pt->y = rectOwner.bottom - pt->y;

	pt->x = _MID_(0, pt->x, rectOwner.right);
	pt->y = _MID_(0, pt->y, rectOwner.bottom);

	// Set tooltip display position
	ClientToScreen(m_hwndOwner, pt);
	SendMessage(m_htt, TTM_TRACKPOSITION, 0, MAKELONG(pt->x, pt->y));

	// Set tooltip text
	int textlen = mm_vsnprintf(m_textbuf, m_buflen_, fmt, args);
	if(textlen>=m_buflen_)
	{
		TCHAR *newbuf = (TCHAR*)realloc(m_textbuf, (textlen+1)*sizeof(TCHAR));
		if(newbuf)
		{
			m_textbuf = newbuf;
			m_buflen_ =  textlen+1;
			mm_vsnprintf(m_textbuf, m_buflen_, fmt, args);
		}
		else
			malloc_error = true;
	}
	m_toolinfo.lpszText = m_textbuf;
	SendMessage(m_htt, TTM_SETTOOLINFO, 0, (LPARAM)&m_toolinfo);

	SendMessage(m_htt, TTM_TRACKACTIVATE, TRUE, (LPARAM)&m_toolinfo); 

	va_end(args);

	if(malloc_error)
		return ERROR_NOT_ENOUGH_MEMORY;
	else
		return ERROR_SUCCESS;
}

TooltipHandle_t 
ggt_CreateManualTooltip(HWND hOwner, bool isBalloon, WinErr_gt *pWinErr)
{
	WinErr_gt _winerr = 0;
	if(!pWinErr)
		pWinErr = &_winerr;
	*pWinErr = 0;

	CTooltipHandle *ptt = new CTooltipHandle;
	if(!ptt)
	{
		*pWinErr = ERROR_NOT_ENOUGH_MEMORY;
		return NULL;
	}

	*pWinErr = ptt->CreateManualTip(hOwner, isBalloon);
	if(*pWinErr)
	{
		delete ptt;
		return NULL;
	}

	return (TooltipHandle_t)ptt;
}

//////////////////////////////////////////////////////////////////////////

WinErr_gt 
ggt_TooltipShow(TooltipHandle_t htt, bool isShow, POINT *pt, const TCHAR *szfmt, ...)
{
	va_list args;
	va_start(args, szfmt);

	CTooltipHandle *ptt = (CTooltipHandle*)htt;
	return ptt->TooltipShow(isShow, pt, _T("%w"), MM_WPAIR_PARAM(szfmt, args));

	va_end(args);
}

bool 
ggt_TooltipDelete(TooltipHandle_t htt)
{
	if(!htt)
		return false;

	CTooltipHandle *ptt = (CTooltipHandle*)htt;
	delete ptt;

	return true;
}
