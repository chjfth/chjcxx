#include "stdafx.h"
#include <sdkddkver.h>

#include <gadgetlib/mswin/oldfuncs_wrapper.h>


#ifdef _WIN32_WINNT_WIN10

#include <versionhelpers.h>

static bool IsWinverOrAbove_newapi(ggt_winver qver)
{
	if (qver == ggt_WIN2000)
	{
			return true;
	}
	else if (qver == ggt_WINXP)
	{
		if (IsWindowsXPOrGreater())
			return true;
	}
	else if (qver == ggt_WINVISTA)
	{
		if (IsWindowsVistaOrGreater())
			return true;
	}
	else if (qver == ggt_WIN7)
	{
		if (IsWindows7OrGreater())
			return true;
	}
	else if (qver == ggt_WIN8)
	{
		if (IsWindows8OrGreater())
			return true;
	}
	else if (qver == ggt_WIN81)
	{
		if (IsWindows8Point1OrGreater())
			return true;

	}
	else if (qver == ggt_WIN10)
	{
		if (IsWindows10OrGreater())
			return true;
	}

	return false;
}

#else

static bool IsWinverOrAbove_oldapi(ggt_winver qver)
{
	OSVERSIONINFO osver = { sizeof(OSVERSIONINFO) };
	GetVersionEx(&osver);

	DWORD m = osver.dwMajorVersion, n = osver.dwMinorVersion;

	if (qver == ggt_WIN2000)
	{
		return true;
	}
	else if (qver == ggt_WINXP)
	{
		if ((m == 5 && n >= 1) || m > 5)
			return true;
	}
	else if (qver == ggt_WINVISTA)
	{
		if (m >= 6)
			return true;
	}
	else if (qver == ggt_WIN7)
	{
		if ((m == 6 && n >= 1) || m > 6)
			return true;
	}
	else if (qver == ggt_WIN8)
	{
		if ((m == 6 && n >= 2) || m > 6)
			return true;
	}
	else if (qver == ggt_WIN81)
	{
		// To get versions equal or above Win81
		// https://docs.microsoft.com/en-us/windows/win32/sysinfo/targeting-your-application-at-windows-8-1
		if ((m == 6 && n >= 3) || m > 6)
			return true;

	}
	else if (qver == ggt_WIN10)
	{
		if (m >= 10)
			return true;
	}

	return false;
}

#endif


DLLEXPORT_gadgetlib
bool ggt_IsWinverOrAbove(ggt_winver qver)
{
	bool ret = false;
#ifdef _WIN32_WINNT_WIN10
	ret = IsWinverOrAbove_newapi(qver);
#else
	ret = IsWinverOrAbove_oldapi(qver);
#endif
	return ret;
}

