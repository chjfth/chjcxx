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

	vaDbgTs deals with system clock changing by doing a bias verifying every 10 seconds
	(configurable via vaDbgTs_bias_verify_interval).

	Of course, vaDbgTs does not launch a background thread for bias check, it is only 
	carried out when user calls vaDbgTs().
*/

#include <stdarg.h>

#include <ps_TCHAR.h> // for stramphibian TCHAR

#ifdef __cplusplus
extern"C" {
#endif

unsigned int va_millisec(); 
// -- This function tries to provide real 1 millisecond granularity.
// WinNT's GetTickCount() at most times has only 15.625 millisec precision.
// This should be an ever increasing value, even after user turns system clock backward.

void vaDbgTs(const TCHAR *fmt, ...); // with seq+timestamp prefix
void vlDbgTs(const TCHAR *fmt, va_list args);

void vaDbgS(const TCHAR *fmt, ...);  // with sequence-only prefix
void vlDbgS(const TCHAR *fmt, va_list args);

typedef void PROC_vaDbg_output(const TCHAR *dbgstr, void *ctx);

void vaDbg_set_output(PROC_vaDbg_output proc, void *ctx);

unsigned int vaDbgTs_options(
	unsigned int opts // vaDbg_ymd etc
); // -- return old value

int vaDbgTs_bias_check_interval(int seconds);
// -- return old value

enum {
	vaDbg_seq = 1,      // leading sequence number
	vaDbg_ymd = 2,      // show year, month, date
	vaDbg_millisec = 4, // show millisecond
	vaDbg_diff = 8,     // show diff seconds to previous
	vaDbg_newline = 16, // extra \n at tail

	vaDbg_all = vaDbg_seq | vaDbg_ymd | vaDbg_millisec | vaDbg_diff | vaDbg_newline
};


#ifdef __cplusplus
} // extern"C" {
#endif



///////////////////////////////////////////////////////////////
// Implementation Below:
///////////////////////////////////////////////////////////////

#if defined(vaDbgTs_IMPL) || (defined CHHI_ALL_IMPL && !defined CHHI_ALL_IMPL_HIDE_vaDbgTs)


#include "mswin/vaDbgTs.CHHI.cpp"



#endif // ...CHHI_ALL_IMPL...


#endif
