#ifndef __ENSURECLNUP_MSWIN_H
#define __ENSURECLNUP_MSWIN_H

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <EnsureClnup.h>


MakeCleanupPtrClass_winapi(Cec_PTRHANDLE, BOOL, CloseHandle, HANDLE)
	// This is for CreateEvent,OpenProcess, who return NULL as fail.
MakeCleanupIntClass_winapi(Cec_FILEHANDLE, BOOL, CloseHandle, HANDLE, INVALID_HANDLE_VALUE)
	// This is for CreateFile, who returns INVALID_HANDLE_VALUE as fail. Damn M$.

MakeCleanupPtrClass_winapi(Cec_HKEY, LONG, RegCloseKey, HKEY)
	// for RegOpenKeyEx 


#ifdef _INC_SETUPAPI
MakeCleanupIntClass_winapi(Cec_HDEVINFO, BOOL, SetupDiDestroyDeviceInfoList, HDEVINFO, INVALID_HANDLE_VALUE)
#endif

//MakeCleanupPtrClass_winapi(Cec_HDC_Release, int, ReleaseDC, ); // bad! ReleaseDC need 2 params.


#endif
