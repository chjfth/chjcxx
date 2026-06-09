#ifndef __snTcat_h_
#define __snTcat_h_
#define __snTcat_h_created_ 20260609
#define __snTcat_h_updated_ 20260609

#include <snTprintf.h>

// [2026-06-09] This is the va-arg version of strcat().
// To remedy Linux Glibc snprintf's problem of NOT able to concatenate
// a string using:
//		snprintf(buf, "%s...", buf, ...);
//
// I have to encapsulate vsnTcat() and snTcat() for that purpose.
// So that, snTcat() becomes portable across Windows & Linux.


template<typename TChar>
int vsnTcat(TChar* buf, size_t bufsize, const TChar* fmt, va_list args)
{
	int oldlen = (int)_tcslen(buf);

	int addlen = vsnTprintf(buf + oldlen, bufsize - oldlen, fmt, args);

	return oldlen + addlen;
}

template<typename TChar, size_t bufsize>
int vsnTcat(TChar(&buf)[bufsize], const TChar* fmt, va_list args)
{
	return vsnTcat(buf, bufsize, fmt, args);
}

template<typename TChar>
int snTcat(TChar* buf, size_t bufsize, const TChar* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	int ret = vsnTcat(buf, bufsize, fmt, args);
	va_end(args);
	return ret;
}

template<typename TChar, size_t bufsize>
int snTcat(TChar(&buf)[bufsize], const TChar* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	int ret = vsnTcat(buf, bufsize, fmt, args);
	va_end(args);
	return ret;
}


#endif
