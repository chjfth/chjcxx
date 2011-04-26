#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <mm_snprintf.h>

#include <gadgetlib/timefuncs.h>

// Partial code from Apache Portable Runtime(APR)

/* Number of milliseconds between the beginning of the Windows epoch
 * (Jan. 1, 1601) and the Unix epoch (Jan. 1, 1970) 
 */
#define APR_DELTA_EPOCH_IN_MSEC 11644473600000i64

static __int64 
FileTime_to_int64_millisec(const FILETIME *input)
{
	__int64 result;

	result = input->dwHighDateTime;
	result = result << 32;
	result |= input->dwLowDateTime;
	result /= 10000;    /* Convert from 100 nano-sec periods to milliseconds. */
	result -= APR_DELTA_EPOCH_IN_MSEC;  /* Convert from Windows epoch to Unix epoch */

	return result;
}

static void 
Winsystime_to_Ansi_tm(SYSTEMTIME *pst, struct tm *ptm, int *pMillisec=0)
{
    static const int dayoffset[12] =
    {0, 31, 59, 90, 120, 151, 182, 212, 243, 273, 304, 334};

    /* Note; the caller is responsible for filling in detailed tm_usec,
     * tm_gmtoff and tm_isdst data when applicable.
     */
    ptm->tm_sec  = pst->wSecond;
    ptm->tm_min  = pst->wMinute;
    ptm->tm_hour = pst->wHour;
    ptm->tm_mday = pst->wDay;
    ptm->tm_mon  = pst->wMonth - 1;
    ptm->tm_year = pst->wYear - 1900;
    ptm->tm_wday = pst->wDayOfWeek;
    ptm->tm_yday = dayoffset[ptm->tm_mon] + (pst->wDay - 1);
    ptm->tm_isdst = 0;

    /* If this is a leap year, and we're past the 28th of Feb. (the
     * 58th day after Jan. 1), we'll increment our tm_yday by one.
     */
    if (ggt_IsLeapYear(pst->wYear) && (ptm->tm_yday > 58))
        ptm->tm_yday++;

	if(pMillisec)
		*pMillisec = pst->wMilliseconds;
}

//////////////////////////////////////////////////////////////////////////

ggt_Minute_t 
ggt_LocalTimeZone(void)
{
	TIME_ZONE_INFORMATION tzi = {0};
	GetTimeZoneInformation(&tzi);

	return -tzi.Bias;
}

__int64 
ggt_time64()
{
	return ggt_time64_millisec()/1000;
}

__int64 
ggt_time64_local()
{
	return ggt_time64_local_millisec()/1000;
}

__int64 
ggt_time64_millisec()
{
    FILETIME time;

#ifndef _WIN32_WCE
    GetSystemTimeAsFileTime(&time);
#else
    SYSTEMTIME st;
    GetSystemTime(&st);
    SystemTimeToFileTime(&st, &time);
#endif
	return FileTime_to_int64_millisec(&time);
}

__int64 
ggt_time64_local_millisec()
{
    FILETIME time;

    SYSTEMTIME st;
    GetLocalTime(&st);
    SystemTimeToFileTime(&st, &time);

	return FileTime_to_int64_millisec(&time);
}

bool 
ggt_gmtime(struct tm *ptm)
{
	return ggt_gmtime_millisec(ptm, NULL);
}

bool 
ggt_gmtime_millisec(struct tm *ptm, int *pMillisec)
{
    SYSTEMTIME st;
    GetSystemTime(&st);

	Winsystime_to_Ansi_tm(&st, ptm, pMillisec);

	return true;
}

bool 
ggt_localtime(struct tm *ptm)
{
	return ggt_localtime_millisec(ptm, NULL);
}

bool 
ggt_localtime_millisec(struct tm *ptm, int *pMillisec)
{
    SYSTEMTIME st;
    GetLocalTime(&st);

	Winsystime_to_Ansi_tm(&st, ptm, pMillisec);

	return true;
}

//////////////////////////////////////////////////////////////////////////

bool 
ggt_SleepMillisec(int millisec)
{
	Sleep(millisec);
	return true;
}

__int64 
ggt_GetOsMillisec(void)
{
	return GetTickCount();
}

int 
ggt_OsMillisecWrapBits(void)
{
	return 32; // bits
}

//////////////////////////////////////////////////////////////////////////

static TCHAR *
ggt_FormatTimeStr_Winsystime(const SYSTEMTIME &st, ggt_Time_et fmt, TCHAR *buf, int bufsize)
{
	buf[0] = _T('\0');
	if(fmt==ggt_Year)
	{
		mm_snprintf(buf, bufsize, _T("%s%04d-"), buf, st.wYear);
		fmt = ggt_Month;
	}

	if(fmt==ggt_Month)
	{
		mm_snprintf(buf, bufsize, _T("%s%02d-%02d "), buf, st.wMonth, st.wDay);
	}

	mm_snprintf(buf, bufsize, _T("%s%02d:%02d:%02d"), buf,
		st.wHour, st.wMinute, st.wSecond);

	return buf;
}

TCHAR *
ggt_FormatTimeStr(__int64 t64, ggt_Time_et fmt, TCHAR *buf, int bufsize)
{
	t64 *= 1000*10000; // translate sec to 100 nano-sec.
	FILETIME ft = { (DWORD)t64, (DWORD)(t64>>32) };
	SYSTEMTIME st;
	FileTimeToSystemTime(&ft, &st);

	ggt_FormatTimeStr_Winsystime(st, fmt, buf, bufsize);
	return buf;
}


TCHAR *
ggt_FormatTimeStr_Now(int isLocalTime, ggt_Time_et fmt, TCHAR *buf, int bufsize)
{
    SYSTEMTIME st;
	if(isLocalTime)
		GetLocalTime(&st);
	else
		GetSystemTime(&st);

	return ggt_FormatTimeStr_Winsystime(st, fmt, buf, bufsize);
}
