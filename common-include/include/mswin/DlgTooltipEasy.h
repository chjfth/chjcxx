#ifndef __DlgTooltipEasy_h_20250605_
#define __DlgTooltipEasy_h_20250605_

#include <tchar.h>
#include <windows.h>

enum Dlgtte_err
{
	Dlgtte_Succ = 0,
	Dlgtte_Fail = -1,

};

typedef const TCHAR* PROC_DlgtteGetText(void *userctx);

Dlgtte_err Dlgtte_EnableTooltip(HWND hwndCtl, 
	PROC_DlgtteGetText *getUsageText, void *uctxUsage,
	PROC_DlgtteGetText *getContentText, void *uctxContent);


///////////////////////////////////////////////////////////////
// Implementation Below:
///////////////////////////////////////////////////////////////

#ifdef DlgTooltipEasy_IMPL


#include <assert.h>
#include <windowsx.h>
#include <DlOpe.h>

namespace Dlgtte // (internal) 
{

struct SToolNode
{
	SToolNode *phead, *pprev, *pnext;

	PROC_DlgtteGetText *getText;
	void *uctx;

	SToolNode();
};

typedef DlOpeH<SToolNode, &SToolNode::phead, &SToolNode::pprev, &SToolNode::pnext> DLOPE_SToolNode;

inline SToolNode::SToolNode()
{
	DLOPE_SToolNode::InitNode(this);
	getText = nullptr; uctx = nullptr;
}

struct SToolHead
{
	SToolNode *pHead, *pPrev, *pNext;

	SToolHead()
	{
		DLOPE_SToolNode::InitHead(pHead, pPrev, pNext);
	}
};

static const TCHAR *sig_EasyHottool = _T("sig_EasyHottool");
static const TCHAR *sig_EasyTooltipMan = _T("sig_EasyTooltipMan");

struct GetTextCallbacks_st
{
	PROC_DlgtteGetText *getUsageText;
	void *uctxUsage;

	PROC_DlgtteGetText *getContentText;
	void *uctxContent;
};


class CxxSubclassHottool : public CxxWindowSubclass
{
public:
	CxxSubclassHottool();

	virtual LRESULT WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	
	ReCode_et xxxAttachHwnd(HWND hwnd/*HWND hwndHottool, HWND hwndTooltip*/);

	void AssignCallback(const GetTextCallbacks_st &gtcb)
	{
		m_getUsageText = gtcb.getUsageText;
		m_uctxUsage = gtcb.uctxUsage;
		m_getContentText = gtcb.getContentText;
		m_uctxContent = gtcb.uctxContent;
	}

	const TCHAR *Call_getUsageText()
	{
		if (m_getUsageText)
		{
			return m_getUsageText(m_uctxUsage);
		}
		else
			return nullptr;
	}

//	friend class CTooltipMan;

private:
	HWND m_hwndTooltip;

	PROC_DlgtteGetText *m_getUsageText;
	void *m_uctxUsage;
	
	PROC_DlgtteGetText *m_getContentText;
	void *m_uctxContent;
};

CxxSubclassHottool::CxxSubclassHottool()
{
	m_hwndTooltip = NULL;

	m_getUsageText = m_getContentText = nullptr;
	m_uctxUsage = m_uctxContent = nullptr;
}

CxxSubclassHottool::ReCode_et
CxxSubclassHottool::xxxAttachHwnd(HWND hwnd)
{
	ReCode_et err = __super::AttachHwnd(hwnd, sig_EasyHottool);
	return err;
}


LRESULT 
CxxSubclassHottool::WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{


	return DefSubclassProc(hwnd, uMsg, wParam, lParam);
}

//////////////////////////////////////////////////////////////////////////


class CTooltipMan : public CxxWindowSubclass
{
public:
	CTooltipMan()
	{
		m_httUsage = NULL;
		m_httContent = NULL;
	}

	ReCode_et AddUic(HWND hwndUic, const GetTextCallbacks_st &gtcb);
	// -- todo: a second call will overwrite previous gtcb
	
	ReCode_et DelUic(HWND hwndUic);

	virtual LRESULT WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
	HWND m_httUsage;  // tooltip-window HWND
	HWND m_httContent;

	SToolHead m_headToolUsage;
	SToolHead m_headToolContent;
};



CxxWindowSubclass::ReCode_et 
CTooltipMan::AddUic(HWND hwndUic, const GetTextCallbacks_st &gtcb)
{
	HWND hdlg = m_hwnd;

	if (gtcb.getUsageText == nullptr && gtcb.getContentText == nullptr)
		return E_BadParam;

	if (gtcb.getUsageText)
	{
		if (!m_httUsage)
		{
			m_httUsage = CreateWindowEx(
				WS_EX_TOPMOST | WS_EX_TRANSPARENT,
				TOOLTIPS_CLASS,
				NULL, // window title
				TTS_NOPREFIX | TTS_ALWAYSTIP, // not TTS_BALLOON()
				CW_USEDEFAULT, CW_USEDEFAULT,
				CW_USEDEFAULT, CW_USEDEFAULT,
				hdlg, // tooltip-window's owner
				NULL, NULL, NULL);
			assert(m_httUsage);
		}

		// Associate Uic to the tooltip.

		TOOLINFO ti = { sizeof(TOOLINFO) };
		ti.hwnd = hdlg;
		ti.uId = (UINT_PTR)hwndUic;
		ti.uFlags = TTF_IDISHWND | TTF_SUBCLASS;
		ti.lpszText = LPSTR_TEXTCALLBACK;

		LRESULT lsucc = SendMessage(m_httUsage, TTM_ADDTOOL, 0, (LPARAM)&ti);
		assert(lsucc);

		// Make the tooltip appear quickly (100ms), instead of default delaying 500ms.
		SendMessage(m_httUsage, TTM_SETDELAYTIME, TTDT_INITIAL, 100);
	}

	CxxWindowSubclass::ReCode_et err = CxxWindowSubclass::E_Fail;
	CxxSubclassHottool *psub = 
		CxxWindowSubclass::FetchCxxobjFromHwnd<CxxSubclassHottool>(
			hwndUic, sig_EasyHottool, true, &err);
	assert(psub);

	if (gtcb.getContentText)
	{

	}

	psub->AssignCallback(gtcb);

	return E_Success;
}

LRESULT 
CTooltipMan::WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_NOTIFY)
	{
//		dbg_WM_NOTIFY(wParam, lParam);

		NMHDR *pnmh = (NMHDR *)lParam;
		HWND hwndUic = (HWND)pnmh->idFrom;

		if (pnmh->code == TTN_NEEDTEXT)
		{
			CxxSubclassHottool *phot = 
				CxxWindowSubclass::FetchCxxobjFromHwnd<CxxSubclassHottool>(
					hwndUic, sig_EasyHottool, FALSE);

			if(phot)
			{
				const TCHAR *ptext = phot->Call_getUsageText();

				NMTTDISPINFO *pdi = (NMTTDISPINFO *)pnmh;
				pdi->lpszText = (LPTSTR)ptext;
			}
		}
	}

	return DefSubclassProc(hwnd, uMsg, wParam, lParam);
}


static bool HasAnyHottool(HWND hwndTooltip)
{
	TOOLINFO ti = { sizeof(TOOLINFO) };
	BOOL any = (BOOL)SendMessage(hwndTooltip, TTM_ENUMTOOLS, 0, (LPARAM)&ti);
	return any ? true : false;
}

CxxWindowSubclass::ReCode_et
CTooltipMan::DelUic(HWND hwndUic)
{
	ReCode_et err_th = CxxWindowSubclass::E_Fail;
	CxxSubclassHottool *pth = CxxWindowSubclass::FetchCxxobjFromHwnd<CxxSubclassHottool>(
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
		this->DetachHwnd();
	}

	return E_Success;
}


} // (internal) namespace Dlgtte


using namespace Dlgtte;


Dlgtte_err Dlgtte_EnableTooltip(HWND hwndCtl,
	PROC_DlgtteGetText *getUsageText, void *uctxUsage,
	PROC_DlgtteGetText *getContentText, void *uctxContent)
{
	HWND hdlg = GetParent(hwndCtl);

	CxxWindowSubclass::ReCode_et err = CxxWindowSubclass::E_Fail;
	CTooltipMan *ptm = CxxWindowSubclass::FetchCxxobjFromHwnd<CTooltipMan>(
		hdlg, sig_EasyTooltipMan, TRUE, &err);

	assert(!err);

	GetTextCallbacks_st gtcb = { getUsageText, uctxUsage, getContentText, uctxContent };

	err = ptm->AddUic(hwndCtl, gtcb);

	return err ? Dlgtte_Fail : Dlgtte_Succ;
}


bool Dlgtte_IsEnabled(HWND hwndCtl)
{
	// Check whether hwndCtl has been installed a DlgTooltipEasy facility.

	CxxSubclassHottool *pth = CxxWindowSubclass::FetchCxxobjFromHwnd<CxxSubclassHottool>(
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
	CxxSubclassHottool *pth = CxxWindowSubclass::FetchCxxobjFromHwnd<CxxSubclassHottool>(
		hwndCtl, sig_EasyHottool, FALSE, &err_th);

	if (err_tm == CxxWindowSubclass::E_NotExist)
	{
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
