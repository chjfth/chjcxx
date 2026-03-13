#ifndef __CHHI__util_WinSecuAPI_h_20200405_
#define __CHHI__util_WinSecuAPI_h_20200405_

// This C header uses CHIMPL model.

#include <windows.h>
#include <tchar.h>

#ifndef WinError_t
#define WinError_t DWORD
#endif

WinError_t util_EnableWinPriv(const TCHAR *whichPriv);


/*
////////////////////////////////////////////////////////////////////////////
 ___                 _                           _        _   _             
|_ _|_ __ ___  _ __ | | ___ _ __ ___   ___ _ __ | |_ __ _| |_(_) ___  _ __  
 | || '_ ` _ \| '_ \| |/ _ \ '_ ` _ \ / _ \ '_ \| __/ _` | __| |/ _ \| '_ \ 
 | || | | | | | |_) | |  __/ | | | | |  __/ | | | || (_| | |_| | (_) | | | |
|___|_| |_| |_| .__/|_|\___|_| |_| |_|\___|_| |_|\__\__,_|\__|_|\___/|_| |_|
              |_|                                                           
////////////////////////////////////////////////////////////////////////////
*/
// ++++++++++++++++++ Implementation Below ++++++++++++++++++
//

// >>> Include headers required by this lib's implementation
// ...
// <<< Include headers required by this lib's implementation


#if defined(util_WinSecuAPI_IMPL) || (defined CHHI_ALL_IMPL && !defined CHHI_ALL_IMPL_HIDE_util_WinSecuAPI) // [IMPL]


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



#endif // [IMPL]

#endif // include once guard
