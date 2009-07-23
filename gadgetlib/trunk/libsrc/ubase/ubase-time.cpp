#include <time64.h>
#include <uBase.h>

#include <mm_snprintf.h>

#include <gadgetlib/timefuncs.h>

// Partial code from Apache Portable Runtime(APR)


//////////////////////////////////////////////////////////////////////////

ggt_Minute_t 
ggt_LocalTimeZone(void)
{
	return GetTimeZoneOffset(0);
}

__int64 
ggt_time64()
{
	return U_time64();
}

__int64 
ggt_time64_local()
{
	return ggt_time64() - ggt_LocalTimeZone()*60;
}

__int64 
ggt_time64_millisec()
{
	return ggt_time64()*1000; // temp, no millisec accuracy
}

__int64 
ggt_time64_local_millisec()
{
	return ggt_time64_millisec() - ggt_LocalTimeZone()*60*1000;
}

bool 
ggt_gmtime(struct tm *ptm)
{
	__int64 t64 = U_time64();
	if(t64==-1)
		return false;
	
	if(U_gmtime64(t64, ptm)!=0)
		return false;

	return true;
}

bool 
ggt_gmtime_millisec(struct tm *ptm, int *pMillisec)
{
	ggt_gmtime(ptm);
	if(pMillisec)
		*pMillisec = 0; //temp

	return true;
}

bool 
ggt_localtime(struct tm *ptm)
{
	__int64 t64 = U_time64();
	if(t64==-1)
		return false;
	
	if(U_localtime64(t64, ptm)!=0)
		return false;

	return true;
}

bool 
ggt_localtime_millisec(struct tm *ptm, int *pMillisec)
{
/*	bool b = ggt_gmtime_millisec(ptm, pMillisec); 
	if(!b)
		return false;
	
	return true;
*/
	//temp. should use GetClock_msec
	return true;
}

//////////////////////////////////////////////////////////////////////////

bool 
ggt_SleepMillisec(int millisec)
{
	if(millisec<0)
		return false;

	if(millisec==0)
		millisec = 1;

	USleep(millisec);
	return true;
}

__int64 
ggt_GetOsMillisec(void)
{
	return GetTick()*GetOSTickTime();
}

int 
ggt_OsMillisecWrapBits(void)
{
	return 31; // bits;
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
	buf[0] = _T('\0');
	struct tm tmInput; //, *ptm = &tmInput;
	
	int err = U_gmtime64(t64, &tmInput);
	if(err)
		return buf;
	
	ggt_FormatTimeStr_tm(&tmInput, fmt, buf, bufsize);

	return buf;
}


TCHAR *
ggt_FormatTimeStr_Now(int isLocalTime, ggt_Time_et fmt, TCHAR *buf, int bufsize)
{
	bool b = false;
    struct tm tmNow;
	buf[0] = _T('\0');

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

