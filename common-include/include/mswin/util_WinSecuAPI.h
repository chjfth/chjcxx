#ifndef __util_WinSecuAPI_h_20200405_
#define __util_WinSecuAPI_h_20200405_

// This C header uses CHIMPL model.

#include <windows.h>
#include <tchar.h>

#ifndef WinError_t
#define WinError_t DWORD
#endif

WinError_t util_EnableWinPriv(const TCHAR *whichPriv);


//////////////////////////////////////////////////////////////////////////
// Implementation code below
//////////////////////////////////////////////////////////////////////////

#ifdef util_WinSecuAPI_IMPL


WinError_t util_EnableWinPriv(const TCHAR *whichPriv) 
{
	WinError_t winerr = 0;
    HANDLE hToken = NULL;
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
        return GetLastError();

	TOKEN_PRIVILEGES tp = {1};
    if (!LookupPrivilegeValue(NULL, whichPriv, &tp.Privileges[0].Luid)) {
		winerr = GetLastError();
        CloseHandle(hToken);
        return winerr;
    }

    tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

    BOOL success = AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(tp), NULL, NULL);
	winerr = GetLastError();
    CloseHandle(hToken);

    return success ? ERROR_SUCCESS : winerr;
}



#endif util_WinSecuAPI_IMPL

#endif
