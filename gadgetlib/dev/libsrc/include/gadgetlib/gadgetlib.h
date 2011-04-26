#ifndef __gadgetlib_h_20110426_
#define __gadgetlib_h_20110426_

// Place your library interface function/type/const declarations here.

#ifdef __cplusplus
extern"C"{
#endif

//#include <gadgetlib/stdio.h>
//#include <gadgetlib/stdlib.h>
//#include <gadgetlib/time.h>

#include <ps_TCHAR.h>

#ifndef DLLEXPORT_gadgetlib
# define DLLEXPORT_gadgetlib
#endif


#include <gadgetlib/ggt-const.h>
#include <gadgetlib/charenccvt.h>
#include <gadgetlib/dirfuncs.h>
#include <gadgetlib/fstream-aux.h>
#include <gadgetlib/fstream64.h>
#include <gadgetlib/gadgetlib.h>
#include <gadgetlib/gsm0338cvt.h>
#include <gadgetlib/mbcs-ops.h>
#include <gadgetlib/timefuncs.h>

DLLEXPORT_gadgetlib
unsigned int gadgetlib_get_version_v3n(void);



#ifdef __cplusplus
} //extern"C"{
#endif

#endif

