#ifndef __fstream_aux_h_20080827_
#define __fstream_aux_h_20080827_

#ifdef __cplusplus
extern"C" {
#else
#define bool int; // sly
#endif


#include <stdio.h>

// struct FILE; // cause VC6 compile error if <stdio.h> is included later : 
//	error C2371: 'FILE' : redefinition; different basic types
// VC6 CRT used: 
//	typedef struct _iobuf FILE;

DLLEXPORT_gadgetlib
bool ggt_fstream_change_file_size(FILE *fp, int newsize);
	// [2008-08-27]
	// Since the ANSI(C99) file stream operation has not standardized functions
	// supporting 64-bit file length(e.g. no fseek64 for FILE*) , I just use 
	// int instead of __int64.

#ifdef __cplusplus
} // extern"C" {
#endif




#endif
