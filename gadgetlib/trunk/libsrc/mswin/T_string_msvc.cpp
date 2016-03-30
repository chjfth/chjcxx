#define _CRT_SECURE_NO_WARNINGS // to disable warning on VC++ (have to put it before sys headers)

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <tchar.h>
#include <wchar.h>

#include <ps_TCHAR.h>
#include <mm_snprintf.h>
#include <gadgetlib/T_string.h>

#define DLL_AUTO_EXPORT_STUB

#define TPRINTF_BUFSIZE 500 // 500 chars

// bool g_setlocale_done = false;

int 
T_printf_query_bufsize(void)
{
	return TPRINTF_BUFSIZE;
}


int 
T_printf(const TCHAR *fmt, ...)
{
	TCHAR buf[TPRINTF_BUFSIZE];
    va_list args;
    va_start(args, fmt);
	int ret = mm_vsnprintf(buf, TPRINTF_BUFSIZE, fmt, args);

#ifndef WINCE
	_wsetlocale(LC_ALL, L"");
		// in order for Unicode characters to be shown on CMD window.
#endif

	_tprintf(_T("%s"), buf);

	va_end(args);

	fflush(stdout);
	return ret;
}

int 
T_printf_stderr(const TCHAR *fmt, ...)
{
	TCHAR buf[TPRINTF_BUFSIZE];
    va_list args;
    va_start(args, fmt);
	int ret = mm_vsnprintf(buf, TPRINTF_BUFSIZE, fmt, args);

#ifndef WINCE
	_wsetlocale(LC_ALL, L"");
		// in order for Unicode characters to be shown on CMD window.
#endif

	_ftprintf(stderr, _T("%s"), buf);

	va_end(args);

	fflush(stderr);
	return ret;
}


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

