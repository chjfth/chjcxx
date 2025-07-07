#ifndef __DlgTooltipEasy_h_20250707_
#define __DlgTooltipEasy_h_20250707_

#include <tchar.h>
#include <windows.h>

enum Dlgtte_err
{
	Dlgtte_Succ = 0,
	Dlgtte_Fail = -1,

	Dlgtte_InvalidHwnd = -5,
	Dlgtte_InvalidHottool = -6,
};

enum Dlgtte_BitFlags_et
{
	Dlgtte_Flags0 = 0,

	Dlgtte_BalloonUp = 0,
	Dlgtte_BalloonDown = 1,

	Dlgtte_AutoContentTipOnFocus_no = 0,
	Dlgtte_AutoContentTipOnFocus = 2,
};

typedef const TCHAR* PROC_DlgtteGetText(HWND hwndCtl, void *userctx);
// -- When callback, hwndCtl tells which control(button, editbox etc) is requesting tooltip text.


Dlgtte_err Dlgtte_EnableTooltip(HWND hwndCtl, 
	PROC_DlgtteGetText *getUsageText, void *uctxUsage,
	PROC_DlgtteGetText *getContentText = nullptr, void *uctxContent = nullptr,
	Dlgtte_BitFlags_et flags = Dlgtte_Flags0);
// -- This is the core function.
// User assigns dynamic usage-text via callback [getUsageText, uctxUsage], and 
// dynamic content-text via callback [getContentText, uctxContent].
// At least one of getUsageText and getContentText must be non-NULL.

Dlgtte_err Dlgtte_RemoveTooltip(HWND hwndCtl);

void Dlgtte_GetTooltipHwnd(HWND hwndCtl, HWND *pUsageTip=NULL, HWND *pContentTip=NULL);
// -- Hint: In PROC_DlgtteGetText callback, user can fetch hwndContentTip via this function,
//    and do SendMessage(hwndContentTip, TTM_SETTITLE, ...) to customize its icon/title.

Dlgtte_err Dlgtte_GetFlags(HWND hwndCtl, Dlgtte_BitFlags_et *pFlags);
Dlgtte_err Dlgtte_SetFlags(HWND hwndCtl, Dlgtte_BitFlags_et flags);

Dlgtte_err Dlgtte_ShowUsageTooltip(HWND hwndCtl, bool is_show, int duration_millisec=0);
// -- This is used when you die for constantly refreshing usage-tip. And you need to launch 
//    a timer callback in PROC_DlgtteGetText to manually call Dlgtte_ShowUsageTooltip(),
//    with appropriate is_show=true/false.

Dlgtte_err Dlgtte_ShowContentTooltip(HWND hwndCtl, bool is_show);
// -- This can manually show/hide content-tooltip, useful when user does not enable
//    Dlgtte_AutoContentTipOnFocus flag.


// A facility function Dlgtte_EnableStaticUsageTooltip():
static inline const TCHAR* _dlgtte_static_text(HWND hwndCtl, void *userctx)
{
	(void)hwndCtl;
	return (const TCHAR*)userctx;
}
static inline Dlgtte_err Dlgtte_EnableStaticUsageTooltip(HWND hwndCtl, const TCHAR* usagetext)
{
	// Note: User's usagetext buffer must be persistent.
	return Dlgtte_EnableTooltip(hwndCtl, _dlgtte_static_text, (void*)usagetext);
}



///////////////////////////////////////////////////////////////
// Implementation Below:
///////////////////////////////////////////////////////////////

#ifdef DlgTooltipEasy_IMPL


#include <assert.h>
#include <windowsx.h>

#include <mswin/Tooltip-helper.h>

#define WinMultiMon_IMPL
#include <mswin/WinMultiMon.h>

#define CxxWindowSubclass_IMPL
#include <mswin/CxxWindowSubclass.h>

#ifndef DlgTooltipEasy_DEBUG
#include <CHHI_vaDBG_hide.h>
#endif

namespace Dlgtte // (internal) 
{

static const TCHAR *sig_EasyHottool = _T("sig_EasyHottool");
static const TCHAR *sig_EasyTooltipMan = _T("sig_EasyTooltipMan");

// Window-message: Get subclass-C++ object from HWND
static UINT s_MSG_GetSubclassCxxObj = 0;


inline int ptmid(int a, int b) { return (a + b) / 2; }
inline int midx(const RECT &rc) { return (rc.left + rc.right) / 2; }
inline int midy(const RECT &rc) { return (rc.top + rc.bottom) / 2; }
inline int rcwidth(const RECT &rc) { return rc.right - rc.left; }
inline int rcheight(const RECT &rc) { return rc.bottom - rc.top; }


BOOL SetTooltipWidth_by_MousePos(int screenx, int screeny, HWND hwndTooltiop)
{
	// This function considers multiple-monitor environment.
	// Set TTM_SETMAXTIPWIDTH to half of [screenx,screeny]'s monitor width.

	RECT rcMon = {};
	bool isok = getMonitorRectByPoint(screenx, screeny, &rcMon);
	if (!isok)
	{
		const int defwidth = 600;
		vaDBG2(_T("Unexpect! getMonitorRectByPoint() fail. Use %d as TTM_SETMAXTIPWIDTH."), defwidth);
		SendMessage(hwndTooltiop, TTM_SETMAXTIPWIDTH, 0, defwidth);
		return FALSE;
	}

	int ttwidth = rcwidth(rcMon) / 2;
	assert(ttwidth>=40);
	SendMessage(hwndTooltiop, TTM_SETMAXTIPWIDTH, 0, ttwidth);
//	vaDBG2(_T("Use %d as TTM_SETMAXTIPWIDTH."), ttwidth);

	return TRUE;
}


struct GetTextCallbacks_st
{
	PROC_DlgtteGetText *getUsageText;
	void *uctxUsage;

	PROC_DlgtteGetText *getContentText;
	void *uctxContent;

	Dlgtte_BitFlags_et flags;
};

class CTooltipMan;
class CContentTooltipPeeker;

class CHottoolSubsi : public CxxWindowSubclass
{
	friend CTooltipMan;

public:
	CHottoolSubsi();
	virtual ~CHottoolSubsi();

	virtual LRESULT WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		bool MsgDone = false;
		LRESULT lre = in_WndProc(hwnd, uMsg, wParam, lParam, &MsgDone);
		
		if (MsgDone)
			return lre;
		else
			return DefSubclassProc(hwnd, uMsg, wParam, lParam);
	}
	
	void AssignCallback(CTooltipMan *pTooltipMan, const GetTextCallbacks_st &gtcb)
	{
		m_pTooltipMan = pTooltipMan;

		m_getUsageText = gtcb.getUsageText;
		m_uctxUsage = gtcb.uctxUsage;
		m_getContentText = gtcb.getContentText;
		m_uctxContent = gtcb.uctxContent;

		m_flags = gtcb.flags;
	}

	const TCHAR *Call_getUsageText(HWND hwndUic)
	{
		if (m_getUsageText)
		{
			return m_getUsageText(hwndUic, m_uctxUsage);
		}
		else
			return nullptr;
	}

	const TCHAR *Call_getContentText(HWND hwndUic)
	{
		if (m_getContentText)
		{
			return m_getContentText(hwndUic, m_uctxContent);
		}
		else
			return nullptr;
	}

	void SetHwndttUsage(HWND httUsage)
	{
		m_hwndttUsage = httUsage;
	}

	void SetHwndttContent(HWND httContent)
	{
		m_hwndttContent = httContent;
	}

	Dlgtte_BitFlags_et GetFlags()
	{
		return m_flags;
	}

	bool SetFlags(Dlgtte_BitFlags_et flags)
	{
		m_flags = flags;
		return true;
	}

	Dlgtte_err ShowContentTooltip(bool is_show);

	Dlgtte_err ShowUsageTooltip(bool is_show, int duration_millisec=0); // 0=default


private:
	LRESULT in_WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool *pMsgDone);

	enum { OffScreenPosX = -32001, OffScreenPosY = -32001 };

	void Clear_rcFinal() {
		SetRect(&m_rcFinal, OffScreenPosX, OffScreenPosY, OffScreenPosX, OffScreenPosY);
	}

	void ReActivateContentTooltip();

	static void CALLBACK Delayed_ToggleActivateContentTooltip(HWND hdlg, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
	{
		((CHottoolSubsi*)idEvent)->ReActivateContentTooltip();
		KillTimer(hdlg, idEvent);
	}

	void StopHideUsageTipTimer();

	static void CALLBACK TimerProc_StopHideUsageTipTimer(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
	{
		CHottoolSubsi *phot = (CHottoolSubsi*)idEvent;
		assert(hwnd == phot->m_hwndttUsage);

		TOOLINFO ti = {sizeof(TOOLINFO)};
		ti.hwnd = GetParent(phot->m_hwnd);
		ti.uId = (UINT_PTR)phot->m_hwnd;
		SendMessage(hwnd, TTM_TRACKACTIVATE, FALSE,	(LPARAM)&ti);

		// This is call-once timer, so we stop the timer right now.
		phot->StopHideUsageTipTimer();
	}

private:
	CTooltipMan *m_pTooltipMan;

	HWND m_hwndttUsage;
	HWND m_hwndttContent;

	PROC_DlgtteGetText *m_getUsageText;
	void *m_uctxUsage;
	
	PROC_DlgtteGetText *m_getContentText;
	void *m_uctxContent;

	Dlgtte_BitFlags_et m_flags;

	RECT m_rcFinal; 

	UINT_PTR m_timeridHideUsageTip;
};

CHottoolSubsi::CHottoolSubsi()
{
	m_pTooltipMan = nullptr;

	m_hwndttUsage = NULL;
	m_hwndttContent = NULL;

	m_getUsageText = m_getContentText = nullptr;
	m_uctxUsage = m_uctxContent = nullptr;

	m_flags = Dlgtte_Flags0;

	Clear_rcFinal();

	m_timeridHideUsageTip = 0;
}

CHottoolSubsi::~CHottoolSubsi()
{
	StopHideUsageTipTimer();
}

void CHottoolSubsi::StopHideUsageTipTimer()
{
	if (m_timeridHideUsageTip != 0)
	{
		assert(m_hwndttUsage);

		KillTimer(m_hwndttUsage, m_timeridHideUsageTip);
		m_timeridHideUsageTip = 0;
	}
}

class CTooltipMan : public CxxWindowSubclass
{
	friend CHottoolSubsi;
	friend CContentTooltipPeeker;

public:
	CTooltipMan()
	{
		m_httUsage = NULL;
		m_httContent = NULL;

		m_suppress_content_tip_once = false;

		m_pszRecentContentText = nullptr;

		m_dbg_delay1 = 0;
	}

	ReCode_et AddUic(HWND hwndUic, const GetTextCallbacks_st &gtcb);
	// -- note: a second call will overwrite previous gtcb

	ReCode_et DelUic(HWND hwndUic);

	virtual LRESULT WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	CHottoolSubsi* FindHottoolByHwnd(HWND hwndUic);

	void GetTooltipHwnd(HWND *pUsageTooltip, HWND *pContentTooltip)
	{
		*pUsageTooltip = m_httUsage;
		*pContentTooltip = m_httContent;
	}

private:
	// Two tooltip-window HWND-s
	HWND m_httUsage;
	HWND m_httContent;

	bool m_suppress_content_tip_once;

	const TCHAR *m_pszRecentContentText; 
	// -- recent content tooltip text, can right-click copy to clipboard

	int m_dbg_delay1;
};


//////////////////////////////////////////////////////////////////////////

static void ReActivateTrackingTooltip(HWND hwndTooltip, TOOLINFO &ti)
{
	// Purpose: Force tooltip libcode to re-draw tooltip text so the tooltip 
	// window RECT is updated. 
	// Caller can query GetWindowRect() after ReActivateTrackingTooltip() returns.

	SendMessage(hwndTooltip, TTM_TRACKACTIVATE, FALSE, (LPARAM)&ti);

	SendMessage(hwndTooltip, TTM_TRACKACTIVATE, TRUE, (LPARAM)&ti);
	// -- this will internally call TTM_NEEDTEXT

	// Note: If we lack the TTM_TRACKACTIVATE=FALSE step, and we did not do
	// TTM_TRACKPOSITION to change tooltip position recently, tooltip libcode 
	// does *nothing* inside TTM_TRACKACTIVATE=TRUE.
}


static bool QueryTooltipRect_by_TrackPoint(HWND hwndTooltip, TOOLINFO &ti,
	int screenx, int screeny, RECT *pRect)
{
	vaDBG2(_T("TTM_TRACKPOSITION at screen-pos: x=%d, y=%d"), screenx, screeny);

	SendMessage(hwndTooltip, TTM_TRACKPOSITION, 0, (LPARAM)MAKELONG(screenx, screeny));

	ReActivateTrackingTooltip(hwndTooltip, ti);

	GetWindowRect(hwndTooltip, pRect);
	TCHAR rctext[80] = _T("");
	vaDBG2(_T("TTM_TRACKPOSITION done, TT-rect: %s"), RECTtext(*pRect, rctext));

	return true;
}


Dlgtte_err
CHottoolSubsi::ShowUsageTooltip(bool is_show, int duration_millisec)
{
	// This function is required only when user wants to preserve a usage-tip on the screen.
	// For example, when mouse hovering on a button, the tooltip refreshes a timestamp text
	// every 0.5 second.

	HWND hdlg = GetParent(m_hwnd);
	HWND hUic = m_hwnd;
	HWND hwndTooltip = m_hwndttUsage;

	TOOLINFO ti = { sizeof(TOOLINFO) };
	ti.hwnd = hdlg;
	ti.uId = (UINT_PTR)hUic;

	if (duration_millisec <= 0)
	{
		duration_millisec = SendMessage(hwndTooltip, TTM_GETDELAYTIME, TTDT_AUTOPOP, 0);
		
		if(duration_millisec<=0)
			duration_millisec = 9999;
	}

	vaDBG2(_T("Hottool(0x%X) ShowUsageTooltip(%s, %dms)."), 
		PtrToUint(hUic), 
		is_show?_T("true"):_T("false"),
		duration_millisec);

	// Yes, TTM_TRACKACTIVATE is effective on non-TTF_TRACK tooltip.
	// And we need a off/on toggle so that the usage-tip really refreshes its display.

	SendMessage(hwndTooltip, TTM_TRACKACTIVATE, FALSE, (LPARAM)&ti);

	if(is_show)
	{
		SendMessage(hwndTooltip, TTM_TRACKACTIVATE,
			TRUE, 
			(LPARAM)&ti);

		// Set a timer to turn off the usage tooltip.
		// If not doing this, the usage tooltip will persist on the screen and 
		// "mouse hovering on other hottools" will not activate.

		m_timeridHideUsageTip = SetTimer(hwndTooltip, (UINT_PTR)this, duration_millisec, 
			TimerProc_StopHideUsageTipTimer);
	}

	return Dlgtte_Succ;
}

Dlgtte_err 
CHottoolSubsi::ShowContentTooltip(bool is_show)
{
	HWND hdlg = GetParent(m_hwnd);
	HWND hUic = m_hwnd;
	HWND hwndTooltip = m_hwndttContent;

	TOOLINFO ti = { sizeof(TOOLINFO) };
	ti.hwnd = hdlg;
	ti.uId = (UINT_PTR)hUic;

	if(!is_show)
	{
		vaDBG2(_T("Hottool(0x%X) Will hide Content tooltip."), PtrToUint(hUic));

		SendMessage(hwndTooltip, TTM_TRACKACTIVATE, FALSE, (LPARAM)&ti);
		return Dlgtte_Succ;
	}

	assert(is_show==true);

	this->Clear_rcFinal();

	vaDBG2(_T("Hottool(0x%X) Will show Content tooltip."), PtrToUint(hUic));

	// We will select Uic's upper-middle or lower-middle as "track-point" to display the tooltip.
	// Selecting upper-middle means showing tooltip upside of the Uic, balloon-stem down.
	// Selecting lower-middle means showing tooltip downside of the Uic, balloon-stem up.

	RECT rcUic = {};
	GetWindowRect(hUic, &rcUic);

	RECT rcMon = {};
	bool isok = getMonitorRectByPoint(midx(rcUic), midy(rcUic), &rcMon);

	// Set tooltip display width to half monitor width. (also enables multiline tooltip)
	SetTooltipWidth_by_MousePos(midx(rcUic), midy(rcUic), hwndTooltip);

	bool finalAbove = false; // true: finally show tooltip above Uic
	RECT rcTooltip = {};

	if ((m_flags & Dlgtte_BalloonDown)==0)
	{
		// User prefers Dlgtte_BalloonUp

		// probe-show the tooltip at monitor bottom, and peek its rect in rcTooltip
		QueryTooltipRect_by_TrackPoint(hwndTooltip, ti, midx(rcUic), rcMon.bottom-1, &rcTooltip);

		if (
			rcheight(rcTooltip) <= rcUic.top - rcMon.top // Upside-space of Uic is enough for tooltip
			|| rcheight(rcTooltip) > rcMon.bottom - rcUic.bottom // Downside-space is not enough
			)
		{	// Use upside-space
			finalAbove = true;
		}
		else
		{	// Upside-space not enough, but downside-space enough, use downside.

			// probe-show the tooltip at monitor top, and peek its rect in rcTooltip
			QueryTooltipRect_by_TrackPoint(hwndTooltip, ti, midx(rcUic), rcMon.top, &rcTooltip);

			finalAbove = false; // ok, Downside enough, use it
		}
	}
	else // User prefers Dlgtte_BalloonDown
	{
		assert((m_flags & Dlgtte_BalloonDown) == Dlgtte_BalloonDown);

		// probe-show the tooltip at monitor top, and peek its rect in rcTooltip
		QueryTooltipRect_by_TrackPoint(hwndTooltip, ti, midx(rcUic), rcMon.top, &rcTooltip);

		if(
			rcheight(rcTooltip) <= rcMon.bottom - rcUic.bottom // Downside-space of Uic is enough for tooltip
			|| rcheight(rcTooltip) > rcUic.top - rcMon.top // Upside-space is not enough
			)
		{	// Use downside-space
			finalAbove = false;
		}
		else
		{	// Downside-space not enough, but upside-space enough, use upside.

			// probe-show the tooltip at monitor bottom, and peek its rect in rcTooltip
			QueryTooltipRect_by_TrackPoint(hwndTooltip, ti, midx(rcUic), rcMon.bottom-1, &rcTooltip);

			finalAbove = true; // ok, Upside is enough, use it
		}
	}

	m_rcFinal.left = rcTooltip.left;
	m_rcFinal.right = rcTooltip.right;

	if (finalAbove)
	{
		m_rcFinal.top = rcUic.top - rcheight(rcTooltip);
		m_rcFinal.bottom = rcUic.top;
	}
	else
	{
		m_rcFinal.top = rcUic.bottom;
		m_rcFinal.bottom = m_rcFinal.top + rcheight(rcTooltip);
	}

	TCHAR rctext[80] = _T("");
	vaDBG2(_T("In ShowContentTooltip(), [%s]tooltip-rect: %s"), 
		finalAbove ? _T("UP") : _T("DN"),
		RECTtext(m_rcFinal, rctext));

	// We must force toggle TTM_TRACKACTIVATE off/on, so that tooltip refreshes its 
	// display position according to our new m_rcFinal.
	if(m_pTooltipMan->m_dbg_delay1==0)
		ReActivateContentTooltip();
	else
		SetTimer(hdlg, (UINT_PTR)this, 1000, Delayed_ToggleActivateContentTooltip);

	return Dlgtte_Succ;
}

void 
CHottoolSubsi::ReActivateContentTooltip()
{
	TOOLINFO ti = { sizeof(TOOLINFO) };
	ti.hwnd = GetParent(m_hwnd);
	ti.uId = (UINT_PTR)m_hwnd;

	ReActivateTrackingTooltip(m_hwndttContent, ti);
}

LRESULT 
CHottoolSubsi::in_WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool *pMsgDone)
{
	assert(m_hwnd==hwnd);

	*pMsgDone = false;

	if (uMsg == s_MSG_GetSubclassCxxObj)
	{
		*pMsgDone = true;
		return (LRESULT)this;
	}


	if (m_hwndttContent)
	{
		if (uMsg == WM_SETFOCUS)
		{
			if ((m_flags & Dlgtte_AutoContentTipOnFocus) == 0)
				return 0;

			if (m_pTooltipMan->m_suppress_content_tip_once)
			{
				// Purpose: If user click content-tooltip-window itself to close the tooltip, 
				// the focus goes back to the hottool. This time we will not show up that
				// tooltip repeatedly. 

				m_pTooltipMan->m_suppress_content_tip_once = false;
				return 0;
			}

			ShowContentTooltip(true);		
		}
		else if (uMsg == WM_KILLFOCUS)
		{
			HWND hwndGainFocus = (HWND)wParam;

			if (hwndGainFocus==m_pTooltipMan->m_httContent)
			{
				// We get here if user clicks on the content-tooltip.
				// According to our design, that click will close(=hide) the tooltip.
				m_pTooltipMan->m_suppress_content_tip_once = true;
			}

			ShowContentTooltip(false);
		}
	
	} // m_hwndttContent is true

	return 0;
}

//////////////////////////////////////////////////////////////////////////

CxxWindowSubclass::ReCode_et 
CTooltipMan::AddUic(HWND hwndUic, const GetTextCallbacks_st &gtcb)
{
	if (s_MSG_GetSubclassCxxObj == 0)
	{
		s_MSG_GetSubclassCxxObj = RegisterWindowMessage(_T("Dlgtte-GetSubclassCxxObj"));
	}

	HWND hdlg = m_hwnd;

	if (gtcb.getUsageText == nullptr && gtcb.getContentText == nullptr)
		return E_BadParam;

	CxxWindowSubclass::ReCode_et err = CxxWindowSubclass::E_Fail;
	CHottoolSubsi *phottool =
		CxxWindowSubclass::FetchCxxobjFromHwnd<CHottoolSubsi>(
			hwndUic, sig_EasyHottool, TRUE, &err);
	assert(phottool);

	if (gtcb.getUsageText)
	{
		if (!m_httUsage)
		{
			m_httUsage = CreateWindowEx(
				WS_EX_TOPMOST | WS_EX_TRANSPARENT,
				TOOLTIPS_CLASS,
				NULL, // window title
				TTS_NOPREFIX | TTS_ALWAYSTIP, // not TTS_BALLOON
				0, 0, 0, 0,
				hdlg, // tooltip-window's owner
				NULL, NULL, NULL);
			assert(m_httUsage);

			if (m_httUsage)
			{
				vaDBG1(_T("[+]Usage-Tooltip created, hwnd=0x%X"), PtrToUint(m_httUsage));
			}
		}

		// Associate Uic to the tooltip.

		TOOLINFO ti = { sizeof(TOOLINFO) };
		ti.uFlags = TTF_IDISHWND | TTF_SUBCLASS;
		ti.hwnd = hdlg;
		ti.uId = (UINT_PTR)hwndUic;
		ti.lpszText = LPSTR_TEXTCALLBACK;

		BOOL succ = do_TTM_ADDTOOL_nodup(m_httUsage, ti);
		assert(succ);

		phottool->SetHwndttUsage(m_httUsage);

		// Make the tooltip appear quickly (100ms), instead of default delaying 500ms.
		SendMessage(m_httUsage, TTM_SETDELAYTIME, TTDT_INITIAL, 100);
		// 
		SendMessage(m_httUsage, TTM_SETDELAYTIME, TTDT_AUTOPOP, 29000);
	}

	if (gtcb.getContentText)
	{
		if (!m_httContent)
		{
			m_httContent = CreateWindowEx(
				WS_EX_TOPMOST, // no WS_EX_TRANSPARENT to enable click,
				TOOLTIPS_CLASS,
				NULL, // window title
				TTS_NOPREFIX | TTS_ALWAYSTIP | TTS_BALLOON, // want balloon style
				0, 0, 0, 0,
				hdlg, // tooltip-window's owner
				NULL, NULL, NULL);
			assert(m_httContent);

			if (m_httContent)
			{
				vaDBG1(_T("[+]Content-Tooltip created, hwnd=0x%X"), PtrToUint(m_httContent));

				CContentTooltipPeeker *psp = CxxWindowSubclass::FetchCxxobjFromHwnd<CContentTooltipPeeker>(
					m_httContent, _T("sig_CContentTooltipPeeker"), TRUE);
				assert(psp);
			}
		}

		// Associate Uic to the tooltip.

		TOOLINFO ti = { sizeof(TOOLINFO) };
		ti.uFlags = TTF_IDISHWND | TTF_TRACK | TTF_CENTERTIP; 
		// -- we "track" the tooltip manually
		// -- Without TTF_ABSOLUTE, we get correct top/bottom-edge probe-shown balloon stem pointing.
		ti.hwnd = hdlg;
		ti.uId = (UINT_PTR)hwndUic;
		ti.lpszText = LPSTR_TEXTCALLBACK;

		BOOL succ = do_TTM_ADDTOOL_nodup(m_httContent, ti);
		assert(succ);

		phottool->SetHwndttContent(m_httContent);
	}

	phottool->AssignCallback(this, gtcb);

	return E_Success;
}

LRESULT 
CTooltipMan::WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_NOTIFY)
	{
		NMHDR *pnmh = (NMHDR *)lParam;
		HWND hwndTooltip = pnmh->hwndFrom;
		HWND hwndUic = (HWND)pnmh->idFrom;

		if (pnmh->code == TTN_NEEDTEXT)
		{
			CHottoolSubsi *phot = 
				CxxWindowSubclass::FetchCxxobjFromHwnd<CHottoolSubsi>(
					hwndUic, sig_EasyHottool, FALSE);

			vaDBG2(_T("TTN_NEEDTEXT from hwndTooltip=0x%X for Uic=0x%X."), hwndTooltip, hwndUic);

			if(phot)
			{
				const TCHAR *ptext = nullptr;

				if (hwndTooltip == m_httUsage)
				{ 
					do_TTM_SETTITLE_ClearTitle(hwndTooltip);

					POINT pt = {};
					GetCursorPos(&pt);
					SetTooltipWidth_by_MousePos(pt.x, pt.y, hwndTooltip);

					ptext = phot->Call_getUsageText(hwndUic);
				}
				else if (hwndTooltip == m_httContent)
				{ 
					do_TTM_SETTITLE_ClearTitle(hwndTooltip);

					ptext = phot->Call_getContentText(hwndUic);

					m_pszRecentContentText = ptext;
				}

				// WinAPI behavior note: 
				// If user's ptext is NULL or ptext[0] is NUL, the tooltip will not be displayed.

				NMTTDISPINFO *pdi = (NMTTDISPINFO *)pnmh;
				pdi->lpszText = (LPTSTR)ptext;
			}
		}
		else if (pnmh->code == TTN_SHOW)
		{
			if (hwndTooltip == m_httContent)
			{
				TCHAR rctext[80] = _T("");
				
				RECT rcInit = {}; 
				GetWindowRect(hwndTooltip, &rcInit);
				vaDBG2(_T("TTN_SHOW from 0x%X,  init-Rect=%s"), hwndTooltip, RECTtext(rcInit, rctext));

				CHottoolSubsi *phot = (CHottoolSubsi*)SendMessage(hwndUic, s_MSG_GetSubclassCxxObj, 0, 0);
				RECT &rcFinal = phot->m_rcFinal;

				vaDBG2(_T("TTN_SHOW from 0x%X, final-Rect=%s"), hwndTooltip, RECTtext(rcFinal, rctext));

				if (rcFinal.left != CHottoolSubsi::OffScreenPosX) // rcFinal has valid value
				{
					int finalx = rcFinal.left;
					int finaly = rcFinal.top;

					vaDBG2(_T("TTN_SHOW final: move tooltip to LT(%d,%d)"), finalx, finaly);

					SetWindowPos(hwndTooltip, 0, 
						finalx, finaly, 0, 0, 
						SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOSIZE);
				}
				else
				{
					// This happens inside QueryTooltipRect_by_TrackPoint().
					// Now tooltip is at our probing position, we should better move it off-screen 
					// to avoid user seeing a flicking temporal tooltip at that position.
					// But for pedagogical purpose, using EXE debugger to set m_dbg_delay1 to 1000 
					// to see what is under the hood.

					if(m_dbg_delay1==0)
					{
						int tempx = rcInit.left; // note: keep .left intact
						int tempy = CHottoolSubsi::OffScreenPosY; // an offscreen coord

						vaDBG2(_T("TTN_SHOW probing: move tooltip offscreen to LT(%d,%d)"), tempx, tempy);

						SetWindowPos(hwndTooltip, 0, 
							tempx, tempy, 0, 0, 
							SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOSIZE);
					}
				}

				// return TRUE to make SetWindowPos() effective, no calling into DefSubclassProc().
				return TRUE;
			}
		}
	}
	else if (uMsg == WM_MOVE || uMsg == WM_SIZE)
	{	
		// If window moved, hide the tooltip
		SendMessage(m_httContent, TTM_ACTIVATE, FALSE, 0);
		SendMessage(m_httContent, TTM_ACTIVATE, TRUE, 0);
	}
	else if (uMsg == WM_KILLFOCUS)
	{
		// Seems we cannot see this on a standard dialogbox.
		vaDBG2(_T("CTooltipMan::WndProc() sees WM_KILLFOCUS"));
	}
 	else if (uMsg == s_MSG_GetSubclassCxxObj)
 	{
 		return (LRESULT)this;
 	}

	return DefSubclassProc(hwnd, uMsg, wParam, lParam);
}


static bool HasAnyHottool(HWND hwndTooltip)
{
/*
	// OK but verbose.
	TOOLINFO ti = { sizeof(TOOLINFO) };
	BOOL any = (BOOL)SendMessage(hwndTooltip, TTM_ENUMTOOLS, 0, (LPARAM)&ti);
	return any ? true : false;
*/
	LRESULT count = SendMessage(hwndTooltip, TTM_GETTOOLCOUNT, 0, 0);
	return count == 0 ? false : true;
}

CxxWindowSubclass::ReCode_et
CTooltipMan::DelUic(HWND hwndUic)
{
	ReCode_et err_th = CxxWindowSubclass::E_Fail;
	CHottoolSubsi *pth = CxxWindowSubclass::FetchCxxobjFromHwnd<CHottoolSubsi>(
		hwndUic, sig_EasyHottool, FALSE, &err_th);

	if (err_th!=E_Existed)
	{
		vaDBG1(_T("!! CTooltipMan::DelUic(hwndUic=0x%X), wrong HWND input, ReCode_et=%d"), 
			PtrToUint(hwndUic), err_th);
		return err_th;
	}

	assert(pth);

	ReCode_et err = pth->DetachHwnd(true);
	assert(!err);

	HWND hdlg = m_hwnd;

	// Do TTM_DELTOOL on the dual tooltip-windows.

	TOOLINFO ti = { sizeof(TOOLINFO) };
	ti.hwnd = hdlg;
	ti.uId = (UINT_PTR)hwndUic;

	bool anyUsageTip = false;
	bool anyContentTip = false;

	if(m_httUsage)
	{
		SendMessage(m_httUsage, TTM_DELTOOL, 0, (LPARAM)&ti);
		
		anyUsageTip = HasAnyHottool(m_httUsage);
	}
	
	if(m_httContent)
	{
		SendMessage(m_httContent, TTM_DELTOOL, 0, (LPARAM)&ti);

		anyContentTip = HasAnyHottool(m_httContent);
	}

	if(!anyUsageTip && !anyContentTip)
	{
		// Cleanup tooltip-window resource

		if (m_httUsage) {
			DestroyWindow(m_httUsage);

			vaDBG1(_T("[-]In CTooltipMan::DelUic(), Usage-Tooltip destroyed, hwnd=0x%X"), PtrToUint(m_httUsage));

			m_httUsage = NULL;
		}

		if (m_httContent) {
			DestroyWindow(m_httContent);

			vaDBG1(_T("[-]In CTooltipMan::DelUic(), Content-Tooltip destroyed, hwnd=0x%X"), PtrToUint(m_httContent));

			m_httContent = NULL;
		}

		this->DetachHwnd(true);

		vaDBG1(_T("[-]In CTooltipMan::DelUic(), destroyed CTooltipMan=%p "), this);
	}

	return E_Success;
}

//// tooltip-window subclass ////

static BOOL
openclipboard_with_timeout(DWORD millisec, HWND hwnd)
{
	DWORD msec_start = GetTickCount();
	do
	{
		if (OpenClipboard(hwnd))
			return TRUE;
	} while (GetTickCount() - msec_start < millisec);
	return FALSE;
}

BOOL
dlgtteSetClipboardText(const TCHAR *pszText)
{
	BOOL b = FALSE;
	HANDLE hret = NULL;

	if(!pszText)
		return FALSE;

	int textchars = lstrlen(pszText);
	int textchars_ = textchars + 1;

	HGLOBAL hmem = GlobalAlloc(GPTR, textchars_ * sizeof(TCHAR));
	if (!hmem)
		return FALSE;

	TCHAR *pmem = (TCHAR*)GlobalLock(hmem);
	lstrcpyn(pmem, pszText, textchars_);
	GlobalUnlock(hmem);

	if (!openclipboard_with_timeout(1000, NULL)) {
		goto FAIL_FREE_HMEM;
	}

	b = EmptyClipboard();
	assert(b);

	hret = SetClipboardData(sizeof(TCHAR) == 1 ? CF_TEXT : CF_UNICODETEXT, hmem);
	if (!hret) {
		goto FAIL_FREE_HMEM;
	}

	CloseClipboard();
	return TRUE;

FAIL_FREE_HMEM:
	CloseClipboard();
	GlobalFree(hmem);
	return FALSE;
}

class CContentTooltipPeeker : public CxxWindowSubclass
{
public:
	virtual LRESULT WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		HWND hwndTooltip = hwnd;

		if (uMsg == WM_LBUTTONDOWN)
		{
			vaDBG2(_T("CContentTooltipPeeker sees WM_LBUTTONDOWN. Hide myself."));
			ShowWindow(hwnd, SW_HIDE);
		}
		else if (uMsg == WM_RBUTTONDOWN)
		{
			vaDBG2(_T("CContentTooltipPeeker sees WM_RBUTTONDOWN. Hide myself and copy text to clipboard."));

			HWND hdlg = GetAncestor(hwndTooltip, GA_ROOTOWNER);
			CTooltipMan *ptm = (CTooltipMan*)SendMessage(hdlg, s_MSG_GetSubclassCxxObj, 0, 0);
			if(ptm)
			{
				dlgtteSetClipboardText(ptm->m_pszRecentContentText);
			}
			else
			{
				vaDBG(_T("Unexpect! CTooltipMan s_MSG_GetSubclassCxxObj query fail!"));
			}

			ShowWindow(hwnd, SW_HIDE);
		}

		return DefSubclassProc(hwnd, uMsg, wParam, lParam);
	}
};


} // (internal) namespace Dlgtte


using namespace Dlgtte;


Dlgtte_err Dlgtte_EnableTooltip(HWND hwndCtl,
	PROC_DlgtteGetText *getUsageText, void *uctxUsage,
	PROC_DlgtteGetText *getContentText, void *uctxContent, 
	Dlgtte_BitFlags_et flags)
{
	HWND hdlg = GetParent(hwndCtl);
	if(!hdlg)
		return Dlgtte_InvalidHwnd;

	CxxWindowSubclass::ReCode_et err = CxxWindowSubclass::E_Fail;
	CTooltipMan *ptm = CxxWindowSubclass::FetchCxxobjFromHwnd<CTooltipMan>(
		hdlg, sig_EasyTooltipMan, TRUE, &err);

	if (err == CxxWindowSubclass::E_Success)
	{
		vaDBG1(_T("[+]Dlgtte_EnableTooltip(hwndCtl=0x%X) hwndParent=0x%X, created CTooltipMan=%p."), 
			PtrToUint(hwndCtl), PtrToUint(hdlg), ptm);
	}
	else if (err == CxxWindowSubclass::E_Existed)
	{
		vaDBG2(_T("   Dlgtte_EnableTooltip(hwndCtl=0x%X) hwndParent=0x%X, existed CTooltipMan=%p."),
			PtrToUint(hwndCtl), PtrToUint(hdlg), ptm);
	}
	else
	{
		vaDBG(_T("Dlgtte_EnableTooltip(hwndCtl=0x%X) hwndParent=0x%X, fail with CxxWindowSubclass::ReCode_et(%d)."),
			PtrToUint(hwndCtl), PtrToUint(hdlg), err);
		return Dlgtte_Fail;
	}

	GetTextCallbacks_st gtcb = { getUsageText, uctxUsage, 
		getContentText, uctxContent, flags };

	err = ptm->AddUic(hwndCtl, gtcb);

	return err ? Dlgtte_Fail : Dlgtte_Succ;
}


bool Dlgtte_IsEnabled(HWND hwndCtl)
{
	// Check whether hwndCtl has been installed a DlgTooltipEasy facility.

	CHottoolSubsi *pth = CxxWindowSubclass::FetchCxxobjFromHwnd<CHottoolSubsi>(
		hwndCtl, sig_EasyHottool, FALSE);
	
	if (pth)
		return true;
	else
		return false;
}


Dlgtte_err Dlgtte_RemoveTooltip(HWND hwndCtl)
{
	HWND hdlg = GetParent(hwndCtl);
	if (!hdlg)
		return Dlgtte_InvalidHwnd;

	CxxWindowSubclass::ReCode_et err_tm = CxxWindowSubclass::E_Fail;
	CTooltipMan *ptm = CxxWindowSubclass::FetchCxxobjFromHwnd<CTooltipMan>(
		hdlg, sig_EasyTooltipMan, FALSE, &err_tm);

	CxxWindowSubclass::ReCode_et err_th = CxxWindowSubclass::E_Fail;
	CHottoolSubsi *pth = CxxWindowSubclass::FetchCxxobjFromHwnd<CHottoolSubsi>(
		hwndCtl, sig_EasyHottool, FALSE, &err_th);

	if (err_tm == CxxWindowSubclass::E_NotExist)
	{
		// Since there is no CTooltipMan, there should be no CxxSubclassHottool as well.

		assert(err_th == CxxWindowSubclass::E_NotExist);
		return Dlgtte_Succ;
	}

	if (err_th == CxxWindowSubclass::E_NotExist)
		return Dlgtte_Succ; // already removed, nothing to do

	ptm->DelUic(hwndCtl);

	return Dlgtte_Succ;
}

void Dlgtte_GetTooltipHwnd(HWND hwndCtl, HWND *pUsageTooltip, HWND *pContentTooltip)
{
	SETTLE_OUTPUT_PTR(HWND, pUsageTooltip, NULL);
	SETTLE_OUTPUT_PTR(HWND, pContentTooltip, NULL);

	HWND hdlg = GetParent(hwndCtl);

	CxxWindowSubclass::ReCode_et err = CxxWindowSubclass::E_Fail;
	CTooltipMan *ptm = CxxWindowSubclass::FetchCxxobjFromHwnd<CTooltipMan>(
		hdlg, sig_EasyTooltipMan, FALSE, &err);

	if(!ptm)
		return;

	assert(err==CxxWindowSubclass::E_Existed);

	ptm->GetTooltipHwnd(pUsageTooltip, pContentTooltip);
}

Dlgtte_err Dlgtte_GetFlags(HWND hwndCtl, Dlgtte_BitFlags_et *pFlags)
{
	if (!IsWindow(hwndCtl))
		return Dlgtte_InvalidHwnd;

	CHottoolSubsi *phot = (CHottoolSubsi*)SendMessage(hwndCtl, s_MSG_GetSubclassCxxObj, 0, 0);
	if (!phot)
		return Dlgtte_InvalidHottool;

	*pFlags = phot->GetFlags();
	
	return Dlgtte_Succ;
}

Dlgtte_err Dlgtte_SetFlags(HWND hwndCtl, Dlgtte_BitFlags_et flags)
{
	if(!IsWindow(hwndCtl))
		return Dlgtte_InvalidHwnd;

	CHottoolSubsi *phot = (CHottoolSubsi*)SendMessage(hwndCtl, s_MSG_GetSubclassCxxObj, 0, 0);
	if(!phot)
		return Dlgtte_InvalidHottool;

	phot->SetFlags(flags);

	return Dlgtte_Succ;
}

Dlgtte_err Dlgtte_ShowUsageTooltip(HWND hwndCtl, bool is_show, int duration_millisec)
{
	if (!IsWindow(hwndCtl))
		return Dlgtte_InvalidHwnd;

	CHottoolSubsi *phot = (CHottoolSubsi*)SendMessage(hwndCtl, s_MSG_GetSubclassCxxObj, 0, 0);
	if (!phot)
		return Dlgtte_InvalidHottool;

	return phot->ShowUsageTooltip(is_show, duration_millisec);
}

Dlgtte_err Dlgtte_ShowContentTooltip(HWND hwndCtl, bool is_show)
{
	if (!IsWindow(hwndCtl))
		return Dlgtte_InvalidHwnd;

	CHottoolSubsi *phot = (CHottoolSubsi*)SendMessage(hwndCtl, s_MSG_GetSubclassCxxObj, 0, 0);
	if (!phot)
		return Dlgtte_InvalidHottool;

	return phot->ShowContentTooltip(is_show);
}




#ifndef DlgTooltipEasy_DEBUG
#include <CHHI_vaDBG_hide.h>
#endif

#endif // DlgTooltipEasy_IMPL

#endif
