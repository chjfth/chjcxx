#ifndef __WinMultiMon_h_20250608_
#define __WinMultiMon_h_20250608_

#include <windows.h>

#ifndef WinErr_t_DEFINED
#define WinErr_t_DEFINED
typedef DWORD WinErr_t;
#endif

struct OneMonitorInfo_st
{
	bool isPrimary;
	RECT rcMonitor;
	RECT rcWorkArea;
	TCHAR szDevice[CCHDEVICENAME];
};

WinErr_t doEnumDisplayMonitors(OneMonitorInfo_st arMonInfo[], int arEles, int *pTotalMonitors);
// -- Return ERROR_MORE_DATA if arEles is too small(smaller than output *pTotalMonitors).

bool getMonitorRectByPoint(int screen_x, int screen_y, RECT *pMonitorRect);
// -- Given a Point(X,Y), find out which monitor contains that point.
//    Return true if pt is in one monitor, else false.
//    *pMonitorRect outputs the screen-coord of the containing monitor.


#ifdef WinMultiMon_IMPL

#include <commdefs.h>

namespace WinMultiMon // (private)
{


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

	if( !(emp.nUserFilled < emp.nUserMoninfo) )
		return TRUE;  // continue enumeration so that we can still update emp.nMonitorCount

	OneMonitorInfo_st &oneinfo = emp.arUserMoninfo[emp.nUserFilled];

	MONITORINFOEX minfo; // Damn M$: cannot just write = {sizeof(MONITORINFOEX)}
	minfo.cbSize = sizeof(MONITORINFOEX);
	BOOL b = GetMonitorInfo(hMonitor, &minfo);
	if(!b)
		return FALSE;

	oneinfo.isPrimary =	(minfo.dwFlags&MONITORINFOF_PRIMARY) ? true : false;
	oneinfo.rcMonitor = minfo.rcMonitor;
	oneinfo.rcWorkArea = minfo.rcWork;
	_tcscpy_s(oneinfo.szDevice, minfo.szDevice);

	emp.nUserFilled++;
	return TRUE;
}

WinErr_t 
doEnumDisplayMonitors(OneMonitorInfo_st arMonInfo[], int nEles, int *pTotalMonitors)
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


bool getMonitorRectByPoint(int screen_x, int screen_y, RECT *pMonitorRect)
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


} // namespace WinMultiMon (private)



WinErr_t 
doEnumDisplayMonitors(OneMonitorInfo_st arMonInfo[], int nEles, int *pTotalMonitors)
{
	return WinMultiMon::doEnumDisplayMonitors(arMonInfo, nEles, pTotalMonitors);
}

bool getMonitorRectByPoint(int screen_x, int screen_y, RECT *pMonitorRect)
{
	return WinMultiMon::getMonitorRectByPoint(screen_x, screen_y, pMonitorRect);
}


#endif // WinMultiMon_IMPL

#endif
