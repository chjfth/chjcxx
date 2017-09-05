#ifndef __ReposNewbox_h_20170718_
#define __ReposNewbox_h_20170718_

#include <stdio.h>

#include <RECTxy.h>

#ifdef __cplusplus
extern"C"{
#endif

#ifndef DLLEXPORT_gadgetlib
# define DLLEXPORT_gadgetlib
#endif



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
