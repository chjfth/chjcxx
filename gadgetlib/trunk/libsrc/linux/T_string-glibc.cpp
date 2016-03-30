#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <wchar.h>

#include <ps_TCHAR.h>
#include <mm_snprintf.h>
#include <gadgetlib/charenccvt.h>
#include <gadgetlib/T_string.h>

#define DLL_AUTO_EXPORT_STUB
extern"C" void gadgetlib_lib__T_string_glibc__DLL_AUTO_EXPORT_STUB(void){}

#define TPRINTF_BUFSIZE 500 // 500 chars



int 
T_printf_query_bufsize(void)
{
	return TPRINTF_BUFSIZE;
}


int 
T_printf_in(bool isStderr, const TCHAR *fmt, va_list args)
{
	TCHAR buf[TPRINTF_BUFSIZE];
	int ret = mm_vsnprintf(buf, TPRINTF_BUFSIZE, fmt, args);

#ifdef UNICODE
	// convert to utf8 then call printf.
	char bytebuf[TPRINTF_BUFSIZE*3];
	ggt_encode_to_utf8(buf, -1, bytebuf, sizeof(bytebuf));
	bytebuf[sizeof(bytebuf)-1] = '\0';
#else
	char *bytebuf = buf;
#endif

	if(isStderr) {
		fprintf(stderr, "%s", bytebuf);
		fflush(stderr);
	}
	else {
		printf("%s", bytebuf);
		fflush(stdout);
	}
	return ret;
}

int 
T_printf(const TCHAR *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
	int ret = T_printf_in(false, fmt, args);
	va_end(args);
	return ret;
}

int 
T_printf_stderr(const TCHAR *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
	int ret = T_printf_in(true, fmt, args);
	va_end(args);
	return ret;
}



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

