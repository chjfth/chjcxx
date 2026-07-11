#ifndef __CHHI__WinMultiMon_h_
#define __CHHI__WinMultiMon_h_
#define __CHHI__WinMultiMon_h_created_ 20260227
#define __CHHI__WinMultiMon_h_updated_ 20260711

#include <windows.h>
#include <RECTxy.h>

#ifndef WinErr_t_DEFINED
#define WinErr_t_DEFINED
typedef DWORD WinErr_t;
#endif

struct OneMonitorInfo_st
{
	bool isPrimary;
	RECT rcMonitor;
	RECT rcWorkArea;
	TCHAR szDevice[CCHDEVICENAME]; // '\\.\DISPLAY1', '\\.\DISPLAY2' etc
	HMONITOR hMonitor;
	HDC hdcMonitor;
};

WinErr_t doEnumDisplayMonitors(OneMonitorInfo_st arMonInfo[], int nEles, int *pTotalMonitors);
// -- Return ERROR_MORE_DATA if arEles is too small(smaller than output *pTotalMonitors).

bool getMonitorRectByPoint(int screen_x, int screen_y, RECT *pMonitorRect);
// -- Given a Point(X,Y), find out which monitor contains that point.
//    Return true if pt is in one monitor, else false.
//    *pMonitorRect outputs the screen-coord of the containing monitor.

Rect_st mumo_ReposRect(const Rect_st& urect,
	int nMonitors, const Rect_st arMonitorRect[],
	bool allow_straddle, bool allow_shrink=false);

RECT mumo_PlaceRectInsideScreen(const RECT& urect, bool allow_straddle, bool allow_shrink=false);


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
#if defined(WinMultiMon_IMPL) || (defined CHHI_ALL_IMPL && !defined CHHI_ALL_IMPL_HIDE_WinMultiMon) // [IMPL]

// >>> Include headers required by this lib's implementation
#include <assert.h>
#include <commdefs.h>
#include <RECTxy.h>
#include <mswin/win32cozy.h> // RECTtext() macro
#include <EnsureClnup_mswin.h>

// <<< Include headers required by this lib's implementation


#ifndef WinMultiMon_DEBUG
#include <CHHI_vaDBG_hide.h>
#endif

/*
    ____       _             __                                                                
   / __ \_____(_)   ______ _/ /____       ____  ____ _____ ___  ___  _________  ____ _________ 
  / /_/ / ___/ / | / / __ `/ __/ _ \     / __ \/ __ `/ __ `__ \/ _ \/ ___/ __ \/ __ `/ ___/ _ \
 / ____/ /  / /| |/ / /_/ / /_/  __/    / / / / /_/ / / / / / /  __(__  ) /_/ / /_/ / /__/  __/
/_/   /_/  /_/ |___/\__,_/\__/\___/    /_/ /_/\__,_/_/ /_/ /_/\___/____/ .___/\__,_/\___/\___/ 
                                                                      /_/                      
*/
// We should enclose this lib's implementation into private namespace.

////////////////////////////////////////////////////////////////////////////
namespace WinMultiMon {
////////////////////////////////////////////////////////////////////////////
// (private namespace 'WinMultiMon') 



struct EnumMonitorPrivate_st
{
	OneMonitorInfo_st *arUserMoninfo;
	int nUserMoninfo;

	int nUserFilled; // How many eles in arUserMoninfo[] have been filled

	int nMonitorCount; // total monitor count
};

static BOOL CALLBACK 
MonitorEnumProc(
	HMONITOR hMonitor,  // handle to display monitor
	HDC hdcMonitor,     // handle to monitor DC
	LPRECT lprcMonitor, // monitor intersection rectangle
	LPARAM dwData       // user context
	)
{
	EnumMonitorPrivate_st &emp = *(EnumMonitorPrivate_st*)dwData;
	emp.nMonitorCount++;

	if( emp.nUserFilled < emp.nUserMoninfo )
	{
		OneMonitorInfo_st &oneinfo = emp.arUserMoninfo[emp.nUserFilled];
		oneinfo.hMonitor = hMonitor;
		oneinfo.hdcMonitor = hdcMonitor;

		MONITORINFOEX minfo; // Damn M$: cannot just write = {sizeof(MONITORINFOEX)}
		minfo.cbSize = sizeof(MONITORINFOEX);
		BOOL b = GetMonitorInfo(hMonitor, &minfo);
		if (!b)
		{ 
			DWORD winerr = GetLastError();
			vaDBG0(_T("GetMonitorInfo() fail on hMonitor=0x%p, winerr=%d."), hMonitor, winerr);

			return FALSE;
		}

		if(! EqualRect(lprcMonitor, &minfo.rcMonitor))
		{
			RECT &r1 = *lprcMonitor;
			RECT &r2 = minfo.rcMonitor;
			vaDBG0(
				_T("Panic! In MonitorEnumProc(): Callback lprcMonitor does NOT equal to GetMonitorInfo().\n")
				_T("    L/T/R/B [%d,%d,%d,%d] vs [%d,%d,%d,%d]")
				, 
				r1.left, r1.top, r1.right, r1.bottom,  r2.left, r2.top, r2.right, r2.bottom
				);
		}

		oneinfo.isPrimary = (minfo.dwFlags&MONITORINFOF_PRIMARY) ? true : false;
		oneinfo.rcMonitor = minfo.rcMonitor;
		oneinfo.rcWorkArea = minfo.rcWork;
		_tcscpy_s(oneinfo.szDevice, minfo.szDevice);

		emp.nUserFilled++;

		TCHAR rctext[40] = _T("");
		vaDBG2(_T("MonitorEnumProc#%d: hMonitor=0x%p, hdcMonitor=0x%p, \"%s\" %s"),
			emp.nMonitorCount,
			hMonitor, hdcMonitor, 
			minfo.szDevice, // \"%s\"
			RECTtext(minfo.rcMonitor, rctext) // %s
			);
	}
	else
	{
		vaDBG2(_T("MonitorEnumProc#%d: less buffer, not calling GetMonitorInfo()"), emp.nMonitorCount);
	}
	
	return TRUE; // continue enumeration 
}

WinErr_t 
_doEnumDisplayMonitors(OneMonitorInfo_st arMonInfo[], int nEles, int *pTotalMonitors)
{
	SETTLE_OUTPUT_PTR(int, pTotalMonitors, 0);

	SetLastError(0);

	EnumMonitorPrivate_st emp = {0};
	emp.arUserMoninfo = arMonInfo;
	emp.nUserMoninfo = nEles;
	emp.nUserFilled = 0;
	emp.nMonitorCount = 0;

	BOOL succ = EnumDisplayMonitors(NULL, NULL, MonitorEnumProc, (LPARAM)&emp);
	if(succ)
	{
		*pTotalMonitors = emp.nMonitorCount;

		if(emp.nUserFilled==emp.nMonitorCount)
			return ERROR_SUCCESS;
		else
			return ERROR_MORE_DATA;
	}
	else
	{
		WinErr_t winerr = GetLastError();
		if(winerr)
			return winerr; // a real error
		else
			return ERROR_INVALID_FUNCTION;
	}
}


static BOOL CALLBACK OneMonitorEnumProc(
	HMONITOR hMonitor,  // handle to display monitor
	HDC hdcMonitor,     // handle to monitor DC
	LPRECT lprcMonitor, // monitor intersection rectangle
	LPARAM dwData       // user context
	)
{
	MONITORINFO minfo = {sizeof(MONITORINFO)};
	BOOL b = GetMonitorInfo(hMonitor, &minfo);
	if(!b)
		return FALSE;

	RECT &rcOut = *(RECT*)dwData;
	rcOut = minfo.rcMonitor;

	return TRUE; // if FALSE, EnumDisplayMonitors() returns FALSE
}


bool _getMonitorRectByPoint(int screen_x, int screen_y, RECT *pMonitorRect)
{
	SetRect(pMonitorRect, 0,0,0,0);

	RECT rcInput = { screen_x, screen_y, screen_x+1, screen_y+1 }; // one pixel rect

	BOOL succ = EnumDisplayMonitors(NULL, &rcInput, OneMonitorEnumProc, (LPARAM)pMonitorRect);
	if(!succ)
		return false;

	if(pMonitorRect->left==0 && pMonitorRect->right==0)
		return false;

	return true;
}


Rect_st _mumo_ReposRect(const Rect_st& urect,
	int nMonitors, const Rect_st arMonitorRect[],
	bool allow_straddle, bool allow_shrink)
{
	struct PerMonOp_st
	{
		Rect_st ovlprect;    // intersect x,y area of urect to this Monitor
		Pace_st pace_to_fix; // what's the pace value to place urect into this Monitor
	};
	PerMonOp_st *arPerMonOp = new PerMonOp_st[nMonitors];
	CEC_raw_delete cec_op = arPerMonOp;

	const RECT &uRECT = *(const RECT*)&urect;
	vaDBG3(_T("_mumo_ReposRect() input urect %s:"), RECTtext(uRECT).c_str());

	int idxMaxOvlpMonitor = 0;
	int areaMaxOvlpPermon = 0;
	int areaTotalOvlp = 0;
	
	int idxMinPaceMonitor = 0;
	int minAbsPaceTotal = 0x7FFFffff;

	for (int i = 0; i < nMonitors; i++)
	{
		const Rect_st& rcMonitor = arMonitorRect[i];
		PerMonOp_st &op = arPerMonOp[i];

		getRectOverlap(urect, rcMonitor, &op.ovlprect);
		op.pace_to_fix = urect.PaceToRect(rcMonitor);
		int absPaceTotal = abs(op.pace_to_fix.x) + abs(op.pace_to_fix.y);

		int areaOvlp = RECTarea(op.ovlprect);
		areaTotalOvlp += areaOvlp;

		vaDBG3(_T(".   Monitor#%-2d  : %s"), i, rcMonitor.text().c_str());
		vaDBG3(_T(".     overlapped: %s , area=%d"), RECTtext(op.ovlprect).c_str(), areaOvlp);
		vaDBG3(_T(".     pace_to_fix: x=%d , y=%d (abstot=%d)"), op.pace_to_fix.x, op.pace_to_fix.y, absPaceTotal);

		if (op.pace_to_fix.x==0 && op.pace_to_fix.y==0)
		{
			vaDBG3(_T(".   uRECT already in monitor#%d, done."), i);
			return uRECT;
		}

		if (areaOvlp > areaMaxOvlpPermon)
		{
			areaMaxOvlpPermon = areaOvlp;
			idxMaxOvlpMonitor = i;
		}

		if (absPaceTotal < minAbsPaceTotal)
		{
			minAbsPaceTotal = absPaceTotal;
			idxMinPaceMonitor = i;
		}
	}

	vaDBG3(_T(".   Monitor#%d has max overlapping area(%d)"), idxMaxOvlpMonitor, areaMaxOvlpPermon);
	vaDBG3(_T(".   Monitor#%d has min x+y absolute pacing distance(%d)"), idxMinPaceMonitor, minAbsPaceTotal);

	if (areaTotalOvlp == RECTarea(uRECT))
	{
		vaDBG3(_T(".   uRECT straddles multiple monitors, and fully visible."));

		if (allow_straddle)
			return urect;
	}

	// Now we have to adjust uRECT position to make it go inside idxMaxOvlpMonitor.
	// Note: If areaTotalOvlp>0, urect is outside all monitors, then we should follow minAbsPaceTotal.
	
	int idxSnapToMonitor = areaTotalOvlp>0 ? idxMaxOvlpMonitor : idxMinPaceMonitor;

	int xfix = arPerMonOp[idxSnapToMonitor].pace_to_fix.x;
	int yfix = arPerMonOp[idxSnapToMonitor].pace_to_fix.y;
	assert(xfix!=0 || yfix!=0);

	Rect_st outrect = urect;
	OffsetRect((RECT*)&outrect, xfix, yfix);

	vaDBG3(_T(".   Need to apply pace fix (%d,%d) to uRECT, result: %s"), xfix, yfix, RECTtext(outrect).c_str());

	if (allow_shrink )
	{
		int cxMon = RECTcx(arMonitorRect[idxSnapToMonitor]);
		int cyMon = RECTcy(arMonitorRect[idxSnapToMonitor]);

		int cxShrink = RECTcx(outrect)>cxMon ? RECTcx(outrect)-cxMon : 0;
		int cyShrink = RECTcy(outrect)>cyMon ? RECTcy(outrect)-cyMon : 0;

		if(cxShrink>0 || cyShrink>0)
		{
			outrect.right -= cxShrink;
			outrect.bottom -= cyShrink;
			vaDBG3(_T(".   Shrink result by (%d,%d), final result: %s"), cxShrink, cyShrink, RECTtext(outrect).c_str());
		}
	}

	return outrect;
}

MakeDelega_CleanupCxxPtr(OneMonitorInfo_st)

RECT _mumo_PlaceRectInsideScreen(const RECT& uRECT, bool allow_straddle, bool allow_shrink)
{
	// Check if urect is inside all-monitors' area.
	// If not(some portion of urect is invisible), move the urect(as return value)
	// into the nearest monitor in hope that urect is fully visible.
	//
	// Use case of this function: 
	//   A program records last-seen window position and restore that position
	//   on next-time program starts up, but the monitor for that old position
	//   has been unplugged, causing the window to be outside of all monitors.
	// By running this function, the program code can easily re-place the window
	// to most appropriate new position.

	const Rect_st& urect = *(const Rect_st*)&uRECT;

	assert(RECTcx(urect)>0);
	assert(RECTcy(urect)>0);

	int nMonitors = 0;
	WinErr_t winerr = _doEnumDisplayMonitors(NULL, 0, &nMonitors);
	assert(winerr==ERROR_MORE_DATA);
	if(nMonitors<1)
		return RECT();

	OneMonitorInfo_st *arMonInfo = new OneMonitorInfo_st[nMonitors];
	CecArray_OneMonitorInfo_st cec = arMonInfo;

	int nRet = 0;
	winerr = _doEnumDisplayMonitors(arMonInfo, nMonitors, &nRet);
	if(nRet<nMonitors)
		nMonitors = nRet;
	assert(nMonitors>0);

	Rect_st *arMonitorRect = new Rect_st[nMonitors];
	CEC_raw_delete cec_monrect = arMonitorRect;

	for(int i=0; i<nMonitors; i++)
		arMonitorRect[i] = arMonInfo[i].rcMonitor;

	Rect_st outrect = mumo_ReposRect(uRECT, nMonitors, arMonitorRect, allow_straddle, allow_shrink);
	return outrect;
}


////////////////////////////////////////////////////////////////////////////
} // namespace WinMultiMon
////////////////////////////////////////////////////////////////////////////



/*
 ,-. .     .       .                          ,.  ;-.  ,
/    |     |       |                         /  \ |  ) |
| -. | ,-. |-. ,-: |   ,-. ;-. ,-: ,-. ,-.   |--| |-'  |
\  | | | | | | | | |   `-. | | | | |   |-'   |  | |    |
 `-' ' `-' `-' `-` '   `-' |-' `-` `-' `-'   '  ' '    '
*/

// Global space API implementation wrapper:


WinErr_t 
doEnumDisplayMonitors(OneMonitorInfo_st arMonInfo[], int nEles, int *pTotalMonitors)
{
	return WinMultiMon::_doEnumDisplayMonitors(arMonInfo, nEles, pTotalMonitors);
}

bool getMonitorRectByPoint(int screen_x, int screen_y, RECT *pMonitorRect)
{
	return WinMultiMon::_getMonitorRectByPoint(screen_x, screen_y, pMonitorRect);
}

Rect_st mumo_ReposRect(const Rect_st& urect,
	int nMonitors, const Rect_st arMonitorRect[],
	bool allow_straddle, bool allow_shrink)
{
	return WinMultiMon::_mumo_ReposRect(urect, nMonitors, arMonitorRect, allow_straddle, allow_shrink);
}


RECT mumo_PlaceRectInsideScreen(const RECT& urect, bool allow_straddle, bool allow_shrink)
{
	return WinMultiMon::_mumo_PlaceRectInsideScreen(urect, allow_straddle, allow_shrink);
}


#ifndef WinMultiMon_DEBUG
#include <CHHI_vaDBG_show.h>
#endif

#endif // [IMPL]

#endif
