#include <assert.h>
#include <stdlib.h>
#include <wchar.h>
#include <ps_TCHAR.h>

#include <gadgetlib/T_string.h>

#define DLL_AUTO_EXPORT_STUB
extern"C" void gadgetlib_lib__T_string_msvc__DLL_AUTO_EXPORT_STUB(void){}


__int64 
T_strtoi64(const TCHAR *str, TCHAR **endptr, int base)
{
	__int64 ret = 0;
#if _MSC_VER<=1200 || defined _WIN32_WCE
// VC6 and VC9-WinCE5 does not support _strtoi64(), assert error for now.
	assert(0);
#else 

  #ifdef UNICODE
	ret = _wcstoi64(str, endptr, base);
  #else
	ret = _strtoi64(str, endptr, base);
  #endif

#endif
	return ret;
}

