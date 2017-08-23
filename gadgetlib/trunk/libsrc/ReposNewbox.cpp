#include "stdafx.h"

#include "RECTxy.h"
#include <gadgetlib/ReposNewbox.h>

#define DLL_AUTO_EXPORT_STUB
extern"C" void gadgetlib_lib__ReposNewbox__DLL_AUTO_EXPORT_STUB(void){}


#define Abs(i) ( (i)>=0 ? (i) : 0-(i) )
#define _Min_(a,b) ((a)<=(b) ? (a) : (b))
#define _Max_(a,b) ((a)>=(b) ? (a) : (b))
#define _Mid_(a,b,c) ( _Max_(a,_Min_(b,c)) )


inline bool InRange2(int n, const int *pi)
{
	// If pi points to RECT.left, it will check whether n in range [RECT.left, RECT.right) .
	// If pi points to RECT.top, it will check whether n in range [RECT.top, RECT.bottom) .
	return (n>=pi[0] && n<pi[2]) ? true : false;
}


bool 
Point_st::InRect(const Rect_st &r) const
{
	bool xInRange = InRange2(x, &r.left);
	bool yInRange = InRange2(y, &r.top);
	return (xInRange && yInRange) ? true : false;
}

Pace_st 
Point_st::PaceToRect(const Rect_st &r) const
{
	// Return: 
	// If ret.x==30, it means this pt should walk toward right 30 pixels to reach the Rect's left border.
	// If ret.x==-20, it means this pt should walk toward left 20 pixels to reach the Rect's right border.
	// If this pt falls inside the rect(per axis), the return-value is 0.

	Pace_st pace = {0,0};

	if(x<r.left)
		pace.x = r.left - x;
	else if(x>=r.right)
		pace.x = r.right - x;

	if(y<r.top)
		pace.y = r.top - y;
	else if(y>=r.bottom)
		pace.y = r.bottom - y;

	return pace;
}


Pace_st 
Rect_st::PaceToRect(const Rect_st &rBig)
{
	// Idea similar to Point_st:PaceToRect().
	// this Rect must be x&y smaller than or equal to rBig.

	Pace_st paceRet = {0,0};

	// memo: A is this-rect's left-top corner; B is this-rect's right-bottom corner.
	Pace_st paceA = ((Point_st*)(&this->left))->PaceToRect(rBig);
	Pace_st paceB = ((Point_st*)(&this->right))->PaceToRect(rBig);

	// For each pace.x/pace.y, pick the one with larger abs-value.
	paceRet.x = Abs(paceA.x)>Abs(paceB.x) ? paceA.x : paceB.x;
	paceRet.y = Abs(paceA.y)>Abs(paceB.y) ? paceA.y : paceB.y;

	return paceRet;
}

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
