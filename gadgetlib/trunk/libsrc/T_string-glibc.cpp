//#define __USE_ISOC99 // so that
#include <stdlib.h>

#include <ps_TCHAR.h>
#include <gadgetlib/T_string.h>

#define DLL_AUTO_EXPORT_STUB


__int64 
T_strtoi64(const TCHAR *str, TCHAR **endptr, int base)
{
#ifdef UNICODE
	__int64 ret = wcstoll(str, endptr, base);
#else
	__int64 ret = strtoll(str, endptr, base);
#endif
	return ret;
}

