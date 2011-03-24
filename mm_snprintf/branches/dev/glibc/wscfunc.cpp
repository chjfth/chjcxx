#include <stdio.h>
#include <stdarg.h>
#include <wctype.h>
#include <wchar.h>

#include <ps_Tstrdef.h>


//#define TMM_strchr mmsnpr_wcschr
wchar_t *
mmsnpr_wcschr( const wchar_t *string, wchar_t c )
{
	while(*string!=c && *string!=0)
		string++;

	if(*string==c)
		return (wchar_t*)string;
	else 
		return NULL;
}

//#define TMM_strlen mmsnpr_wcslen
size_t 
mmsnpr_wcslen( const wchar_t *string )
{
	const wchar_t *pstart = string;
	while(*string)
		string++;

	return string - pstart;
}

//#define TMM_isdigit mmsnpr_iswdigit
int mmsnpr_iswdigit( /*wint_t*/int c )
{
	return iswdigit(c);
}

//#define TMM_sprintf mmsnpr_swprintf
int 
mmsnpr_swprintf (wchar_t *sbuf, const wchar_t *fmt, ...) 
{
	va_list args;
	int ret;

	va_start(args, fmt);
	ret = vswprintf(sbuf, 0x7fffFFFF, fmt, args);
	va_end(args);
	return ret;
	
}
