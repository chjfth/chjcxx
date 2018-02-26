#include "../mm_psfunc.h"

#if 0 // This file is no longer needed since v5

int 
mmps_i64_type_prefix(char *szPrefix)
{
	szPrefix[0] = 'l'; szPrefix[1] = 'l';
	return 2; // "ll" is 2 characters.
}

#ifndef MMSNPRINTF_NO_UNICODE

wchar_t 
mmps_wsfmt_char()
{
	return L'S';
}

#endif

#endif