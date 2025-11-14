#ifndef __snTprintf_h_20250628_
#define __snTprintf_h_20250628_

// snTprintf() is the TCHAR-style function name for C99 snprintf and its WCHAR counterpart.
// This is C++ header(not C header), usable on VC2010+.

#include <stdarg.h>
#include <stdio.h>
#include <tchar.h>

#ifdef _MSC_VER
#ifndef va_copy
// VC2010 lacks va_copy, so manual supply it
#define va_copy(destination, source) ((destination) = (source))
#endif
#endif 

inline int vsnTprintf(wchar_t* buf, size_t bufsize, const wchar_t* fmt, va_list args)
{
	va_list args_copy;
	va_copy(args_copy, args); // save a copy for 2nd-used

	int ret = _vsnwprintf_s(buf, bufsize, _TRUNCATE, fmt, args); // truncate if needed

	if (ret < 0)
	{
		// ret is -1 if bufsize not enough.
		// To conform to C99 snprintf's return value, we need to calculate bufsize required.

		ret = _vscwprintf(fmt, args_copy); // count number of characters needed
	}

	va_end(args_copy); // pair with va_copy()
	return ret;
}

inline int vsnTprintf(char* buf, size_t bufsize, const char* fmt, va_list args)
{
	va_list args_copy;
	va_copy(args_copy, args); // save a copy for 2nd-used

	int ret = _vsnprintf_s(buf, bufsize, _TRUNCATE, fmt, args); // truncate if needed

	if (ret < 0)
	{
		// ret is -1 if bufsize not enough.
		// To conform to C99 snprintf's return value, we need to calculate bufsize required.

		ret = _vscprintf(fmt, args_copy); // count number of characters needed
	}

	va_end(args_copy); // pair with va_copy()
	return ret;
}

template<typename TChar>
int snTprintf(TChar* buf, size_t bufsize, const TChar* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	int ret = vsnTprintf(buf, bufsize, fmt, args);
	va_end(args);
	return ret;
}

template<typename TChar, size_t bufsize>
int snTprintf(TChar(&buf)[bufsize], const TChar* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	int ret = vsnTprintf(buf, bufsize, fmt, args);
	va_end(args);
	return ret;
}


#endif
