#ifndef __snTprintf_h_
#define __snTprintf_h_
#define __snTprintf_h_created_ 20251113
#define __snTprintf_h_updated_ 20260711
	
// snTprintf() is the TCHAR-style function name for C99 snprintf and its WCHAR counterpart.
// This is TCHAR-style function name make me feel comfortable when writing Windows C++ program,
// so I hop to use that symbols as well on Linux.

// Note on Linux/Unix, I do not implement wchar_t version of snTprintf, bcz stupid C99 swprintf()
// family cannot provide the same return-value semantics of snprintf.

#include <stdarg.h>
#include <stdio.h>

// Note: I deliberately use (signed) int as bufsize's type.
// Unsigned bufsize behaves badly, see Evclip [20260711.c1].

inline int vsnTprintf(char* buf, int bufsize, const char* fmt, va_list args)
{
	if(bufsize<0)
		bufsize = 0;
		
	return vsnprintf(buf, bufsize, fmt, args);
}

inline int snTprintf(char* buf, int bufsize, const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	int ret = vsnTprintf(buf, bufsize, fmt, args);
	va_end(args);
	return ret;
}

// template for deducing 'bufsize' automatically from char[] array type.
template<int bufsize>
int snTprintf(char (&buf)[bufsize], const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	int ret = vsnTprintf(buf, bufsize, fmt, args);
	va_end(args);
	return ret;
}


#endif
