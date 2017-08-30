#include "stdafx.h"

#include <gadgetlib/enum_monitors.h>

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
	LPARAM dwData       // data
	)
{
	EnumMonitorPrivate_st &emp = *(EnumMonitorPrivate_st*)dwData;
	emp.nMonitorCount++;

	if( !(emp.nUserFilled < emp.nUserMoninfo) )
		return TRUE;  // continue enumeration so that we can still update emp.nMonitorCount

	OneMonitorInfo_st &oneinfo = emp.arUserMoninfo[emp.nUserFilled];

	MONITORINFO minfo = {sizeof(MONITORINFO)};
	BOOL b = GetMonitorInfo(hMonitor, &minfo);
	if(!b)
		return FALSE;

	oneinfo.isPrimary =	(minfo.dwFlags&MONITORINFOF_PRIMARY) ? true : false;
	oneinfo.rcMonitor = minfo.rcMonitor;
	oneinfo.rcWorkArea = minfo.rcWork;

	emp.nUserFilled++;
	return TRUE;
}


WinErr_t 
ggt_EnumMonitors(OneMonitorInfo_st arMonInfo[], int arEles, int *pTotalMonitors)
{
	int TotalMonitors = 0;
	if(!pTotalMonitors)
		pTotalMonitors = &TotalMonitors;

	SetLastError(0);

	EnumMonitorPrivate_st emp = {0};
	emp.arUserMoninfo = arMonInfo;
	emp.nUserMoninfo = arEles;
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
