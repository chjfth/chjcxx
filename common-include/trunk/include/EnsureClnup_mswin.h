#ifndef __ENSURECLNUP_MSWIN_H
#define __ENSURECLNUP_MSWIN_H

//#define WIN32_LEAN_AND_MEAN
//#include <windows.h>
// -- user should include the above two lines themselves

#include <EnsureClnup.h>


MakeCleanupPtrClass_winapi(Cec_PTRHANDLE, BOOL, CloseHandle, HANDLE)
	// This is for CreateEvent,OpenProcess, who return NULL as fail.
MakeCleanupIntClass_winapi(Cec_FILEHANDLE, BOOL, CloseHandle, HANDLE, INVALID_HANDLE_VALUE)
	// This is for CreateFile, who returns INVALID_HANDLE_VALUE as fail. Damn M$.

MakeCleanupPtrClass_winapi(Cec_HKEY, LONG, RegCloseKey, HKEY)
	// for RegOpenKeyEx 

MakeCleanupPtrClass_winapi(Cec_DeleteDC, BOOL, DeleteDC, HDC)
	// for CreateCompatibleDC, CreateDC

MakeCleanupPtrClass_winapi(Cec_DestroyMenu, BOOL, DestroyMenu, HMENU)
	// for CreatePopupMenu
MakeCleanupPtrClass_winapi(Cec_DestroyIcon, BOOL, DestroyIcon, HICON)
	// for CreateIconIndirect, CopyIcon, SHGetStockIconInfo 


#if (_WIN32_WINNT >= 0x0600) // Vista+
MakeCleanupIntClass_winapi(Cec_ReleaseActCtx, void, ReleaseActCtx, HANDLE, INVALID_HANDLE_VALUE)
	// for CreateActCtx
#endif


#ifdef _INC_SETUPAPI
MakeCleanupIntClass_winapi(Cec_HDEVINFO, BOOL, SetupDiDestroyDeviceInfoList, HDEVINFO, INVALID_HANDLE_VALUE)
#endif

//MakeCleanupPtrClass_winapi(Cec_HDC_Release, int, ReleaseDC, ); // bad! ReleaseDC need 2 params.


#endif
