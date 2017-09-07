#include "mm_psfunc.h"
#include <mm_snprintf.h>

const int mbufsize = 4000;

int mm_printfA(const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);

	char buf[mbufsize];
	int ret = mm_vsnprintfA(buf, mbufsize, fmt, args);

	va_end(args);

	printf("%s", buf);
	return ret;
}

#ifndef DONT_HAVE_wprintf

int mm_printfW(const wchar_t *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	
	wchar_t buf[mbufsize];
	int ret = mm_vsnprintfW(buf, mbufsize, fmt, args);
	
	va_end(args);
	
	wprintf(mmps_wsfmt(), buf);
	return ret;
}

#endif
