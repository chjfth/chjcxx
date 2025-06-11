#ifndef __DlgTooltipEasy_h_20250605_
#define __DlgTooltipEasy_h_20250605_

#include <tchar.h>
#include <windows.h>

enum Dlgtte_err
{
	Dlgtte_Succ = 0,
	Dlgtte_Fail = -1,

};

enum Dlgtte_BitFlags_et
{
	Dlgtte_Default0 = 0,

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
	Dlgtte_BitFlags_et flags = Dlgtte_Default0);

Dlgtte_err Dlgtte_RemoveTooltip(HWND hwndCtl);


///////////////////////////////////////////////////////////////
// Implementation Below:
///////////////////////////////////////////////////////////////

#ifdef DlgTooltipEasy_IMPL


#include <assert.h>
#include <windowsx.h>

#define WinMultiMon_IMPL
#include <mswin/WinMultiMon.h>

namespace Dlgtte // (internal) 
{

static const TCHAR *sig_EasyHottool = _T("sig_EasyHottool");
static const TCHAR *sig_EasyTooltipMan = _T("sig_EasyTooltipMan");

static UINT s_msgQUERY_FINAL_POS = 0;


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

	m_flags = Dlgtte_Default0;

	SetRect(&m_rcFinal, -1, -1, -1, -1);
}

class CTooltipMan : public CxxWindowSubclass
{
public:
	CTooltipMan()
	{
		m_httUsage = NULL;
		m_httContent = NULL;

		m_prevHottoolFocus = NULL;
	}

	ReCode_et AddUic(HWND hwndUic, const GetTextCallbacks_st &gtcb);
	// -- note: a second call will overwrite previous gtcb

	ReCode_et DelUic(HWND hwndUic);

	virtual LRESULT WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	HWND NotifyHottoolFocus(HWND nowfocus)
	{
		HWND oldfocus = m_prevHottoolFocus;
		m_prevHottoolFocus = nowfocus;
		return oldfocus;
	}

private:
	// Two tooltip-window HWND-s
	HWND m_httUsage;
	HWND m_httContent;

	HWND m_prevHottoolFocus;
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

LRESULT 
CHottoolSubsi::in_WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool *pMsgDone)
{
	*pMsgDone = false;

	if (!m_hwndttContent)
	{
		// User has not assigned a hottool for content-tooltip, so do nothing,
		// just call next chain-node.
		return 0;
	}

	HWND hUic = hwnd;
	HWND hdlg = GetParent(hwnd);

	TOOLINFO ti = { sizeof(TOOLINFO) };
	ti.hwnd = hdlg;
	ti.uId = (UINT_PTR)hUic;

	if (uMsg == WM_SETFOCUS)
	{
		if ((m_flags & Dlgtte_AutoContentTipOnFocus) == 0)
			return 0;

		HWND hwndPrevFocus = m_pTooltipMan->NotifyHottoolFocus(hUic);
		vaDbgTs(_T("Hottool WM_SETFOCUS: hUic=0x%X, hwndPrevFocus=0x%X"), PtrToUint(hUic), PtrToUint(hwndPrevFocus));
		if (hwndPrevFocus == hUic)
		{
			// Purpose: If user click tooltip-window itself to close the tooltip, the focus goes back
			// to the hottool. So `hwndPrevFocus==hUic` detects this case and the tooltip will not
			// show up repeatedly.
			return 0;
		}

		vaDBG(_T("Hottool(0x%X) Will show Content tooltip."), PtrToUint(hUic));

		HWND hwndTooltip = m_hwndttContent;

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
		vaDbgTs(_T("In WM_SETFOCUS, [%s]tooltip-rect: %s"), 
			dbg_isUp ? _T("UP") : _T("DN"),
			RECTtext(m_rcFinal, rctext));

		// We must force toggle TTM_TRACKACTIVATE so that tooltip refreshes its display position
		// according to our new m_rcFinal.
		SendMessage(m_hwndttContent, TTM_TRACKACTIVATE, FALSE, (LPARAM)&ti);
		SendMessage(m_hwndttContent, TTM_TRACKACTIVATE, TRUE, (LPARAM)&ti);
		// -- TTM_TRACKACTIVATE(TRUE) this will internally call TTM_NEEDTEXT

	}
	else if (uMsg == WM_KILLFOCUS)
	{
		vaDBG(_T("Hottool(0x%X) Will hide Content tooltip."), hUic);

		SendMessage(m_hwndttContent, TTM_TRACKACTIVATE, FALSE, (LPARAM)&ti);
	}
	else if (uMsg == s_msgQUERY_FINAL_POS)
	{
		assert(lParam != 0);

		*(RECT*)lParam = m_rcFinal;

		*pMsgDone = true;
	}

	return 0;
}

//////////////////////////////////////////////////////////////////////////


CxxWindowSubclass::ReCode_et 
CTooltipMan::AddUic(HWND hwndUic, const GetTextCallbacks_st &gtcb)
{
	if (s_msgQUERY_FINAL_POS == 0)
	{
		s_msgQUERY_FINAL_POS = RegisterWindowMessage(_T("Dlgtte-QUERY_FINAL_POS"));
	}

	HWND hdlg = m_hwnd;

	if (gtcb.getUsageText == nullptr && gtcb.getContentText == nullptr)
		return E_BadParam;

	CxxWindowSubclass::ReCode_et err = CxxWindowSubclass::E_Fail;
	CHottoolSubsi *phottool =
		CxxWindowSubclass::FetchCxxobjFromHwnd<CHottoolSubsi>(
			hwndUic, sig_EasyHottool, true, &err);
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

		LRESULT lsucc = SendMessage(m_httUsage, TTM_ADDTOOL, 0, (LPARAM)&ti);
		assert(lsucc);

		// Make the tooltip appear quickly (100ms), instead of default delaying 500ms.
		SendMessage(m_httUsage, TTM_SETDELAYTIME, TTDT_INITIAL, 100);
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

		LRESULT lsucc = SendMessage(m_httContent, TTM_ADDTOOL, 0, (LPARAM)&ti);
		assert(lsucc);

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

				SendMessage(hwndUic, s_msgQUERY_FINAL_POS, 0, (LPARAM)&rc);

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

	ReCode_et err = pth->DetachHwnd();
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

		this->DetachHwnd();
	}

	return E_Success;
}


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



#endif // DlgTooltipEasy_IMPL

#endif
