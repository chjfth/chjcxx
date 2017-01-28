#ifndef __ENSURECLNUP_MSWIN_H
#define __ENSURECLNUP_MSWIN_H

#include <EnsureClnup.h>


MakeCleanupPtrClass_winapi(Cec_PTRHANDLE, BOOL, CloseHandle, HANDLE);
	// This is for CreateEvent,OpenProcess, who return NULL as fail.
MakeCleanupIntClass_winapi(Cec_FILEHANDLE, BOOL, CloseHandle, HANDLE, INVALID_HANDLE_VALUE); 
	// This is for CreateFile, who returns INVALID_HANDLE_VALUE as fail. Damn M$.

#ifdef _INC_SETUPAPI
MakeCleanupIntClass_winapi(Cec_HDEVINFO, BOOL, SetupDiDestroyDeviceInfoList, HDEVINFO, INVALID_HANDLE_VALUE); 
#endif

//MakeCleanupPtrClass_winapi(Cec_HDC_Release, int, ReleaseDC, ); // bad! ReleaseDC need 2 params.


#endif
