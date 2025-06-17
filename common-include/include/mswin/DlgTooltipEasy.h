#ifndef __DlgTooltipEasy_h_20250617_
#define __DlgTooltipEasy_h_20250617_

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

typedef const TCHAR* PROC_DlgtteGetText(HWND hwndUic, void *userctx);
// -- When callback, hwndUic tells which control(button, editbox etc) is requesting tooltip text.



Dlgtte_err Dlgtte_EnableTooltip(HWND hwndCtl, 
	PROC_DlgtteGetText *getUsageText, void *uctxUsage,
	PROC_DlgtteGetText *getContentText = nullptr, void *uctxContent = nullptr,
	Dlgtte_BitFlags_et flags = Dlgtte_Flags0);

Dlgtte_err Dlgtte_RemoveTooltip(HWND hwndCtl);

void Dlgtte_GetTooltipHwnd(HWND hwndCtl, HWND *pUsage=NULL, HWND *pContent=NULL);

Dlgtte_err Dlgtte_GetFlags(HWND hwndCtl, Dlgtte_BitFlags_et *pFlags);
Dlgtte_err Dlgtte_SetFlags(HWND hwndCtl, Dlgtte_BitFlags_et flags);

Dlgtte_err Dlgtte_ShowContentTooltip(HWND hwndCtl, bool is_show);
// -- This can manually show/hide content-tooltip, useful when user does not assign
//    Dlgtte_AutoContentTipOnFocus.



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

namespace Dlgtte // (internal) 
{

static const TCHAR *sig_EasyHottool = _T("sig_EasyHottool");
static const TCHAR *sig_EasyTooltipMan = _T("sig_EasyTooltipMan");

// Two window-messages:  get C++ object from HWND
static UINT s_MSG_GetHottoolSubsi = 0;


struct GetTextCallbacks_st
{
	PROC_DlgtteGetText *getUsageText;
	void *uctxUsage;

	PROC_DlgtteGetText *getContentText;
	void *uctxContent;

	Dlgtte_BitFlags_et flags;
};

class CTooltipMan;

class CHottoolSubsi : public CxxWindowSubclass
{
	friend CTooltipMan;

public:
	CHottoolSubsi();

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


private:
	LRESULT in_WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool *pMsgDone);

private:
	CTooltipMan *m_pTooltipMan;

	HWND m_hwndttContent;

	PROC_DlgtteGetText *m_getUsageText;
	void *m_uctxUsage;
	
	PROC_DlgtteGetText *m_getContentText;
	void *m_uctxContent;

	Dlgtte_BitFlags_et m_flags;

	RECT m_rcFinal; 
};

CHottoolSubsi::CHottoolSubsi()
{
	m_pTooltipMan = nullptr;

	m_hwndttContent = NULL;

	m_getUsageText = m_getContentText = nullptr;
	m_uctxUsage = m_uctxContent = nullptr;

	m_flags = Dlgtte_Flags0;

	SetRect(&m_rcFinal, -1, -1, -1, -1);
}

class CTooltipMan : public CxxWindowSubclass
{
	friend CHottoolSubsi;

public:
	CTooltipMan()
	{
		m_httUsage = NULL;
		m_httContent = NULL;

		m_suppress_content_tip_once = false;
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
};

//////////////////////////////////////////////////////////////////////////


static bool QueryTooltipRect_by_TrackPoint(HWND hwndTooltip, TOOLINFO &ti,
	int screenx, int screeny, RECT *pRect)
{
	SendMessage(hwndTooltip, TTM_TRACKPOSITION, 0, (LPARAM)MAKELONG(screenx, screeny));

	SendMessage(hwndTooltip, TTM_TRACKACTIVATE, TRUE, (LPARAM)&ti);
	// -- this will internally call TTM_NEEDTEXT

	GetWindowRect(hwndTooltip, pRect);
	TCHAR rctext[80];
	vaDbgTs(_T("After TTM_TRACKACTIVATE, TT-rect: %s"), RECTtext(*pRect, rctext));

	return true;
}

/*
static int QueryTooltipHeight_by_TrackPoint(HWND hwndTooltip, TOOLINFO &ti,
	int screenx, int screeny)
{
	RECT rc = {};
	QueryTooltipRect_by_TrackPoint(hwndTooltip, ti, screenx, screeny, &rc);
	return rc.bottom - rc.top;
}
*/


inline int ptmid(int a, int b) { return (a + b) / 2; }
inline int midx(const RECT &rc) { return (rc.left + rc.right) / 2; }
inline int midy(const RECT &rc) { return (rc.top + rc.bottom) / 2; }
inline int rcwidth(const RECT &rc) { return rc.right - rc.left; }
inline int rcheight(const RECT &rc) { return rc.bottom - rc.top; }

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
		vaDBG(_T("Hottool(0x%X) Will hide Content tooltip."), hUic);

		SendMessage(m_hwndttContent, TTM_TRACKACTIVATE, FALSE, (LPARAM)&ti);
		return Dlgtte_Succ;
	}

	assert(is_show==true);

	vaDBG(_T("Hottool(0x%X) Will show Content tooltip."), PtrToUint(hUic));

	// We will select Uic's upper-middle or lower-middle as "track-point" to display the tooltip.
	// Selecting upper-middle means showing tooltip upside of the Uic, balloon-stem down.
	// Selecting lower-middle means showing tooltip downside of the Uic, balloon-stem up.

	RECT rcUic = {};
	GetWindowRect(hUic, &rcUic);

	RECT rcMon = {};
	bool isok = getMonitorRectByPoint(midx(rcUic), midy(rcUic), &rcMon);

	bool dbg_isUp = false;
	RECT rcTooltip = {};

	if ((m_flags & Dlgtte_BalloonDown)==0)
	{
		// User prefers Dlgtte_BalloonUp

		// probe-show the tooltip at monitor bottom, and peek its rect in rcTooltip
		QueryTooltipRect_by_TrackPoint(hwndTooltip, ti, midx(rcMon), rcMon.bottom-1, &rcTooltip);

		if (
			rcheight(rcTooltip) <= rcUic.top - rcMon.top // Upside-space of Uic is enough for tooltip
			|| rcheight(rcTooltip) > rcMon.bottom - rcUic.bottom // Downside-space is not enough
			)
		{	// Use upside-space
			dbg_isUp = true;
		}
		else
		{	// Upside-space not enough, but downside-space enough, use downside.

			// probe-show the tooltip at monitor top, and peek its rect in rcTooltip
			QueryTooltipRect_by_TrackPoint(hwndTooltip, ti, midx(rcMon), rcMon.top, &rcTooltip);

			dbg_isUp = false; // ok, Downside enough, use it
		}
	}
	else // User prefers Dlgtte_BalloonDown
	{
		assert((m_flags & Dlgtte_BalloonDown) == Dlgtte_BalloonDown);

		// probe-show the tooltip at monitor top, and peek its rect in rcTooltip
		QueryTooltipRect_by_TrackPoint(hwndTooltip, ti, midx(rcMon), rcMon.top, &rcTooltip);

		if(
			rcheight(rcTooltip) <= rcMon.bottom - rcUic.bottom // Downside-space of Uic is enough for tooltip
			|| rcheight(rcTooltip) > rcUic.top - rcMon.top // Upside-space is not enough
			)
		{	// Use downside-space
			dbg_isUp = false;
		}
		else
		{	// Downside-space not enough, but upside-space enough, use upside.

			// probe-show the tooltip at monitor bottom, and peek its rect in rcTooltip
			QueryTooltipRect_by_TrackPoint(hwndTooltip, ti, midx(rcMon), rcMon.bottom-1, &rcTooltip);

			dbg_isUp = true; // ok, Upside is enough, use it
		}
	}

	m_rcFinal.left = midx(rcUic) - rcwidth(rcTooltip) / 2;
	m_rcFinal.right = m_rcFinal.left + rcwidth(rcTooltip);

	if (dbg_isUp)
	{
		m_rcFinal.top = rcUic.top - rcheight(rcTooltip);
		m_rcFinal.bottom = rcUic.top;
	}
	else
	{
		m_rcFinal.top = rcUic.bottom;
		m_rcFinal.bottom = m_rcFinal.top + rcheight(rcTooltip);
	}

	TCHAR rctext[80];
	vaDbgTs(_T("In ShowContentTooltip, [%s]tooltip-rect: %s"), 
		dbg_isUp ? _T("UP") : _T("DN"),
		RECTtext(m_rcFinal, rctext));

	// We must force toggle TTM_TRACKACTIVATE so that tooltip refreshes its display position
	// according to our new m_rcFinal.
	SendMessage(m_hwndttContent, TTM_TRACKACTIVATE, FALSE, (LPARAM)&ti);
	SendMessage(m_hwndttContent, TTM_TRACKACTIVATE, TRUE, (LPARAM)&ti);
	// -- TTM_TRACKACTIVATE(TRUE) this will internally call TTM_NEEDTEXT


	return Dlgtte_Succ;
}


LRESULT 
CHottoolSubsi::in_WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool *pMsgDone)
{
	assert(m_hwnd==hwnd);

	*pMsgDone = false;

	if (!m_hwndttContent)
	{
		// User has not assigned a hottool for content-tooltip, so do nothing,
		// just call next chain-node.
		return 0;
	}

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
	else if (uMsg == s_MSG_GetHottoolSubsi)
	{
		*pMsgDone = true;
		return (LRESULT)this;
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////////


CxxWindowSubclass::ReCode_et 
CTooltipMan::AddUic(HWND hwndUic, const GetTextCallbacks_st &gtcb)
{
	if (s_MSG_GetHottoolSubsi == 0)
	{
		s_MSG_GetHottoolSubsi = RegisterWindowMessage(_T("Dlgtte-GetHottoolSubsi"));
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
		}

		// Associate Uic to the tooltip.

		TOOLINFO ti = { sizeof(TOOLINFO) };
		ti.uFlags = TTF_IDISHWND | TTF_SUBCLASS;
		ti.hwnd = hdlg;
		ti.uId = (UINT_PTR)hwndUic;
		ti.lpszText = LPSTR_TEXTCALLBACK;

		BOOL succ = do_TTM_ADDTOOL_nodup(m_httUsage, ti);
		assert(succ);

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

		// Enable multiline tooltip
		SendMessage(m_httContent, TTM_SETMAXTIPWIDTH, 0, 800);

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
//		dbg_WM_NOTIFY(wParam, lParam);

		NMHDR *pnmh = (NMHDR *)lParam;
		HWND hwndTooltip = pnmh->hwndFrom;
		HWND hwndUic = (HWND)pnmh->idFrom;

		if (pnmh->code == TTN_NEEDTEXT)
		{
			CHottoolSubsi *phot = 
				CxxWindowSubclass::FetchCxxobjFromHwnd<CHottoolSubsi>(
					hwndUic, sig_EasyHottool, FALSE);

			vaDBG(_T("TTN_NEEDTEXT from hwndTooltip=0x%X for Uic=0x%X."), hwndTooltip, hwndUic);

			if(phot)
			{
				const TCHAR *ptext = nullptr;

				if (hwndTooltip == m_httUsage)
					ptext = phot->Call_getUsageText(hwndUic);
				else if (hwndTooltip == m_httContent)
					ptext = phot->Call_getContentText(hwndUic);

				NMTTDISPINFO *pdi = (NMTTDISPINFO *)pnmh;
				pdi->lpszText = (LPTSTR)ptext;
			}
		}
		else if (pnmh->code == TTN_SHOW)
		{
			if (hwndTooltip == m_httContent)
			{
				RECT rc; TCHAR rctext[80];
				GetWindowRect(hwndTooltip, &rc);
				vaDBG(_T("TTN_SHOW from 0x%X,  init-Rect=%s"), hwndTooltip, RECTtext(rc, rctext));

				CHottoolSubsi *phot = (CHottoolSubsi*)SendMessage(hwndUic, s_MSG_GetHottoolSubsi, 0, 0);
				rc = phot->m_rcFinal;

				vaDBG(_T("TTN_SHOW from 0x%X, final-Rect=%s"), hwndTooltip, RECTtext(rc, rctext));

				if (rc.left != -1)
				{
					SetWindowPos(hwndTooltip, 0, rc.left, rc.top, 0, 0, SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOSIZE);
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

	if (err_th)
		return err_th;

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
			m_httUsage = NULL;
		}

		if (m_httContent) {
			DestroyWindow(m_httContent);
			m_httContent = NULL;
		}

		this->DetachHwnd(true);
	}

	return E_Success;
}


/*
CHottoolSubsi* CTooltipMan::FindHottoolByHwnd(HWND hwndUic)
{
	// Only for content-tooltip 
	
	HWND hwndTooltip = m_httContent;

	int iTool = 0;
	while(1)
	{
		TOOLINFO ti = { sizeof(TOOLINFO) };
		BOOL any = (BOOL)SendMessage(hwndTooltip, TTM_ENUMTOOLS, iTool, (LPARAM)&ti);
		
		if (!any)
			return nullptr;

		if ((HWND)ti.uId == hwndUic)
		{
			CHottoolSubsi *phot = (CHottoolSubsi*)SendMessage(hwndUic, s_MSG_GetHottoolSubsi, 0, 0);
			xxx;
		}
	}
}
*/


} // (internal) namespace Dlgtte


using namespace Dlgtte;


Dlgtte_err Dlgtte_EnableTooltip(HWND hwndCtl,
	PROC_DlgtteGetText *getUsageText, void *uctxUsage,
	PROC_DlgtteGetText *getContentText, void *uctxContent, 
	Dlgtte_BitFlags_et flags)
{
	HWND hdlg = GetParent(hwndCtl);

	CxxWindowSubclass::ReCode_et err = CxxWindowSubclass::E_Fail;
	CTooltipMan *ptm = CxxWindowSubclass::FetchCxxobjFromHwnd<CTooltipMan>(
		hdlg, sig_EasyTooltipMan, TRUE, &err);

	assert(!err);

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

	CxxWindowSubclass::ReCode_et err_tm = CxxWindowSubclass::E_Fail;
	CTooltipMan *ptm = CxxWindowSubclass::FetchCxxobjFromHwnd<CTooltipMan>(
		hdlg, sig_EasyTooltipMan, FALSE, &err_tm);

	if(!ptm)
		return;

	ptm->GetTooltipHwnd(pUsageTooltip, pContentTooltip);
}

Dlgtte_err Dlgtte_GetFlags(HWND hwndCtl, Dlgtte_BitFlags_et *pFlags)
{
	if (!IsWindow(hwndCtl))
		return Dlgtte_InvalidHwnd;

	CHottoolSubsi *phot = (CHottoolSubsi*)SendMessage(hwndCtl, s_MSG_GetHottoolSubsi, 0, 0);
	if (!phot)
		return Dlgtte_InvalidHottool;

	*pFlags = phot->GetFlags();
	
	return Dlgtte_Succ;
}

Dlgtte_err Dlgtte_SetFlags(HWND hwndCtl, Dlgtte_BitFlags_et flags)
{
	if(!IsWindow(hwndCtl))
		return Dlgtte_InvalidHwnd;

	CHottoolSubsi *phot = (CHottoolSubsi*)SendMessage(hwndCtl, s_MSG_GetHottoolSubsi, 0, 0);
	if(!phot)
		return Dlgtte_InvalidHottool;

	phot->SetFlags(flags);

	return Dlgtte_Succ;
}


Dlgtte_err Dlgtte_ShowContentTooltip(HWND hwndCtl, bool is_show)
{
	if (!IsWindow(hwndCtl))
		return Dlgtte_InvalidHwnd;

	CHottoolSubsi *phot = (CHottoolSubsi*)SendMessage(hwndCtl, s_MSG_GetHottoolSubsi, 0, 0);
	if (!phot)
		return Dlgtte_InvalidHottool;

	return phot->ShowContentTooltip(is_show);
}



#endif // DlgTooltipEasy_IMPL

#endif
