#ifndef __ggt_simple_thread_h_
#define __ggt_simple_thread_h_

#ifdef __cplusplus
extern"C" {
#endif


#ifndef DLLEXPORT_gadgetlib
#define DLLEXPORT_gadgetlib
#endif

typedef void *ggt_hsimplethread;
typedef void (*PROC_ggt_simple_thread)(void *param);
	// Note: On x86 Windows, this defaults to __cdecl, not __stdcall .

DLLEXPORT_gadgetlib
ggt_hsimplethread ggt_simple_thread_create(PROC_ggt_simple_thread proc, void *param);
	// return non-NULL handle on thread creation success.

DLLEXPORT_gadgetlib
bool ggt_simple_thread_waitend(ggt_hsimplethread h);


#ifdef __cplusplus
} // extern"C" {
#endif

#endif
