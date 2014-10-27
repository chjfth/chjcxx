#include <stdarg.h>
#include <mm_snprintf.h>

#undef mm_snprintf
#undef mm_vsnprintf

//////////////////////////////////////////////////////////////////////////
// This following two funcs are used for backward binary compatibility,
// i.e, users' binary already linked to mm_snprintf/mm_vsnprintf
// will still get linked well.
// Don't declare these two funcs in mm_snprintf.h, otherwise they'll be
// #define-ed to mm_snprintfA or mm_snprintfW etc.
//////////////////////////////////////////////////////////////////////////
extern"C" int 
mm_snprintf(char *str, size_t str_m, const char *fmt,  ...) 
{
	va_list ap;
	int str_l;

	va_start(ap, fmt);
	str_l = mm_vsnprintfA(str, str_m, fmt, ap);
	va_end(ap);
	return str_l;
}

extern"C" int 
mm_vsnprintf(char *str, size_t str_m, const char *fmt, va_list ap)
{
	return mm_vsnprintfA(str, str_m, fmt, ap);
}
