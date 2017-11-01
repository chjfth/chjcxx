#ifndef __ggt_getch_h_20171101_
#define __ggt_getch_h_20171101_

#ifdef __cplusplus
extern"C" {
#endif

#ifndef DLLEXPORT_gadgetlib
# define DLLEXPORT_gadgetlib
#endif

const int ggt_getch_peek = 0;

DLLEXPORT_gadgetlib
int ggt_getch(int timeout_millisec=-1);
	// Pass ggt_getch_peek to do a peek, i.e. no-wait.
	// -1 means wait infinitely.


#ifdef __cplusplus
} // extern"C" {
#endif

#endif