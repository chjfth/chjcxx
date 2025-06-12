#ifndef __TooltipHelper_h_20250612_
#define __TooltipHelper_h_20250612_

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

inline BOOL do_TTM_ADDTOOL_nodup(HWND htt, const TOOLINFO &ti, bool *pExisted=nullptr)
{
	// TTM_ADDTOOL no-duplicate .
	// If the hottool has existed, we just call TTM_SETTOOLINFO.

	SETTLE_OUTPUT_PTR(bool, pExisted, false);

	TOOLINFO tiOld = {sizeof(TOOLINFO)};
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



#endif
