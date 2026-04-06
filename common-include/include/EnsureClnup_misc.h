#ifndef __EnsureClnup_misc_h_
#define __EnsureClnup_misc_h_
#define __EnsureClnup_misc_h_created_ 20260406

#include <stdio.h>
#include <stdlib.h>
#include "EnsureClnup.h"

// For `` FILE* ''
// inline int fclose_FILE_ptr(FILE *fp){ return fclose(fp); }
MakeDelega_CleanupPtr(CEC_fclose, int, fclose, FILE*)


#ifdef __CHHI__fsapi_h_

MakeDelega_CleanupAny(CEC_filehandle_t, \
	fsapi::fserror_et, fsapi::file_close, fsapi::filehandle_t, fsapi::bad_filehandle)

#endif // __CHHI__fsapi_h_


#endif
