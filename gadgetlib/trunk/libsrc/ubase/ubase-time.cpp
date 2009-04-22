#include <time.h>
#include <uBase.h>

#include <mm_snprintf.h>

#include <gadgetlib/timefuncs.h>

// Partial code from Apache Portable Runtime(APR)


//////////////////////////////////////////////////////////////////////////

ggt_Minute_t 
ggt_LocalTimeZone(void)
{
	return 0;
}

__int64 
ggt_time64()
{
	return time(NULL);
		// Wish uBase to go time64 some day.
}

__int64 
ggt_time64_local()
{
	return ggt_time64()/* - timezone*/;
}

__int64 
ggt_time64_millisec()
{
	return ggt_time64()*1000; // temp, no millisec accuracy
}

__int64 
ggt_time64_local_millisec()
{
	return ggt_time64_millisec() /*- timezone*1000*/;
}

bool 
ggt_gmtime(struct tm *ptm)
{
	time_t t = time(NULL);
	IrqLock();
	struct tm *pret = gmtime(&t);
	*ptm = *pret;
	IrqUnLock();
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
	return ggt_gmtime(ptm);
}

bool 
ggt_localtime_millisec(struct tm *ptm, int *pMillisec)
{
	return ggt_gmtime_millisec(ptm, pMillisec);
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
	struct tm tmInput;
	time_t etime = (time_t)t64;
	
	IrqLock();
	struct tm * ptm = gmtime(&etime);
	if(ptm)
		tmInput = *ptm;
	IrqUnLock();
	
	if(ptm)
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
