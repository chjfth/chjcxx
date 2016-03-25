#define _CRT_SECURE_NO_WARNINGS // to disable warning on VC++ (have to put it before sys headers)

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

#include <ps_TCHAR.h>
#include <mm_snprintf.h>
#include <gadgetlib/T_string.h>

#define DLL_AUTO_EXPORT_STUB
extern"C" void gadgetlib_lib__T_string__DLL_AUTO_EXPORT_STUB(void){}

#define TPRINTF_BUFSIZE 500 // 500 chars


int 
T_printf(const TCHAR *fmt, ...)
{
	TCHAR buf[TPRINTF_BUFSIZE];
    va_list args;
    va_start(args, fmt);
	int ret = mm_vsnprintf(buf, TPRINTF_BUFSIZE, fmt, args);

#ifdef UNICODE
 #ifdef __linux__
	wprintf(L"%S", buf); // upper-case S
 #else
	wprintf(L"%s", buf); // lower-case S
 #endif
#else
	printf("%s", buf);
#endif

	va_end(args);
	return ret;
}

int 
T_printf_stderr(const TCHAR *fmt, ...)
{
	TCHAR buf[TPRINTF_BUFSIZE];
    va_list args;
    va_start(args, fmt);
	int ret = mm_vsnprintf(buf, TPRINTF_BUFSIZE, fmt, args);

#ifdef UNICODE
 #ifdef __linux__
	fwprintf(stderr, L"%S", buf); // upper-case S
 #else
	fwprintf(stderr, L"%s", buf); // lower-case S
 #endif
#else
	fprintf(stderr, "%s", buf);
#endif

	va_end(args);
	return ret;
}

int 
T_printf_query_bufsize(void)
{
	return TPRINTF_BUFSIZE;
}


int 
T_strlen(const TCHAR *str)
{
#ifdef UNICODE
	int ret = (int)wcslen(str);
#else
	int ret = (int)strlen(str);
#endif
	return ret;
}


TCHAR * 
T_strcpy(TCHAR *dst, const TCHAR *src)
{
#ifdef UNICODE
	TCHAR * ret = wcscpy(dst, src);
#else
	TCHAR * ret = strcpy(dst, src);
#endif
	return ret;
}	


TCHAR * 
T_strncpy(TCHAR *dst, const TCHAR *src, int count)
{
#ifdef UNICODE
	TCHAR * ret = wcsncpy(dst, src, count);
#else
	TCHAR * ret = strncpy(dst, src, count);
#endif
	return ret;
}


int 
T_strcmp(const TCHAR *str1, const TCHAR *str2)
{
#ifdef UNICODE
	int ret = wcscmp(str1, str2);
#else
	int ret = strcmp(str1, str2);
#endif
	return ret;
}


TCHAR * 
T_strstr(const TCHAR *str1, const TCHAR *str2)
{
#ifdef UNICODE
	return (TCHAR*)wcsstr(str1, str2);
#else
	return (TCHAR*)strstr(str1, str2);
#endif
}

TCHAR * 
T_strchr(const TCHAR *str, TCHAR chr)
{
#ifdef UNICODE
	return (TCHAR*)wcschr(str, chr);
#else
	return (TCHAR*)strchr(str, chr);
#endif
}


//DLLEXPORT_gadgetlib
//int T_printf wprintf // set-aside due to %s/%S difference between Windows and GLibc


//// Functions from stdlib.h

int 
T_atoi(const TCHAR *str)
{
	return T_strtoi(str, NULL, 10);
}

__int64 
T_atoi64(const TCHAR *str)
{
	return T_strtoi64(str, NULL, 10);
}


int 
T_strtoi(const TCHAR *str, TCHAR **endptr, int base)
{
#ifdef UNICODE
	int ret = wcstol(str, endptr, base);
#else
	int ret = strtol(str, endptr, base);
#endif
	return ret;
}
