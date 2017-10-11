#ifndef __wintooltip_h_20171011_
#define __wintooltip_h_20171011_

#include <gadgetlib/ggt-mswin.h>

#ifdef __cplusplus
extern"C"{
#endif

#ifndef DLLEXPORT_gadgetlib
# define DLLEXPORT_gadgetlib
#endif

struct TooltipHandle_user_st { };
typedef TooltipHandle_user_st *TooltipHandle_t;

DLLEXPORT_gadgetlib
TooltipHandle_t ggt_CreateManualTooltip(HWND hOwner, bool isBalloon=false, WinErr_gt *pWinErr=NULL);
	// return NULL on fail

DLLEXPORT_gadgetlib
WinErr_gt ggt_TooltipShow(TooltipHandle_t htt, bool isShow, POINT *pt=NULL,
	const TCHAR *szfmt=NULL, ...);
	// pt.x/pt.y can be negative, which mean count from right/bottom edge.

DLLEXPORT_gadgetlib
bool ggt_TooltipDelete(TooltipHandle_t htt);
	// Release resource related to htt.
	// One ggt_CreateManualTooltip call must be paired with one ggt_TooltipDelete.

#ifdef __cplusplus
} // extern"C"{
#endif

#endif
