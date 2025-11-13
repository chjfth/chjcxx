#ifndef __vaDbgTs_h_20251113_
#define __vaDbgTs_h_20251113_

#include <stdarg.h>

#include <ps_TCHAR.h> // for stramphibian TCHAR

#ifdef __cplusplus
extern"C" {
#endif

unsigned int va_millisec(); 
// -- This function tries to provide real 1 millisecond granularity.
// WinNT's GetTickCount() at most times has only 15.625 millisec precision.

bool vaDbgTs_NeedYmd(bool ymd);
// -- return old value

void vaDbgTs(const TCHAR *fmt, ...); // with seq+timestamp prefix
void vlDbgTs(const TCHAR *fmt, va_list args);

void vaDbgS(const TCHAR *fmt, ...);  // with sequence-only prefix
void vlDbgS(const TCHAR *fmt, va_list args);

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
