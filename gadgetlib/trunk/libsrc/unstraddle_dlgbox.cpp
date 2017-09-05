#include "stdafx.h"

#include <gadgetlib/unstraddle_dlgbox.h>

#include <AutoBuf.h>
#include <gadgetlib/enum_monitors.h>
#include <gadgetlib/ReposNewbox.h>

#define DLL_AUTO_EXPORT_STUB
extern"C" void gadgetlib_lib__unstraddle_dlgbox__DLL_AUTO_EXPORT_STUB(void){}


Rect_st 
ggt_unstraddle_dlgbox(const Rect_st &rectRef, int newWidth, int newHeight, int *pIdxMonitor)
{
	CAutoBuf<OneMonitorInfo_st, sizeof(OneMonitorInfo_st)> abMonsinfo;
	WinErr_t winerr = 0;
	do {
		winerr = ggt_EnumMonitors(abMonsinfo, abMonsinfo, abMonsinfo);
	} while(winerr==ERROR_MORE_DATA);

	if(winerr)
		return rectRef;

	ReposNewboxInput_st nbi = {0};
	nbi.nMonitors = abMonsinfo;
	//
	int rect_buf_bytes = nbi.nMonitors*sizeof(RECT);
	Cec_delete_pchar rectbuf = new char[rect_buf_bytes];
	nbi.arMonitorRect = (Rect_st*)(void*)rectbuf;
	for(int i=0; i<nbi.nMonitors; i++)
		nbi.arMonitorRect[i] = *(Rect_st*)&abMonsinfo[i].rcWorkArea;
	
	nbi.rectParent = rectRef;
	nbi.sizeNewboxIdeal.cx = newWidth;
	nbi.sizeNewboxIdeal.cy = newHeight;
	//
	Rect_st ret_rect = {0};
	int idxNewMonitor = ggt_ReposNewbox(nbi, &ret_rect);
	if(idxNewMonitor<0)
	{
		return rectRef; // fail. not likely to happen
	}

	if(pIdxMonitor)
		*pIdxMonitor = idxNewMonitor;

	return ret_rect;
}

