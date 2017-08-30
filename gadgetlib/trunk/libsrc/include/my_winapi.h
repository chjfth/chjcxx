#ifndef __my_winapi_h_
#define __my_winapi_h_

#include <windows.h>
#include <Uxtheme.h>
#include <ShlObj.h>
#include <tchar.h>

// #include <stdio.h>

#include <mswin/dl_winapi.h> // In common-include


/* Sample:
DL_MAKE_WINAPI_WRAPPER("user32.dll", int, MessageBoxW, 
	(HWND hWnd, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType), 
	(hWnd, lpText, lpCaption, uType), 
	-2)
*/

#define hrERR_DL_ENTRY 0xA0000001 // as HRESULT

#if (_WIN32_WINNT > 0x0600) // only for Vista

DL_MAKE_WINAPI_WRAPPER("uxtheme.dll", HRESULT, GetBufferedPaintBits,
	(HPAINTBUFFER hBufferedPaint,__out RGBQUAD **ppbBuffer, __out int *pcxRow),
	(hBufferedPaint, ppbBuffer, pcxRow),
	hrERR_DL_ENTRY);

DL_MAKE_WINAPI_WRAPPER("uxtheme.dll", HPAINTBUFFER, BeginBufferedPaint,
	(HDC hdcTarget, const RECT* prcTarget, BP_BUFFERFORMAT dwFormat, __in_opt BP_PAINTPARAMS *pPaintParams, __out HDC *phdc),
	(hdcTarget, prcTarget, dwFormat, pPaintParams, phdc),
	NULL);


DL_MAKE_WINAPI_WRAPPER("uxtheme.dll", HRESULT, EndBufferedPaint,
	(HPAINTBUFFER hBufferedPaint, BOOL fUpdateTarget),
	(hBufferedPaint, fUpdateTarget),
	hrERR_DL_ENTRY);


DL_MAKE_WINAPI_WRAPPER("comctl32.dll", HRESULT, LoadIconMetric,
	(HINSTANCE hinst, LPCTSTR pszName, int lims, HICON *phico),
	(hinst, pszName, lims, phico),
	hrERR_DL_ENTRY)

#endif

#endif // end
