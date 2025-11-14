#ifndef __snTprintf_h_20251113_
#define __snTprintf_h_20251113_

// snTprintf() is the TCHAR-style function name for C99 snprintf and its WCHAR counterpart.
// This is TCHAR-style function name make me fell comfortable when writing Windows C++ program,
// so I hop to use that symbols as well on Linux.

// Note on Linux/Unix, I do not implement wchar_t version of snTprintf, bcz stupid C99 swprintf()
// family cannot provide the same return-value semantics of snprintf.

#include <stdarg.h>
#include <stdio.h>


inline int vsnTprintf(char* buf, size_t bufsize, const char* fmt, va_list args)
{
	return vsnprintf(buf, bufsize, fmt, args);
}

inline int snTprintf(char* buf, size_t bufsize, const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	int ret = vsnTprintf(buf, bufsize, fmt, args);
	va_end(args);
	return ret;
}

// template for deducing 'bufsize' automatically from char[] array type.
template<size_t bufsize>
int snTprintf(char (&buf)[bufsize], const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	int ret = vsnTprintf(buf, bufsize, fmt, args);
	va_end(args);
	return ret;
}


#endif
