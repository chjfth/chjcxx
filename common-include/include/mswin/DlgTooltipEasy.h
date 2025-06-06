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

namespace Dlgtte
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


class CxxSubclassHottool : public CxxWindowSubclass
{
public:
	CxxSubclassHottool();
//	static const TCHAR *sig = _T("");

	virtual LRESULT WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	
	ReCode_et xxxAttachHwnd(HWND hwnd/*HWND hwndHottool, HWND hwndTooltip*/);

	void AssignCallback(PROC_DlgtteGetText *getUsageText, void *uctxUsage,
		PROC_DlgtteGetText *getContentText, void *uctxContent)
	{
		m_getUsageText = getUsageText;
		m_uctxUsage = uctxUsage;
		m_getContentText = getContentText;
		m_uctxContent = uctxContent;
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
	ReCode_et err = __super::AttachHwnd(hwnd, _T("EasyHottool"));
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

	ReCode_et PrepareEasyTooltip(HWND hdlg, HWND hwndCtl,
		PROC_DlgtteGetText *getUsageText, void *uctxUsage,
		PROC_DlgtteGetText *getContentText, void *uctxContent);


	virtual LRESULT WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
	HWND m_httUsage;  // tooltip-window HWND
	HWND m_httContent;

	SToolHead m_headToolUsage;
	SToolHead m_headToolContent;
};



CxxWindowSubclass::ReCode_et 
CTooltipMan::PrepareEasyTooltip(HWND hdlg, HWND hwndCtl,
	PROC_DlgtteGetText *getUsageText, void *uctxUsage,
	PROC_DlgtteGetText *getContentText, void *uctxContent)
{
	if (getUsageText)
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
		ti.uId = (UINT_PTR)hwndCtl;
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
			hwndCtl, _T("EasyHottool"), true, &err);
	assert(psub);

	psub->AssignCallback(getUsageText, uctxUsage, getContentText, uctxContent);

	if (getContentText)
	{

	}

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
					hwndUic, _T("EasyHottool"), FALSE);

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


} // namespace Dlgtte

using namespace Dlgtte;

Dlgtte_err Dlgtte_EnableTooltip(HWND hwndCtl,
	PROC_DlgtteGetText *getUsageText, void *uctxUsage,
	PROC_DlgtteGetText *getContentText, void *uctxContent)
{
	HWND hdlg = GetParent(hwndCtl);

	//	CTooltipMan *ptm = HwndGetPropAsCxxClass<CTooltipMan>(hdlg, _T("DlgTooltipEasy"));

	CxxWindowSubclass::ReCode_et err = CxxWindowSubclass::E_Fail;
	CTooltipMan *ptm = CxxWindowSubclass::FetchCxxobjFromHwnd<CTooltipMan>(
		hdlg, _T("EasyTooltipMan"), TRUE, &err);

	assert(!err);

	err = ptm->PrepareEasyTooltip(
		hdlg, hwndCtl, getUsageText, uctxUsage, getContentText, uctxContent);

	return err ? Dlgtte_Fail : Dlgtte_Succ;
}


#endif // DlgTooltipEasy_IMPL

#endif
