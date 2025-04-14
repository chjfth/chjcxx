#ifndef __EnsureClnup_mswin_h_20250305_
#define __EnsureClnup_mswin_h_20250305_

#include <windows.h>

#include "EnsureClnup.h"

// [2025-03-05] Delega class naming convention: 
// For this global header file, class name should start with CEC_ (3 capital letters).
//
// BTW: For a Delega class defined in local project header, it should start with Cec_ .

MakeDelega_CleanupCxxPtrN(TCHAR, CEC_delete_TCHAR)


MakeDelega_CleanupPtr_winapi(CEC_PTRHANDLE, BOOL, CloseHandle, HANDLE)
	// This is for CreateEvent,OpenProcess, who return NULL as fail.
MakeDelega_CleanupAny_winapi(CEC_FILEHANDLE, BOOL, CloseHandle, HANDLE, INVALID_HANDLE_VALUE)
	// This is for CreateFile, who returns INVALID_HANDLE_VALUE as fail. Damn M$.
	// More of this: CreateToolhelp32Snapshot

MakeDelega_CleanupPtr_winapi(CEC_RegCloseKey, LONG, RegCloseKey, HKEY)
	// for RegOpenKeyEx 

MakeDelega_CleanupPtr_winapi(CEC_DeleteDC, BOOL, DeleteDC, HDC)
	// for CreateCompatibleDC, CreateDC

MakeDelega_CleanupPtr_winapi(CEC_DestroyMenu, BOOL, DestroyMenu, HMENU)
	// for CreatePopupMenu
MakeDelega_CleanupPtr_winapi(CEC_DestroyIcon, BOOL, DestroyIcon, HICON)
	// for CreateIconIndirect, CopyIcon, SHGetStockIconInfo 

MakeDelega_CleanupPtr_winapi(CEC_LocalFree, HLOCAL, LocalFree, HLOCAL)
	// for ConvertSidToStringSid, ConvertStringSidToSid 

MakeDelega_CleanupPtr_winapi(CEC_FreeSid, PVOID, FreeSid, PSID)
	// for AllocateAndInitializeSid

#ifdef _NTSECAPI_ // if NTSecAPI.h is included
MakeDelega_CleanupPtr_winapi(CEC_LsaFreeReturnBuffer, NTSTATUS, LsaFreeReturnBuffer, PVOID)
	// for LsaEnumerateLogonSessions, LsaGetLogonSessionData (NTSecAPI.h)
#endif

// SetupDi... functions:

#ifdef _INC_SETUPAPI // #include <setupapi.h>
MakeDelega_CleanupAny_winapi(CEC_HDEVINFO, BOOL, SetupDiDestroyDeviceInfoList, HDEVINFO, INVALID_HANDLE_VALUE)
MakeDelega_CleanupAny_winapi(CEC_SetupDiHKEY, LONG, RegCloseKey, HKEY, (HKEY)INVALID_HANDLE_VALUE)
	// for SetupDiOpenDevRegKey
	// Note: We need to write '(HKEY)INVALID_HANDLE_VALUE', otherwise we get VS2010 cl.exe error:
	// error C2440: 'specialization' : cannot convert from 'HANDLE' to 'HKEY'
#endif

#ifdef _LMAPIBUF_ // LMAPIBuf.h
MakeCleanupPtrClass_winapi(CEC_NetApiBufferFree, NET_API_STATUS, NetApiBufferFree, void*)
#endif

//MakeDelega_CleanupPtr_winapi(CEC_HDC_Release, int, ReleaseDC, ); // Bad! ReleaseDC needs 2 params.


#endif // __EnsureClnup_mswin_h_
