#ifndef __vaDbgTs_h_20251113_
#define __vaDbgTs_h_20251113_

/* 
[Purpose]

	vaDbgTs() provides a debug output line with timestamp prefix that is accurate to
	one millisecond. A sample output line like this:

	[1][2025-11-14_10:48:32.644] (+  0.000s) ...
	[2][2025-11-14_10:48:33.685] (+  1.041s) ...
	[3][2025-11-14_10:48:34.717] (+  1.031s) ...

[Implementation notes]
	
	On Windows GetSystemTime() and GetLocalTime() can not fulfill bcz they have only
	a PCMildTick(15.625 millisec) accuracy. So I need the help of QueryPerformanceCounter()
	for millisec reference.

	On Windows, vaDbgTs deals with system clock drifting by doing a bias verifying every 
	5 seconds. (configurable via vaDbgTs_bias_check_interval).

	Of course, vaDbgTs does not launch a background thread for bias check, it is only 
	carried out when user calls vaDbgTs().
	
	snTprintf note: This file contains snprintf calling code shared by MSVC & GCC.
	But, Linux's snprintf(eg Ubuntu 20.04, gcc 9.4) lacks the ability to append a buffer 
	with more substrings. So avoid doing that.
	.
	The following code prints "123456789" on MSVC, but not on GCC:
	
	    const int size = 20;
	    char buf[size] = "123";
	    snprintf(buf, size, "%s%d", buf, 456);
	    snprintf(buf, size, "%s%d", buf, 789);
	   
	    printf("%s\n", buf);
*/

#include <stdio.h>
#include <stdarg.h>

#include <ps_TCHAR.h> // for stramphibian TCHAR

#ifdef __cplusplus
extern"C" {
#endif

unsigned int va_millisec(); 
// -- This function tries to provide real 1 millisecond granularity.
// WinNT's GetTickCount() at most times has only 15.625 millisec precision.
// This should be an ever increasing value, even after user turns system clock backward.

void vaDbgTs(const TCHAR *fmt, ...);
void vlDbgTs(const TCHAR *fmt, va_list args);


enum vaDbg_level_et
{
	vaDbg_ERROR = 0,
	vaDbg_WARN = 1,
	vaDbg_INFO = 2,
	vaDbg_VERBOSE = 3,

	vaDbg_DOTS = -1
};

void vaDbgTsl(vaDbg_level_et lvl, const TCHAR *fmt, ...);
void vlDbgTsl(vaDbg_level_et lvl, const TCHAR *fmt, va_list args);


enum vaDbg_opt_et {
	vaDbg_seq = 1,      // leading sequence number
	vaDbg_ymd = 2,      // show year, month, date
	vaDbg_millisec = 4, // show millisecond
	vaDbg_diff = 8,     // show diff seconds to previous
	vaDbg_newline = 16, // extra \n at tail

	vaDbg_all = vaDbg_seq | vaDbg_ymd | vaDbg_millisec | vaDbg_diff | vaDbg_newline
};

unsigned int vaDbgTs_options(
	vaDbg_opt_et opts // vaDbg_ymd etc
	); // -- return old value


typedef void PROC_vaDbg_output(vaDbg_level_et lvl, const TCHAR *dbgstr, void *ctx);
//
void vaDbg_set_output(PROC_vaDbg_output proc, void *ctx);


typedef int PROC_vsnprintf(TCHAR *buf, size_t bufchars, const TCHAR *fmt, va_list args);
//
PROC_vsnprintf* vaDbg_set_vsnprintf(PROC_vsnprintf custom_vsnp);

int vaDbgTs_bias_check_interval(int seconds);
// -- return old value
// -- This is only for Windows, debugging purpose.

int vaDbgTs_set_seq_width(int width);
// Default is 1. If set to 3, the seq prefix "[1]" will be printed as "[  1]".
// -- return old value

#ifdef __cplusplus
} // extern"C" {
#endif



///////////////////////////////////////////////////////////////
// Implementation Below: (private namespace 'CHHI_vaDbgTs')
///////////////////////////////////////////////////////////////

#if defined(vaDbgTs_IMPL) || (defined CHHI_ALL_IMPL && !defined CHHI_ALL_IMPL_HIDE_vaDbgTs)

#ifdef _MSC_VER 
#else 
#include <linux/msvc_extras.h> // for __int64
#define _tcslen strlen
#endif


#include <commdefs.h> // For Uint, Uint64, enum bitwise-OR
#include <snTprintf.h>


#ifdef _MSC_VER 
// Using Microsoft Visual C++ as compiler
#include "mswin/vaDbgTs.CHHI.h"
#else 
// Consider it Linux/GCC
#include "linux/vaDbgTs.CHHI.h"
#endif

namespace CHHI_vaDbgTs { // private namespace

TCHAR* now_timestr(vaDbg_opt_et opts, TCHAR buf[], int bufchars);
// -- Mswin/Linux code must implement this function to complete vaDbgTs() codeflow.
// The return string should be like:
//		"[2025-11-14_15:20:44.510]"
// but varies according to opts.

const int DBG_BUFCHARS = 16000;

int g_dbgcount = 0;

vaDbg_opt_et g_opts = vaDbg_all;

PROC_vaDbg_output *g_vadbg_output = default_output_proc;
void *g_ctx_output = 0;

PROC_vsnprintf *g_custom_vsnprintf = NULL;

int g_seq_width = 1; 


unsigned int vaDbgTs_options(vaDbg_opt_et opts)
{
	Uint oldval = g_opts;
	g_opts = opts;
	return oldval;
}

void vaDbg_set_output(PROC_vaDbg_output proc, void *ctx)
{
	g_vadbg_output = proc;
	g_ctx_output = ctx;
}

int vaDbgTs_set_seq_width(int width)
{
	int oldval = g_seq_width;
	g_seq_width = (width >= 1 && width <= 8)? width : 1;
	return oldval;
}

PROC_vsnprintf* vaDbg_set_vsnprintf(PROC_vsnprintf custom_vsnp)
{
	PROC_vsnprintf *oldfn = g_custom_vsnprintf;
	g_custom_vsnprintf = custom_vsnp;
	return oldfn;
}

void vlDbgTsl(vaDbg_level_et lvl, const TCHAR *fmt, va_list args)
{
	// Note: Each calling outputs one line, with timestamp prefix.
	// A '\n' will be added automatically at end.

	static Uint s_prev_msec = va_millisec();

	Uint now_msec = va_millisec();

	TCHAR buf[DBG_BUFCHARS] = {0};

	// Print extra dot-line to show that time has elapsed for more than one second.
	Uint delta_msec = now_msec - s_prev_msec;
	if(delta_msec>=1000)
	{
		TCHAR dots[12] = _T("..........\n");
		Uint delta_sec = delta_msec / 1000;
		if(delta_sec>10)
			delta_sec = 10;

		dots[delta_sec] = '\n';
		dots[delta_sec+1] = '\0';
		g_vadbg_output(vaDbg_DOTS, dots, g_ctx_output);
	}

	TCHAR timebuf[60] = {};
	now_timestr(g_opts, timebuf, ARRAYSIZE(timebuf));

	int prefixlen = 0;

	int opts = g_opts;
	if(opts & vaDbg_seq)
	{
		snTprintf(buf, _T("[%*d]%s "), g_seq_width, ++g_dbgcount, timebuf);
	}
	else
	{
		snTprintf(buf, _T("%s "), ++g_dbgcount,	timebuf);
	}

	if(opts & vaDbg_diff)
	{
		prefixlen = (int)_tcslen(buf);

		snTprintf(buf+prefixlen, ARRAYSIZE(buf)-prefixlen, _T("(+%3u.%03us) "),
			delta_msec / 1000, delta_msec % 1000);
	}

	prefixlen = (int)_tcslen(buf);

	if(g_custom_vsnprintf)
		g_custom_vsnprintf(buf+prefixlen, ARRAYSIZE(buf)-3-prefixlen, fmt, args);
	else
		vsnTprintf        (buf+prefixlen, ARRAYSIZE(buf)-3-prefixlen, fmt, args);

	if(opts & vaDbg_newline)
	{
		// add trailing \n
		int slen = (int)_tcslen(buf);
		if(slen==ARRAYSIZE(buf)-1)
			--slen;

		buf[slen] = '\n';
		buf[slen+1] = '\0';
	}

	g_vadbg_output(lvl, buf, g_ctx_output);

	s_prev_msec = now_msec;
}



} // private namespace

////////////////////////////////////////////////////////////////////////////////// 
//////////////////////////////// public API below ////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

unsigned int va_millisec()
{
	return CHHI_vaDbgTs::va_millisec();
}

void vlDbgTsl(vaDbg_level_et lvl, const TCHAR *fmt, va_list args)
{
	CHHI_vaDbgTs::vlDbgTsl(lvl, fmt, args);
}

void vaDbgTs(const TCHAR *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	CHHI_vaDbgTs::vlDbgTsl(vaDbg_VERBOSE, fmt, args);
	va_end(args);
}

void vlDbgTs(const TCHAR *fmt, va_list args)
{
	CHHI_vaDbgTs::vlDbgTsl(vaDbg_VERBOSE, fmt, args);
}

void vaDbgTsl(vaDbg_level_et lvl, const TCHAR *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	CHHI_vaDbgTs::vlDbgTsl(lvl, fmt, args);
	va_end(args);
}

void vlDbgTs(vaDbg_level_et lvl, const TCHAR *fmt, va_list args)
{
	CHHI_vaDbgTs::vlDbgTsl(vaDbg_VERBOSE, fmt, args);
}


unsigned int vaDbgTs_options(vaDbg_opt_et opts)
{
	return CHHI_vaDbgTs::vaDbgTs_options(opts);
}


void vaDbg_set_output(PROC_vaDbg_output proc, void *ctx)
{
	CHHI_vaDbgTs::vaDbg_set_output(proc, ctx);
}

int vaDbgTs_set_seq_width(int width)
{
	return CHHI_vaDbgTs::vaDbgTs_set_seq_width(width);
}

PROC_vsnprintf* vaDbg_set_vsnprintf(PROC_vsnprintf custom_vsnp)
{
	return CHHI_vaDbgTs::vaDbg_set_vsnprintf(custom_vsnp);
}


#endif // ...CHHI_ALL_IMPL...


#endif
