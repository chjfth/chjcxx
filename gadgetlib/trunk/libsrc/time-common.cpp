#include "stdafx.h"

#include <gadgetlib/T_string.h>
#include <gadgetlib/timefuncs.h>

#define DLL_AUTO_EXPORT_STUB
extern"C" void gadgetlib_lib__time_common__DLL_AUTO_EXPORT_STUB(void){}

__int64 
ggt_OsMillisecElapsed(__int64 old_msec, __int64 new_msec)
{
	int wrapbit = ggt_OsMillisecWrapBits();

	__int64 delta = new_msec - old_msec;

	delta &= ((__int64)1<<wrapbit)-1;
	
	return delta;
}

int
ggt_IsLeapYear(int y)
{
	// Leap year is any year divisible by four, 
	// but not by 100 unless also divisible by 400

	return y%4==0 ? ((y%400==0 || y%100!=0) ? 1 : 0) : 0;
}


static int mmF_localtime_str(void *param, TCHAR *buf, int bufsize)
{
	PrnTs_et tsmode = *(PrnTs_et*)param;
	int ideallen = 0;

	if(tsmode!=PrnNoTs)
	{
		struct tm tmnow;
		TCHAR sz_msec[8] = {0};
		if(tsmode==PrnMsec)
		{
			int ms_part = 0;
			ggt_localtime_now_millisec(&tmnow, &ms_part);
			mm_snprintf(sz_msec, GetEleQuan_i(sz_msec), _T(".%03d"), ms_part);
		}
		else
			ggt_localtime_now(&tmnow);

		ideallen = mm_snprintf(buf, bufsize, _T("[%d-%02d-%02d_%02d:%02d:%02d%s]"),
			tmnow.tm_year+1900, tmnow.tm_mon+1, tmnow.tm_mday,
			tmnow.tm_hour, tmnow.tm_min, tmnow.tm_sec, 
			sz_msec);
	}
	
	return ideallen;
}


int 
T_printf_wall(PrnTs_et ts, const TCHAR *fmt, ...)
{
	va_list args;
	va_start(args, fmt);

	int ret = T_printf(_T("%F%w\n"), 
		MM_FPAIR_PARAM(mmF_localtime_str, &ts), 
		MM_WPAIR_PARAM(fmt, args)
		);

	va_end(args);
	return ret;
}

int 
T_printfe_wall(PrnTs_et ts, const TCHAR *fmt, ...)
{
	va_list args;
	va_start(args, fmt);

	int ret = T_printfe(_T("%F%w\n"), 
		MM_FPAIR_PARAM(mmF_localtime_str, &ts), 
		MM_WPAIR_PARAM(fmt, args)
		);

	va_end(args);
	return ret;
}
