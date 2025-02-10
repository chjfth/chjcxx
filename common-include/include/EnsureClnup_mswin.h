#ifndef __EnsureClnup_mswin_h_20250209_
#define __EnsureClnup_mswin_h_20250209_

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "EnsureClnup.h"


MakeDelega_CleanupCxxPtrN(TCHAR, Cec_delete_TCHAR)


MakeCleanupPtrClass_winapi(Cec_PTRHANDLE, BOOL, CloseHandle, HANDLE)
	// This is for CreateEvent,OpenProcess, who return NULL as fail.
MakeCleanupClass_winapi(Cec_FILEHANDLE, BOOL, CloseHandle, HANDLE, INVALID_HANDLE_VALUE)
	// This is for CreateFile, who returns INVALID_HANDLE_VALUE as fail. Damn M$.

MakeCleanupPtrClass_winapi(Cec_HKEY, LONG, RegCloseKey, HKEY)
	// for RegOpenKeyEx 

MakeCleanupPtrClass_winapi(Cec_DeleteDC, BOOL, DeleteDC, HDC)
	// for CreateCompatibleDC, CreateDC

MakeCleanupPtrClass_winapi(Cec_DestroyMenu, BOOL, DestroyMenu, HMENU)
	// for CreatePopupMenu
MakeCleanupPtrClass_winapi(Cec_DestroyIcon, BOOL, DestroyIcon, HICON)
	// for CreateIconIndirect, CopyIcon, SHGetStockIconInfo 

MakeCleanupPtrClass_winapi(Cec_LocalFree, HLOCAL, LocalFree, HLOCAL)
	// for ConvertSidToStringSid

MakeCleanupPtrClass_winapi(Cec_FreeSid, PVOID, FreeSid, PSID)
	// for AllocateAndInitializeSid

// SetupDi... functions:

#ifdef _INC_SETUPAPI // #include <setupapi.h>

MakeCleanupClass_winapi(Cec_HDEVINFO, BOOL, SetupDiDestroyDeviceInfoList, HDEVINFO, INVALID_HANDLE_VALUE)

MakeCleanupClass_winapi(Cec_SetupDiHKEY, LONG, RegCloseKey, HKEY, (HKEY)INVALID_HANDLE_VALUE)
	// for SetupDiOpenDevRegKey
	// Note: We need to write '(HKEY)INVALID_HANDLE_VALUE', otherwise we get VS2010 cl.exe error:
	// error C2440: 'specialization' : cannot convert from 'HANDLE' to 'HKEY'

#endif

//MakeCleanupPtrClass_winapi(Cec_HDC_Release, int, ReleaseDC, ); // bad! ReleaseDC need 2 params.


#endif
