#include "stdafx.h"

#include <gadgetlib/mswin/oldfuncs_wrapper.h>
#include <gadgetlib/MenuWithIcon.h>

#define DLL_AUTO_EXPORT_STUB
extern"C" void gadgetlib_lib__MenuWithIcon__DLL_AUTO_EXPORT_STUB(void){}

#if (_WIN32_WINNT >= 0x0600)
// Yes, we need at least Visual C++ 2008 to get complete feature from this cpp.
#define HAS_VISTA_API
#endif

#define NEED_VISTA

// Code for Vista code path is from: https://msdn.microsoft.com/en-us/library/bb757020.aspx
// Code for XP code path is from: MsSDK 2003 samples: $MsSDK2003\Samples\winui\Menu\Menu.c

#pragma comment(lib, "uxtheme.lib")  // BeginBufferedPaint etc
#pragma comment(lib, "comctl32.lib") // LoadIconMetric 

#ifdef HAS_VISTA_API

typedef DWORD ARGB;

static void 
InitBitmapInfo(__out_bcount(cbInfo) BITMAPINFO *pbmi, ULONG cbInfo, LONG cx, LONG cy, WORD bpp)
{
	ZeroMemory(pbmi, cbInfo);
	pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	pbmi->bmiHeader.biPlanes = 1;
	pbmi->bmiHeader.biCompression = BI_RGB;

	pbmi->bmiHeader.biWidth = cx;
	pbmi->bmiHeader.biHeight = cy;
	pbmi->bmiHeader.biBitCount = bpp;
}

static HRESULT 
Create32BitHBITMAP(HDC hdc, const SIZE *psize, 
	__deref_opt_out void **ppvBits, __out HBITMAP* phBmp)
{
	*phBmp = NULL;

	BITMAPINFO bmi;
	InitBitmapInfo(&bmi, sizeof(bmi), psize->cx, psize->cy, 32);

	HDC hdcUsed = hdc ? hdc : GetDC(NULL);
	if (hdcUsed)
	{
		*phBmp = CreateDIBSection(hdcUsed, &bmi, DIB_RGB_COLORS, ppvBits, NULL, 0);
		if (hdc != hdcUsed)
		{
			ReleaseDC(NULL, hdcUsed);
		}
	}
	return (NULL == *phBmp) ? E_OUTOFMEMORY : S_OK;
}

static HRESULT 
AddBitmapToMenuItem(HMENU hmenu, int iItem, BOOL fByPosition, HBITMAP hbmp)
{
	HRESULT hr = E_FAIL;

	MENUITEMINFO mii = { sizeof(mii) };
	mii.fMask = MIIM_BITMAP;
	mii.hbmpItem = hbmp;
	if (SetMenuItemInfo(hmenu, iItem, fByPosition, &mii))
	{
		hr = S_OK;
	}

	return hr;
}

static HRESULT 
ConvertToPARGB32(HDC hdc, __inout ARGB *pargb, HBITMAP hbmp, SIZE& sizImage, int cxRow)
{
	BITMAPINFO bmi;
	InitBitmapInfo(&bmi, sizeof(bmi), sizImage.cx, sizImage.cy, 32);

	HRESULT hr = E_OUTOFMEMORY;
	HANDLE hHeap = GetProcessHeap();
	void *pvBits = HeapAlloc(hHeap, 0, bmi.bmiHeader.biWidth * 4 * bmi.bmiHeader.biHeight);
	if (pvBits)
	{
		hr = E_UNEXPECTED;
		int dibHeight = GetDIBits(hdc, hbmp, 0, bmi.bmiHeader.biHeight, pvBits, &bmi, DIB_RGB_COLORS);
		if (dibHeight == bmi.bmiHeader.biHeight)
		{
			ULONG cxDelta = cxRow - bmi.bmiHeader.biWidth;
			ARGB *pargbMask = static_cast<ARGB *>(pvBits);

			for (ULONG y = bmi.bmiHeader.biHeight; y; --y)
			{
				for (ULONG x = bmi.bmiHeader.biWidth; x; --x)
				{
					if (*pargbMask++)
					{
						// transparent pixel
						*pargb++ = 0;
					}
					else
					{
						// opaque pixel
						*pargb++ |= 0xFF000000;
					}
				}

				pargb += cxDelta;
			}

			hr = S_OK;
		}

		HeapFree(hHeap, 0, pvBits);
	}

	return hr;
}

static bool 
HasAlpha(__in ARGB *pargb, SIZE& sizImage, int cxRow)
{
	ULONG cxDelta = cxRow - sizImage.cx;
	for (ULONG y = sizImage.cy; y; --y)
	{
		for (ULONG x = sizImage.cx; x; --x)
		{
			if (*pargb++ & 0xFF000000)
			{
				return true;
			}
		}

		pargb += cxDelta;
	}

	return false;
}

static LoadIconErr_et  
ConvertBufferToPARGB32(ARGB *pargb, HDC hdc, HICON hicon, SIZE& sizIcon, int cxRow)
{
	if(HasAlpha(pargb, sizIcon, cxRow))
		return LIE_Succ;

	ICONINFO info;
	if (GetIconInfo(hicon, &info))
	{
		LoadIconErr_et lie = LIE_NoMem;
		if (info.hbmMask)
		{
			HRESULT hr = ConvertToPARGB32(hdc, pargb, info.hbmMask, sizIcon, cxRow);
			if(SUCCEEDED(hr))
				lie = LIE_Succ;
		}

		DeleteObject(info.hbmColor);
		DeleteObject(info.hbmMask);

		return lie;
	}
	else
		return LIE_GetIconInfo;
}


static LoadIconErr_et  
ConvertBufferToPARGB32(HPAINTBUFFER hPaintBuffer, HDC hdc, HICON hicon, SIZE& sizIcon)
{
	RGBQUAD *prgbQuad;
	int cxRow;
	HRESULT hr = dl_GetBufferedPaintBits(hPaintBuffer, &prgbQuad, &cxRow); // Vista only
	if (SUCCEEDED(hr))
	{
		ARGB *pargb = reinterpret_cast<ARGB *>(prgbQuad);
		return ConvertBufferToPARGB32(pargb, hdc, hicon, sizIcon, cxRow);
	}
	else
		return LIE_GetBufferedPaintBits;
}

static LoadIconErr_et 
vista_AddIconToMenuItem(HMENU hmenu, int iMenuItem, BOOL fByPosition, HICON hicon, 
	BOOL fAutoDestroy, __out_opt HBITMAP *phbmp)
{
	// The routine is very ugly, infinite nested coding style.

	LoadIconErr_et lie = LIE_Fail;
	HRESULT hr = E_OUTOFMEMORY;
	HBITMAP hbmp = NULL;

	BOOL b;
	SIZE sizIcon;
	sizIcon.cx = GetSystemMetrics(SM_CXSMICON);
	sizIcon.cy = GetSystemMetrics(SM_CYSMICON);

	RECT rcIcon;
	SetRect(&rcIcon, 0, 0, sizIcon.cx, sizIcon.cy);

	ARGB *pBmpBits = NULL;
	HDC hdcDest = CreateCompatibleDC(NULL);
	if (hdcDest)
	{
		hr = Create32BitHBITMAP(hdcDest, &sizIcon, (void**)&pBmpBits, &hbmp);
		if (SUCCEEDED(hr))
		{
//			hrr = E_FAIL;

			HBITMAP hbmpOld = (HBITMAP)SelectObject(hdcDest, hbmp);
			if (hbmpOld)
			{
				BLENDFUNCTION bfAlpha = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
				BP_PAINTPARAMS paintParams = {0};
				paintParams.cbSize = sizeof(paintParams);
				paintParams.dwFlags = BPPF_ERASE;
				paintParams.pBlendFunction = &bfAlpha;
#ifdef NEED_VISTA
				HDC hdcBuffer;
				HPAINTBUFFER hPaintBuffer = dl_BeginBufferedPaint(hdcDest, &rcIcon, BPBF_DIB, 
					&paintParams, &hdcBuffer);

				if (hPaintBuffer)
				{
					b = DrawIconEx(hdcBuffer, 0, 0, hicon, sizIcon.cx, sizIcon.cy, 0, NULL, DI_NORMAL);
					if(b)
					{
						// If icon did not have an alpha channel, we need to convert buffer to PARGB.
						lie = ConvertBufferToPARGB32(hPaintBuffer, hdcDest, hicon, sizIcon);
					}

					// This will write the buffer contents to the destination bitmap.
					dl_EndBufferedPaint(hPaintBuffer, TRUE);
				}
				else
					lie = LIE_BeginBufferedPaint;
#else
				b = DrawIconEx(hdcDest, 0,0, hicon, sizIcon.cx, sizIcon.cy, 0, NULL, DI_NORMAL);
				if(b)
				{
					hrr = ConvertBufferToPARGB32(pBmpBits, hdcDest, hicon, sizIcon, 
						sizIcon.cx // appropriate?
						);
				}

#endif // NEED_VISTA

				SelectObject(hdcDest, hbmpOld);
			}
			else
				lie = LIE_OldBitmapMissing;
		}
		else
			lie = LIE_NoMem;

		DeleteDC(hdcDest);

	} // ok: HDC hdcDest = CreateCompatibleDC(NULL);

	if (lie==LIE_Succ)
	{
		hr = AddBitmapToMenuItem(hmenu, iMenuItem, fByPosition, hbmp);
		if(!SUCCEEDED(hr))
			lie = LIE_SetMenuItemInfo;
	}

	if (lie!=LIE_Succ)
	{
		DeleteObject(hbmp);
		hbmp = NULL;
	}

	if (fAutoDestroy)
	{
		DestroyIcon(hicon);
	}

	if (phbmp)
	{
		*phbmp = hbmp;
		// Who will destroy it later?
	}

	return lie;
}


LoadIconErr_et 
vista_AttachIconToMenuitem(HINSTANCE hInstExeDll, LPCTSTR iconResId, HMENU hmenu, UINT menuCmdId)
{
	HICON hicon = NULL;
	HRESULT hr = dl_LoadIconMetric(hInstExeDll, iconResId, LIM_SMALL, &hicon);
	if(SUCCEEDED(hr))
	{
		HBITMAP hbmp_debug = NULL; // test purpose
		LoadIconErr_et lie = vista_AddIconToMenuItem(hmenu, menuCmdId, FALSE, hicon, TRUE, &hbmp_debug);
		hbmp_debug = NULL;

		return lie;
	}
	else 
	{
		if(hr==DL_WINAPI_ERROR_FIND_ENTRY)
			return LIE_LoadIconMetric;
		else
			return LIE_LoadIcon;
	}
}

#endif // #ifdef HAS_VISTA_API

/****************************************************************************
 *                                                                          *
 *  FUNCTION   : ShrinkBitmap(HWND hwnd, HBITMAP hbm)                                    *
 *                                                                          *
 *  PURPOSE    : This function shrinks a 64x64 bitmap into a bitmap useable *
 *               for the user-defined checkmark for menu items. This can be *
 *               easily generalized to shrink bitmaps of any size.          *
 *                                                                          *
 *  RETURNS    : HBITMAP - A handle to the new bitmap. Should DeleteObject it later.
 *                                                                          *
 ****************************************************************************/
static HBITMAP 
ShrinkBitmap_from_Icon (HWND hwnd, HICON hicon)
{
	HDC     hdc            = NULL;
	HDC     hmemorydcNew   = NULL;
	LONG    checkMarkSize  = 0;
	HBITMAP hCheckBitmap   = NULL; // the HBITMAP as checkmark
	HBITMAP hOldBitmapSave = NULL;
	BOOL b;
	int destW, destH;

	hdc = GetDC (hwnd);

    /* Create DCs for the source (old) and target (new) bitmaps */
	hmemorydcNew = CreateCompatibleDC (hdc);

    /* Determine the dimensions of the default menu checkmark and
     * create a target bitmap of the same dimensions
     */

    checkMarkSize = GetMenuCheckMarkDimensions ();
	
	destW = LOWORD (checkMarkSize), destH = HIWORD (checkMarkSize); 
		// will be 13*13 on WinXP; 15*15 or 17*17 on Win7.
//	destW = destH = 32;

	hCheckBitmap  = CreateCompatibleBitmap (hdc, destW, destH);

	hOldBitmapSave = (HBITMAP)SelectObject (hmemorydcNew, hCheckBitmap);

	RECT rectFill = {0,0, destW, destH};
	FillRect(hmemorydcNew, &rectFill, (HBRUSH)GetStockObject(WHITE_BRUSH));

	b = DrawIconEx(hmemorydcNew, 0,0, hicon, destW, destH, 0,NULL, DI_NORMAL); (void)b;

    /* De-select the bitmaps and clean up .. */
	SelectObject (hmemorydcNew, hOldBitmapSave);
	DeleteDC (hmemorydcNew);
	ReleaseDC (hwnd, hdc);

    /* .. and return a handle to the target bitmap */
    return hCheckBitmap;
}


LoadIconErr_et 
xp_AttachIconToMenuitem(HINSTANCE hInstExeDll, LPCTSTR iconResId, HMENU hmenu, UINT menuCmdId,
	HBITMAP *phbmp) // caller should free the output *phbmp
{
	HICON hicon = LoadIcon(hInstExeDll, iconResId);
	if(!hicon)
		return LIE_LoadIcon;

	*phbmp = ShrinkBitmap_from_Icon(NULL, hicon);
	if(!*phbmp)
		return LIE_ShrinkBitmap;

//	HBITMAP hbmp = ShrinkBitmap (NULL, LoadBitmap (hInstExeDll?hInstExeDll:g_hinst, MAKEINTRESOURCE(IDB_BITMAP1))); // test
//	HBITMAP hbmp = LoadBitmap (hInstExeDll?hInstExeDll:g_hinst, MAKEINTRESOURCE(IDB_BITMAP1)); // test
	
	BOOL b = SetMenuItemBitmaps(hmenu, menuCmdId, MF_BYCOMMAND, *phbmp, *phbmp);
	if(b)
		return LIE_Succ;
	else
	{
		DeleteObject(*phbmp);
		*phbmp = NULL;
		return LIE_SetMenuItemInfo;
	}
}



UINT ggt_TrackPopupMenuWithIcon(
	HMENU hmenu, UINT fuFlags, int x, int y, HWND hwnd, __in_opt LPTPMPARAMS lptpm,
	__in_ecount_opt(nIcons) ICONMENUENTRY arIcons[], UINT nIcons, LoadIconErr_et *pLIE,
	bool isShowIconOnWinXP)
{
	LoadIconErr_et _lie;
	if(!pLIE)
		pLIE = &_lie;
	*pLIE = LIE_Succ; // assume succ

	bool isVisualStyle = ggt_TrackPopupMenuIsVistaStyle();

	if(!isVisualStyle && !isShowIconOnWinXP)
	{
		UINT mret = TrackPopupMenu(hmenu, fuFlags, x, y, 0, hwnd, NULL);
		return mret;
	}

//	HBITMAP xp_hbmp = NULL;

	MENUINFO menuInfo = { sizeof(menuInfo) };
	menuInfo.fMask = MIM_STYLE;

	if (nIcons)
	{
		UINT n;
		for (n = 0; n < nIcons; ++n)
			arIcons[n].hbmpInternal = NULL; // clear to 0 first

		LoadIconErr_et lieNow = LIE_Succ;
		for (n = 0; n < nIcons; ++n)
		{
			// note: Even if some icon loads error, we still try to show other correct icons

#ifdef HAS_VISTA_API
			if(isVisualStyle)
			{
				lieNow = vista_AttachIconToMenuitem(
					arIcons[n].hinst, arIcons[n].pIconId, hmenu, arIcons[n].idMenuItem);
			}
			else
#endif
			{
				lieNow = xp_AttachIconToMenuitem(
					arIcons[n].hinst, arIcons[n].pIconId, hmenu, arIcons[n].idMenuItem,
					&arIcons[n].hbmpInternal);
			}

			if(lieNow!=LIE_Succ)
			{
				// only record first error, e.g. LoadIcon() cannot find the icon resource
				if(*pLIE==LIE_Succ)
					*pLIE = lieNow;
			}
		}

		if(isVisualStyle)
		{
			GetMenuInfo(hmenu, &menuInfo);

			MENUINFO menuInfoNew = menuInfo;
			menuInfoNew.dwStyle = (menuInfo.dwStyle & ~MNS_NOCHECK) | MNS_CHECKORBMP;
			SetMenuInfo(hmenu, &menuInfoNew);
		}
	}

	UINT mret = TrackPopupMenuEx(hmenu, fuFlags, x, y, hwnd, lptpm); 
		// (old comment?) Why fRet gets FALSE when I really see the icon-menu?

	// Clean up resources below
	if (isVisualStyle)
	{
		// Chj: This code suggests: We can DeleteObject the HBITMAP as soon as the menu is displayed on screen.
		for (UINT n = 0; n < nIcons; ++n)
		{
			MENUITEMINFO mii = { sizeof(mii) };
			mii.fMask = MIIM_BITMAP;

			if (GetMenuItemInfo(hmenu, arIcons[n].idMenuItem, FALSE, &mii))
			{
				DeleteObject(mii.hbmpItem); // on Vista, mii.hbmpItem should equal to hbmp_debug values
				mii.hbmpItem = NULL;
				SetMenuItemInfo(hmenu, arIcons[n].idMenuItem, FALSE, &mii);
			}
		}

		if(nIcons)
			SetMenuInfo(hmenu, &menuInfo);
	}
	else
	{
		for(UINT n=0; n<nIcons; ++n)
		{
			HBITMAP hbmpToDel = arIcons[n].hbmpInternal;
			if(hbmpToDel)
				DeleteObject(hbmpToDel);
		}
	}

	return mret;
}



//////////////////////////////////////////////////////////////////////////

bool 
ggt_TrackPopupMenuIsVistaStyle()
{
	if (ggt_IsWinverOrAbove(ggt_WINVISTA))
	{
		if (IsAppThemed())
			return true;
		else
			return false;
	}
	else 
	{
		return false;
	}
}

