#include <stdlib.h>
#include <wchar.h>
#include <ps_TCHAR.h>

#include <gadgetlib/T_string.h>

#define DLL_AUTO_EXPORT_STUB


__int64 
T_strtoi64(const TCHAR *str, TCHAR **endptr, int base)
{
#ifdef UNICODE
	__int64 ret = _wcstoi64(str, endptr, base);
#else
	__int64 ret = _strtoi64(str, endptr, base);
#endif
	return ret;
}

