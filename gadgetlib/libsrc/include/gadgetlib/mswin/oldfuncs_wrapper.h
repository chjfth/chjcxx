#ifndef __oldfuncs_wrapper_h_20220401_
#define __oldfuncs_wrapper_h_20220401_

#include <gadgetlib/ggt-mswin.h>

#ifdef __cplusplus
extern"C"{
#endif

#ifndef DLLEXPORT_gadgetlib
# define DLLEXPORT_gadgetlib
#endif

enum ggt_winver
{
	// same constants from _WIN32_WINNT_WIN2K, _WIN32_WINNT_WINXP etc
	ggt_WIN2000 = 0x0500,
	ggt_WINXP = 0x0501,
	ggt_WINSERVER2003 = 0x0502,
	ggt_WINVISTA = 0x0600,
	ggt_WIN7 = 0x0601,
	ggt_WIN8 = 0x0602,
	ggt_WIN81 = 0x0603,
	ggt_WIN10 = 0x0A00,
};

DLLEXPORT_gadgetlib
bool ggt_IsWinverOrAbove(ggt_winver qver);


	
#ifdef __cplusplus
} // extern"C"{
#endif

#endif
