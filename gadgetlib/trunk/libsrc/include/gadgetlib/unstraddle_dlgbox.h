#ifndef __unstraddle_dlgbox_h_20170905_
#define __unstraddle_dlgbox_h_20170905_

#include <RECTxy.h>

#ifdef __cplusplus
extern"C"{
#endif

#ifndef DLLEXPORT_gadgetlib
# define DLLEXPORT_gadgetlib
#endif


DLLEXPORT_gadgetlib
Rect_st ggt_unstraddle_dlgbox(const Rect_st &rectRef, int newWidth, int newHeight, 
	int *pIdxMonitor=NULL);




#ifdef __cplusplus
} // extern"C"{
#endif


#endif
