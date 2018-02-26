#define _CRT_SECURE_NO_WARNINGS
#include <string.h>
#include "../mm_psfunc.h"

#if 0 // This file is no longer needed since v5

int 
mmps_i64_type_prefix(char *szPrefix)
{
	strcpy(szPrefix, "I64");
	return 3; // "I64" is 3 characters.
}

wchar_t 
mmps_wsfmt_char()
{
	return L's';
}

#endif