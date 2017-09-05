#include "stdafx.h"

#include <RECTxy.h>
#include <gadgetlib/ReposNewbox.h>

#define DLL_AUTO_EXPORT_STUB
extern"C" void gadgetlib_lib__ReposNewbox__DLL_AUTO_EXPORT_STUB(void){}


#define Abs(i) ( (i)>=0 ? (i) : 0-(i) )
#define _Min_(a,b) ((a)<=(b) ? (a) : (b))
#define _Max_(a,b) ((a)>=(b) ? (a) : (b))
#define _Mid_(a,b,c) ( _Max_(a,_Min_(b,c)) )


static int 
DetermineMonitor(int nMonitors, const Rect_st *arMonitorRect, const Point_st &pt)
{
	// Determine target monitor:
	// * If ptcenter is right inside some monitor, use that monitor.
	// * Use nearest pacexy to determine target monitor.

	int i;
	for(i=0; i<nMonitors; i++)
	{
		if(pt.InRect(arMonitorRect[i]))
			return i;
	}

	int pacexy_min = 0x7FFFffff;
	int iselect = 0;

	for(i=0; i<nMonitors; i++)
	{
		Pace_st pace = pt.PaceToRect(arMonitorRect[i]);
		assert(! (pace.x==0 && pace.y==0) );

		int pacexy = Abs(pace.x)+Abs(pace.y);
		if(pacexy<pacexy_min) 
		{	// record the new min-pace
			pacexy_min = pacexy;
			iselect = i;
		}
	}

	return iselect;
}

int 
ggt_ReposNewbox(const ReposNewboxInput_st &input, Rect_st *pRectNewBox)
{
	Rect_st tmp_rectNewbox;
	if(!pRectNewBox)
		pRectNewBox = &tmp_rectNewbox;

	Point_st ptcenter = { RECTxcenter(input.rectParent), RECTycenter(input.rectParent) };

	int idxMonitor = DetermineMonitor(input.nMonitors, input.arMonitorRect, ptcenter);

	// Determine newbox's dimension, the dimension must not exceed the target monitor.

	Rect_st &monitorRect = input.arMonitorRect[idxMonitor]; // target monitor's position

	int cxNewbox = _Min_( input.sizeNewboxIdeal.cx, RECTcx(monitorRect) );
	int cyNewbox = _Min_( input.sizeNewboxIdeal.cy, RECTcy(monitorRect) );

	// Determine centerRect: Place the newbox centered to parent, its rect.
	Rect_st centerRect;
	centerRect.left = ptcenter.x - cxNewbox/2;
	centerRect.top  = ptcenter.y - cyNewbox/2;
	centerRect.right =  centerRect.left + cxNewbox;
	centerRect.bottom = centerRect.top + cyNewbox;

	// Move the centerRect(if necessary) so that it all falls inside the target monitor.
	Pace_st pace = centerRect.PaceToRect(monitorRect);

	pRectNewBox->left = centerRect.left + pace.x;
	pRectNewBox->right = centerRect.right + pace.x;
	pRectNewBox->top = centerRect.top + pace.y;
	pRectNewBox->bottom = centerRect.bottom + pace.y;

	return idxMonitor;
}
