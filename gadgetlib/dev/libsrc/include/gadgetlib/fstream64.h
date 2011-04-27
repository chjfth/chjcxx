#ifndef __fstream64_h_20080827_
#define __fstream64_h_20080827_

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

#ifndef DLLEXPORT_gadgetlib
# define DLLEXPORT_gadgetlib
#endif
	

DLLEXPORT_gadgetlib
int ggt_fstream64_fseek(FILE *fp, __int64 offset, int origin);

DLLEXPORT_gadgetlib
__int64 ggt_fstream64_ftell(FILE *fp);



#ifdef __cplusplus
} // extern"C" {
#endif




#endif
