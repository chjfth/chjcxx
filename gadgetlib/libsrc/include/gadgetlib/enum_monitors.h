#ifndef __enum_monitors_h_20170719_
#define __enum_monitors_h_20170719_

#ifdef __cplusplus
extern"C"{
#endif

struct OneMonitorInfo_st
{
	RECT rcMonitor;
	RECT rcWorkArea;
	bool isPrimary;
};

WinErr_t ggt_EnumMonitors(OneMonitorInfo_st arMonInfo[], int arEles, int *pTotalMonitors);
	// Return ERROR_MORE_DATA if arEles is too small(smaller than *pTotalMonitors).

/* Usage example:
	CAutoBuf<OneMonitorInfo_st, sizeof(OneMonitorInfo_st)> abMonsinfo;
	WinErr_t winerr = 0;
	do {
		winerr = ggt_EnumMonitors(abMonsinfo, abMonsinfo, abMonsinfo);
	} while(winerr==ERROR_MORE_DATA);
*/



#ifdef __cplusplus
} // extern"C"{
#endif




#endif
