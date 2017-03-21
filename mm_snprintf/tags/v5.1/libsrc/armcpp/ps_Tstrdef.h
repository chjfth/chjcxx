#ifndef __Tstrdef_h_
#define __Tstrdef_h_

//#include <tchar.h> glibc don't have this.
#include <stddef.h> // for size_t

#ifdef _UNICODE

	#define TMM_strchr mmsnpr_wcschr
	#define TMM_strlen mmsnpr_wcslen
	#define TMM_isdigit mmsnpr_iswdigit

	#define TMM_sprintf mmsnpr_swprintf

wchar_t *mmsnpr_wcschr( const wchar_t *string, wchar_t c );
size_t mmsnpr_wcslen( const wchar_t *string );
int mmsnpr_iswdigit( int c );
int mmsnpr_swprintf (wchar_t *sbuf, const wchar_t *fmt, ...);

#else

	#define TMM_strchr strchr
	#define TMM_strlen strlen
	#define TMM_isdigit isdigit

	#define TMM_sprintf sprintf

#endif

/// >>> special for va_copy macro 
// ARM ADS 1.2 and onward already has it in <stdarg.h> .
// But ARM SDT 2.50 sys header doesn't have it, so we define it for ARM SDT 2.50.
#ifdef __CC_NORCROFT // __CC_NORCROFT is a pre-defined macro of ARM SDT 2.50
  #define va_copy(dest, src) (*(dest) = *(src))
#endif
/// <<< special for va_copy macro

#endif
