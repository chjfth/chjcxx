#ifndef __ReposNewbox_h_20170718_
#define __ReposNewbox_h_20170718_

#include <stdio.h>

#ifdef __cplusplus
extern"C"{
#endif

#ifndef DLLEXPORT_gadgetlib
# define DLLEXPORT_gadgetlib
#endif


struct Rect_st;

struct Pace_st { int x, y; };

struct Point_st
{
	int x, y;

	bool InRect(const Rect_st &r) const;

	Pace_st PaceToRect(const Rect_st &r) const;
};

struct Size_st
{
	int cx, cy;
};

struct Rect_st // same as Windows API RECT
{
	int left;
	int top;
	int right;
	int bottom;

//	Rect_st(){ left=top=right=bottom=0; } // would cause Visual C++ C2552 error.

	void Reset(){ left=top=right=bottom=0; }

	bool operator==(const Rect_st &ri)
	{
		return (left==ri.left && top==ri.top && right==ri.right && bottom==ri.bottom) ? true:false;
	}
	bool operator!=(const Rect_st &ri)
	{
		return !(*this==ri);
	}

	Pace_st PaceToRect(const Rect_st &rBig);
};

struct ReposNewboxInput_st
{
	int nMonitors; // monitor count
	Rect_st *arMonitorRect; // array size is determined by nMonitors;
	
	Rect_st rectParent; // parent window position&size

	Size_st sizeNewboxIdeal; // new box's ideal size
};


DLLEXPORT_gadgetlib
int ggt_ReposNewbox(const ReposNewboxInput_st &input, Rect_st *pRectNewBox=NULL);
	// Return monitor index of output newbox-Rect, zero-based.
	// Return -1 on error.


#ifdef __cplusplus
} // extern"C"{
#endif

#endif
