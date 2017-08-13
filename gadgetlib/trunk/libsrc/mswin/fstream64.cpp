#include "stdafx.h"

#include <gadgetlib/fstream64.h>

#define DLL_AUTO_EXPORT_STUB

extern"C" int __cdecl _fseeki64(FILE *, __int64, int);
extern"C" __int64 __cdecl _ftelli64(FILE *);
	// The above two are declared in MS CRT source INTERNEL.H


int 
ggt_fstream64_fseek(FILE *fp, __int64 offset, int origin)
{
	return _fseeki64(fp, offset, origin);
}

__int64 
ggt_fstream64_ftell(FILE *fp)
{
	return _ftelli64(fp);
}

