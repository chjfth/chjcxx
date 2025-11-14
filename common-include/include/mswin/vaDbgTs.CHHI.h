// This is included by parent directory vaDbgTs.h .
// Do not compile this file alone.


#include <windows.h>
#include <shellapi.h>
#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>

//////////////////////////////// #include done ////////////////////////////////


namespace CHHI_vaDbgTs { // private namespace


inline Uint64 get_qpf()
{
	LARGE_INTEGER li = {};
	BOOL succ = QueryPerformanceFrequency(&li);
	if(!succ)
		return -1;
	else
		return li.QuadPart;
}

inline Uint64 get_qpc()
{
	LARGE_INTEGER li = {};
	BOOL succ = QueryPerformanceCounter(&li);
	if(!succ)
		return -1;
	else
		return li.QuadPart;
}

unsigned int va_millisec()
{
	// In vaDbgTs libcode, we'll use this function an more accurate GetTickCount().
	// 32bit is enough bcz user should care for the diff of two va_millisec() calls.

	Uint64 s_qpf = get_qpf(); // To play it safe, not `static`

	// We should use QueryPerformanceCounter(), 
	// bcz GetTickCount() only has 15.625 ms resolution.
	DWORD millisec = DWORD(get_qpc()*1000 / s_qpf);
	return millisec;
}

const Uint64 msec_epoch_diff = (11644473600ULL * 1000); // diff in Windows-Unix epoch

Uint64 SystimeToUemsec()
{
	SYSTEMTIME st = {};
	FILETIME ft = {};
	GetSystemTime(&st);

	SystemTimeToFileTime(&st, &ft);
	ULARGE_INTEGER ft64;
	ft64.HighPart = ft.dwHighDateTime;
	ft64.LowPart = ft.dwLowDateTime;

	Uint64 ret = ft64.QuadPart / 10000 - msec_epoch_diff; // result in millisec
	return ret;
}

SYSTEMTIME UemsecToLocalTime(Uint64 uemsec)
{
	LARGE_INTEGER ft64;
	ft64.QuadPart = (uemsec + msec_epoch_diff) * 10000; // millisec to 100ns

	FILETIME ftUtc = {}, ftLocal = {};
	ftUtc.dwHighDateTime = ft64.HighPart;
	ftUtc.dwLowDateTime = ft64.LowPart;

	FileTimeToLocalFileTime(&ftUtc, &ftLocal);

	SYSTEMTIME stLocal = {};
	FileTimeToSystemTime(&ftLocal, &stLocal);
	return stLocal;
}

void ivlDbgTs(const TCHAR *fmt, va_list args) // internal debug interface
{
	TCHAR buf[200];

	SYSTEMTIME st = {};
	GetLocalTime(&st);

	snTprintf(buf, _T("{%04d-%02d-%02d_%02d:%02d:%02d}ivaDbg: "),
		st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

	int pfxlen = _tcslen(buf);

	vsnTprintf(buf+pfxlen, ARRAYSIZE(buf)-pfxlen, fmt, args);
	
	pfxlen = _tcslen(buf);
	if(pfxlen == ARRAYSIZE(buf)-1)
		--pfxlen;
	
	buf[pfxlen] = '\n';
	buf[pfxlen+1] = '\0';
	
	OutputDebugString(buf);
}

void ivaDbgTs1(const TCHAR *fmt, ...) // internal debug interface
{
#ifdef vaDbgTs_DEBUG1
	va_list args;
	va_start(args, fmt);
	ivlDbgTs(fmt, args);
	va_end(args);
#else
	(void)fmt;
#endif
}

void ivaDbgTs2(const TCHAR *fmt, ...) // internal debug interface
{
#ifdef vaDbgTs_DEBUG2
	va_list args;
	va_start(args, fmt);
	ivlDbgTs(fmt, args);
	va_end(args);
#else
	(void)fmt;
#endif
}

void default_output_proc(const TCHAR *dbgstr, void *ctx)
{
	(void)ctx;
	OutputDebugString(dbgstr);
}


TCHAR* SystimeToString(vaDbg_opt_et opts, const SYSTEMTIME &st, TCHAR buf[], int bufchars)
{
	buf[0] = '['; buf[1] = '\0';

	if (opts & vaDbg_ymd)
	{
		snTprintf(buf, bufchars, _T("%s%04d-%02d-%02d_"), buf,
			st.wYear, st.wMonth, st.wDay);
	}

	snTprintf(buf, bufchars, _T("%s%02d:%02d:%02d"), buf,
		st.wHour, st.wMinute, st.wSecond);

	if (opts & vaDbg_millisec)
	{
		snTprintf(buf, bufchars, _T("%s.%03d"), buf,
			st.wMilliseconds);
	}

	snTprintf(buf, bufchars, _T("%s]"), buf);

	return buf;
}

const int msec_bias_tolerant = 1000;

int g_msec_check_interval = 10000;


struct SAccuMillisec
{
	// nomenclature:
	// uemsec: millisec since unix-epoch(1970-01-01), imply a static value
	// mt: millisec ticks reported by va_millisec().

	Uint64 uemsec_base;
	Uint mt_base;

	Uint mt_last_check; // previous check time for clock bias

	SAccuMillisec()	{ Reset(); }

	void Reset()
	{
		uemsec_base = SystimeToUemsec();
		mt_base = va_millisec();

		mt_last_check = mt_base;

		ivaDbgTs1(_T("Reset() done. uemsec_base=%I64u, mt_base=%u"), uemsec_base, mt_base);
	}

	TCHAR* NowTimeStr(vaDbg_opt_et opts, TCHAR buf[], int bufchars)
	{
		// This function deals with system clock bias overtime. Cases are
		// * User changes system clock suddenly.
		// * A VM paused, waited 1 hour then resumed, would cause QPC value diverges
		//   from system clock. See mynote [20250906.c1].
		// * Current va_millisec() overflows and wraps to 0.

		bool is_reset = false;
		Uint mt_now = va_millisec();
		Uint64 uemsec_now = uemsec_base + (mt_now - mt_base);

		int check_delayed = mt_now - mt_last_check;

//		ivaDbgTs2(_T("==== check_delayed=%d , ckin=%d"), check_delayed, g_msec_check_interval);

		if (abs(check_delayed)>=g_msec_check_interval)
		{
			// Check for sysclock bias

			Uint64 uemsec_by_systime = SystimeToUemsec();

			int uemsec_diff = int(uemsec_now - uemsec_by_systime);
			if (abs(uemsec_diff) >= msec_bias_tolerant)
			{
				ivaDbgTs2(_T("check close bias, result: %d (abs>%d), need Reset()"), uemsec_diff, msec_bias_tolerant);

				is_reset = true;
				Reset();
			}
			else
			{
				ivaDbgTs2(_T("check close bias, result: %d (abs<%d), still ok"), uemsec_diff, msec_bias_tolerant);
			}

			mt_last_check = mt_now;
		}

		if(is_reset)
		{ 
			// In case Reset() may delay some time, we'd better refresh mt_now.
			mt_now = mt_base;
			uemsec_now = uemsec_base;
		}

		// Generate YMD_HMS.millisec via uemsec_now

		SYSTEMTIME st = UemsecToLocalTime(uemsec_now);

		buf[0] = '['; buf[1] = '\0';
				
		return SystimeToString(opts, st, buf, bufchars);
	}
};

TCHAR* now_timestr(vaDbg_opt_et opts, TCHAR buf[], int bufchars)
{
	static SAccuMillisec g_accums;
	return g_accums.NowTimeStr(opts, buf, bufchars);
}

int vaDbgTs_bias_check_interval(int seconds)
{
	int oldval = g_msec_check_interval / 1000;

	if(seconds>0)
	{ 
		g_msec_check_interval = seconds * 1000;
	}

	return oldval;
}


} // private namespace


int vaDbgTs_bias_check_interval(int seconds)
{
	return CHHI_vaDbgTs::vaDbgTs_bias_check_interval(seconds);
}
