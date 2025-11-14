// This is included by parent directory vaDbgTs.h .
// Do not compile this file alone.


#include <windows.h>
#include <shellapi.h>
#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>

#include <commdefs.h>

//////////////////////////////// #include done ////////////////////////////////

#ifndef WinMultiMon_DEBUG
#include <CHHI_vaDBG_hide.h>
#endif

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

	_sntprintf_s(buf, _TRUNCATE, _T("{%04d-%02d-%02d_%02d:%02d:%02d}ivaDbg: "),
		st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);

	OutputDebugString(buf);

	_vsntprintf_s(buf, _TRUNCATE, fmt, args);

	OutputDebugString(buf);
	OutputDebugString(_T("\n"));
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


const int msec_bias_tolerant = 1000;

int g_msec_check_interval = 10000;

const int DBG_BUFCHARS = 16000;

static int g_dbgcount = 0;

unsigned int g_opts = vaDbg_all;

PROC_vaDbg_output *g_vadbg_output = default_output_proc;
void *g_ctx_output = 0;


void vaDbg_set_output(PROC_vaDbg_output proc, void *ctx)
{
	g_vadbg_output = proc;
	g_ctx_output = ctx;
}

TCHAR* SystimeToString(const SYSTEMTIME &st, TCHAR buf[], int bufchars)
{
	buf[0] = '['; buf[1] = '\0';

	if (g_opts & vaDbg_ymd)
	{
		_sntprintf_s(buf, bufchars, _TRUNCATE, _T("%s%04d-%02d-%02d_"), buf,
			st.wYear, st.wMonth, st.wDay);
	}

	_sntprintf_s(buf, bufchars, _TRUNCATE, _T("%s%02d:%02d:%02d"), buf,
		st.wHour, st.wMinute, st.wSecond);

	if (g_opts & vaDbg_millisec)
	{
		_sntprintf_s(buf, bufchars, _TRUNCATE, _T("%s.%03d"), buf,
			st.wMilliseconds);
	}

	_sntprintf_s(buf, bufchars, _TRUNCATE, _T("%s]"), buf);

	return buf;
}


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

	TCHAR* NowTimeStr(TCHAR buf[], int bufchars)
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
				
		return SystimeToString(st, buf, bufchars);
	}
};

SAccuMillisec g_accums;

unsigned int vaDbgTs_options(unsigned int opts)
{
	Uint oldval = g_opts;
	g_opts = opts;
	return oldval;
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



void vlDbgTs(const TCHAR *fmt, va_list args)
{
	// Note: Each calling outputs one line, with timestamp prefix.
	// A '\n' will be added automatically at end.

	static DWORD s_prev_msec = va_millisec();

	DWORD now_msec = va_millisec();

	TCHAR buf[DBG_BUFCHARS] = {0};

	// Print timestamp to show that time has elapsed for more than one second.
	DWORD delta_msec = now_msec - s_prev_msec;
	if(delta_msec>=1000)
	{
		g_vadbg_output(_T(".\n"), g_ctx_output);
	}

	TCHAR timebuf[40] = {};
	g_accums.NowTimeStr(timebuf, ARRAYSIZE(timebuf));

	if(g_opts & vaDbg_diff)
	{
		_sntprintf_s(buf, _TRUNCATE, _T("[%d]%s (+%3u.%03us) "),
			++g_dbgcount,
			timebuf,
			delta_msec / 1000, delta_msec % 1000);
	}

	int prefixlen = (int)_tcslen(buf);

	_vsntprintf_s(buf+prefixlen, ARRAYSIZE(buf)-3-prefixlen, _TRUNCATE, fmt, args);

	if(g_opts & vaDbg_newline)
	{
		// add trailing \n
		int slen = (int)_tcslen(buf);
		if(slen==ARRAYSIZE(buf)-1)
			--slen;

		buf[slen] = '\n';
		buf[slen+1] = '\0';
	}

	g_vadbg_output(buf, g_ctx_output);

	s_prev_msec = now_msec;
}


void vlDbgS(const TCHAR *fmt, va_list args)
{
	// This only has Sequential prefix.

	TCHAR buf[DBG_BUFCHARS] = {0};

	_sntprintf_s(buf, ARRAYSIZE(buf)-3, _TRUNCATE, TEXT("[%d] "), ++g_dbgcount); // prefix seq

	int prefixlen = (int)_tcslen(buf);

	_vsntprintf_s(buf+prefixlen, ARRAYSIZE(buf)-3-prefixlen, _TRUNCATE, fmt, args);

	if(g_opts & vaDbg_newline)
	{
		// add trailing \n
		int slen = (int)_tcslen(buf);
		if(slen==ARRAYSIZE(buf)-1)
			--slen;

		buf[slen] = '\n';
		buf[slen+1] = '\0';

	}

	g_vadbg_output(buf, g_ctx_output);
}


} // private namespace

////////////////////////////////////////////////////////////////////////////////// 
//////////////////////////////// public API below ////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

unsigned int va_millisec()
{
	return CHHI_vaDbgTs::va_millisec();
}

void vaDbgTs(const TCHAR *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	CHHI_vaDbgTs::vlDbgTs(fmt, args);
	va_end(args);
}

void vlDbgTs(const TCHAR *fmt, va_list args)
{
	CHHI_vaDbgTs::vlDbgTs(fmt, args);
}

void vaDbgS(const TCHAR *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	CHHI_vaDbgTs::vlDbgS(fmt, args);
	va_end(args);
}

void vlDbgS(const TCHAR *fmt, va_list args)
{
	CHHI_vaDbgTs::vlDbgS(fmt, args);
}


unsigned int vaDbgTs_options(unsigned int opts)
{
	return CHHI_vaDbgTs::vaDbgTs_options(opts);
}

int vaDbgTs_bias_check_interval(int seconds)
{
	return CHHI_vaDbgTs::vaDbgTs_bias_check_interval(seconds);
}

void vaDbg_set_output(PROC_vaDbg_output proc, void *ctx)
{
	CHHI_vaDbgTs::vaDbg_set_output(proc, ctx);
}

#ifndef WinMultiMon_DEBUG
#include <CHHI_vaDBG_show.h>
#endif
