#ifndef __my_winapi_h_
#define __my_winapi_h_

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <Uxtheme.h>
#include <ShlObj.h>
#include <tchar.h>

// #include <stdio.h>

#include <mswin/dl_winapi.h> // In common-include


/* Sample:
DL_WINAPI_MAKE_WRAPPER("user32.dll", int, MessageBoxW, 
	(HWND hWnd, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType), 
	(hWnd, lpText, lpCaption, uType), 
	-2)
*/

#if (_WIN32_WINNT > 0x0600) // only for Vista

DL_WINAPI_MAKE_WRAPPER("uxtheme.dll", HRESULT, GetBufferedPaintBits,
	(HPAINTBUFFER hBufferedPaint,__out RGBQUAD **ppbBuffer, __out int *pcxRow),
	(hBufferedPaint, ppbBuffer, pcxRow),
	DL_WINAPI_ERROR_FIND_ENTRY);

DL_WINAPI_MAKE_WRAPPER("uxtheme.dll", HPAINTBUFFER, BeginBufferedPaint,
	(HDC hdcTarget, const RECT* prcTarget, BP_BUFFERFORMAT dwFormat, __in_opt BP_PAINTPARAMS *pPaintParams, __out HDC *phdc),
	(hdcTarget, prcTarget, dwFormat, pPaintParams, phdc),
	NULL);


DL_WINAPI_MAKE_WRAPPER("uxtheme.dll", HRESULT, EndBufferedPaint,
	(HPAINTBUFFER hBufferedPaint, BOOL fUpdateTarget),
	(hBufferedPaint, fUpdateTarget),
	DL_WINAPI_ERROR_FIND_ENTRY);


DL_WINAPI_MAKE_WRAPPER("comctl32.dll", HRESULT, LoadIconMetric,
	(HINSTANCE hinst, LPCTSTR pszName, int lims, HICON *phico),
	(hinst, pszName, lims, phico),
	DL_WINAPI_ERROR_FIND_ENTRY)

#endif

#endif // end
