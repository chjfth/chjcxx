#include <unistd.h>
#include <time.h>
#include <sys/time.h>

#include <mm_snprintf.h>

#include <gadgetlib/timefuncs.h>

// Partial code from Apache Portable Runtime(APR)

/* Number of milliseconds between the beginning of the Windows epoch
 * (Jan. 1, 1601) and the Unix epoch (Jan. 1, 1970) 
 */
#define APR_DELTA_EPOCH_IN_MSEC 11644473600000i64


//////////////////////////////////////////////////////////////////////////

ggt_Minute_t 
ggt_LocalTimeZone(void)
{
	return -timezone/60;
}

__int64 
ggt_time64()
{
	return time(NULL);
		// Wish Linux to go time64 some day.
}

__int64 
ggt_time64_local()
{
	return ggt_time64() - timezone;
}

__int64 
ggt_time64_millisec()
{
   	timeval tv_abs;
	gettimeofday(&tv_abs, NULL);
	
	return (__int64)tv_abs.tv_sec*1000 + tv_abs.tv_usec/1000;
}

__int64 
ggt_time64_local_millisec()
{
	return ggt_time64_millisec() - timezone*1000;
}

bool 
ggt_gmtime(struct tm *ptm)
{
	time_t t = time(NULL);
	struct tm *pret = gmtime_r(&t, ptm);
	if(pret)
		return true;
	else
		return false;
}

bool 
ggt_gmtime_millisec(struct tm *ptm, int *pMillisec)
{
   	timeval tv_abs;
	int err = gettimeofday(&tv_abs, NULL);
	if(err)
		return false;
	
	time_t etime = tv_abs.tv_sec;
	if(!gmtime_r(&etime, ptm))
		return false;
	
	if(pMillisec)
		*pMillisec = tv_abs.tv_usec/1000;

	return true;
}

bool 
ggt_localtime(struct tm *ptm)
{
	time_t t = time(NULL);
	struct tm *pret = localtime_r(&t, ptm);
	if(pret)
		return true;
	else
		return false;
}

bool 
ggt_localtime_millisec(struct tm *ptm, int *pMillisec)
{
   	timeval tv_abs;
	int err = gettimeofday(&tv_abs, NULL);
	if(err)
		return false;
	
	time_t etime = tv_abs.tv_sec;
	if(!localtime_r(&etime, ptm))
		return false;
	
	if(pMillisec)
		*pMillisec = tv_abs.tv_usec/1000;

	return true;
}

//////////////////////////////////////////////////////////////////////////

bool 
ggt_SleepMillisec(int millisec)
{
	usleep(millisec*1000);
	return true;
}

__int64 
ggt_GetOsMillisec(void)
{
	struct timespec ts;
	if(clock_gettime(CLOCK_MONOTONIC, &ts) != 0) {
		return 0; // 0 means fail!
	}
	
	__int64 ret = ts.tv_sec*1000 + ts.tv_nsec/1000000;
	return ret==0 ? 1 : ret;
}

int 
ggt_OsMillisecWrapBits(void)
{
	return 32+8; // bits;
		// 8 is arbitrary, you can use +4, +10 or +12, so long as the return value
		// does not exceed 63.
}

//////////////////////////////////////////////////////////////////////////

static TCHAR *
ggt_FormatTimeStr_tm(const struct tm *ptm, ggt_Time_et fmt, TCHAR *buf, int bufsize)
{
	buf[0] = _T('\0');
	if(fmt==ggt_Year)
	{
		mm_snprintf(buf, bufsize, _T("%s%04d-"), buf, ptm->tm_year+1900);
		fmt = ggt_Month;
	}

	if(fmt==ggt_Month)
	{
		mm_snprintf(buf, bufsize, _T("%s%02d-%02d "), buf, ptm->tm_mon+1, ptm->tm_mday);
	}

	mm_snprintf(buf, bufsize, _T("%s%02d:%02d:%02d"), buf,
		ptm->tm_hour, ptm->tm_min, ptm->tm_sec);

	return buf;
}

TCHAR *
ggt_FormatTimeStr(__int64 t64, ggt_Time_et fmt, TCHAR *buf, int bufsize)
{
	struct tm tmInput;
	time_t etime = (time_t)t64;
	
	if(gmtime_r(&etime, &tmInput))
	{
		ggt_FormatTimeStr_tm(&tmInput, fmt, buf, bufsize);
	}
	else
	{
		buf[0] = 0;
	}

	return buf;
}


TCHAR *
ggt_FormatTimeStr_Now(int isLocalTime, ggt_Time_et fmt, TCHAR *buf, int bufsize)
{
	bool b = false;
    struct tm tmNow;
	buf[0] = 0;

	if(isLocalTime)
		b = ggt_localtime(&tmNow);
	else
		b = ggt_gmtime(&tmNow);

	if(b)
	{
		ggt_FormatTimeStr_tm(&tmNow, fmt, buf, bufsize);
	}

	return buf;
}
