#ifndef __MenuWithIcon_h_20170821_
#define __MenuWithIcon_h_20170821_

#ifdef __cplusplus
extern"C" {
#endif

#ifndef DLLEXPORT_gadgetlib
# define DLLEXPORT_gadgetlib
#endif

enum LoadIconErr_et
{
	LIE_Succ = 0,
	LIE_Fail = -1,

	LIE_NoMem = -4,
	LIE_LoadIcon = -5,
	LIE_LoadIconMetric = -6, // vista API dynamic loading fail
	LIE_GetIconInfo = -7,
	LIE_GetBufferedPaintBits = -8,
	LIE_OldBitmapMissing = -9,
	LIE_BeginBufferedPaint = -10,
	LIE_SetMenuItemInfo = -11,
	LIE_ShrinkBitmap = -12,
};

struct ICONMENUENTRY
{
	UINT        idMenuItem;
	HINSTANCE   hinst;
	LPCTSTR     pIconId;

	HBITMAP hbmpInternal; // internal use
};

DLLEXPORT_gadgetlib
UINT ggt_TrackPopupMenuWithIcon(
	HMENU hmenu, UINT fuFlags, int x, int y, HWND hwnd, __in_opt LPTPMPARAMS lptpm,
	__in_ecount_opt(nIcons) ICONMENUENTRY arIcons[], UINT nIcons, 
	LoadIconErr_et *pLIE=NULL);
// Return the same value as WinAPI TrackPopupMenuEx().
// Even if icon-loading fails(*pLIE will indicate error code), the text-only menu
// will still pops out.


DLLEXPORT_gadgetlib
bool ggt_TrackPopupMenuIsVistaStyle();


#ifdef __cplusplus
} // extern"C" {
#endif

#endif
