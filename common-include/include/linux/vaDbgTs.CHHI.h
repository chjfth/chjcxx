// This is included by parent directory vaDbgTs.h .
// Do not compile this file alone.


#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <syslog.h>

//////////////////////////////// #include done ////////////////////////////////


namespace CHHI_vaDbgTs { // private namespace


unsigned int va_millisec()
{
	struct timespec ts = {};
	if(clock_gettime(CLOCK_MONOTONIC, &ts) != 0) {
		return 0; // 0 means fail!
	}
	
	__int64 ret = ts.tv_sec*1000 + ts.tv_nsec/1000000;
	return ret==0 ? 1 : (Uint)ret;
}


char* now_timestr(vaDbg_opt_et opts, char buf[], int bufchars)
{
	timeval tv_abs = {};
	gettimeofday(&tv_abs, NULL);

	struct tm tm;
	struct tm *ptm = localtime_r(&tv_abs.tv_sec, &tm);

	buf[0] = '['; buf[1] = '\0';

	if (opts & vaDbg_ymd)
	{
		snTprintf(buf+1, bufchars-1, _T("%04d-%02d-%02d_"),     
			tm.tm_year+1900, tm.tm_mon+1, tm.tm_mday);
	}

	int pfxlen = (int)strlen(buf);
	snTprintf(buf+pfxlen, bufchars-pfxlen, _T("%02d:%02d:%02d"),     
		tm.tm_hour, tm.tm_min, tm.tm_sec);

	pfxlen = (int)strlen(buf);
	if (opts & vaDbg_millisec)
	{
		snTprintf(buf+pfxlen, bufchars-pfxlen, _T(".%03d"),     
		        tv_abs.tv_usec/1000);
	}
	
	pfxlen = (int)strlen(buf);
	if (pfxlen < bufchars-1)
	{
		buf[pfxlen] = ']';
		buf[pfxlen+1] = '\0';
	}

	return buf;
}

void default_output_proc(const char *dbgstr, void *ctx)
{
	(void)ctx;
    openlog("vaDbgTs", LOG_PID | LOG_NDELAY, LOG_USER);
    
    syslog(LOG_DEBUG, "%s", dbgstr);
    
    closelog();
}



} // private namespace
