#ifndef __MenuWithIcon_h_20170821_
#define __MenuWithIcon_h_20170821_

#ifdef __cplusplus
extern"C" {
#endif

#ifndef DLLEXPORT_gadgetlib
# define DLLEXPORT_gadgetlib
#endif


struct ICONMENUENTRY
{
	UINT        idMenuItem;
	HINSTANCE   hinst;
	LPCTSTR     pIconId;

	HBITMAP hbmpInternal; // internal use
};

DLLEXPORT_gadgetlib
bool ggt_TrackPopupMenuWithIcon(
	HMENU hmenu, UINT fuFlags, int x, int y, HWND hwnd, __in_opt LPTPMPARAMS lptpm,
	UINT nIcons, __in_ecount_opt(nIcons) ICONMENUENTRY *pIcons);


DLLEXPORT_gadgetlib
bool ggt_TrackPopupMenuIsVistaStyle();


#ifdef __cplusplus
} // extern"C" {
#endif

#endif
