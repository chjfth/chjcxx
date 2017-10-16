#include "../mm_psfunc.h"

int 
mmps_i64_type_prefix(char *szPrefix)
{
	szPrefix[0] = 'l'; szPrefix[1] = 'l';
	return 2; // "ll" is 2 characters.
}

wchar_t 
mmps_wsfmt_char()
{
	return L'S';
}
