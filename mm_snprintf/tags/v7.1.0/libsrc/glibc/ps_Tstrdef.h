#ifndef __Tstrdef_h_
#define __Tstrdef_h_

//#include <tchar.h> glibc don't have this.
#include <stddef.h> // for size_t

#ifdef UNICODE // Simple matters. UNICODE is simpler than _UNICODE

    #include <wchar.h>
    #include <wctype.h>
    
	#define TMM_strchr wcschr
	#define TMM_strlen wcslen
	#define TMM_isdigit iswdigit

	#define TMM_snprintf swprintf

//wchar_t *mmsnpr_wcschr( const wchar_t *string, wchar_t c );
//size_t mmsnpr_wcslen( const wchar_t *string );
//int mmsnpr_iswdigit( int c );
//int mmsnpr_swprintf (wchar_t *sbuf, const wchar_t *fmt, ...);

#else

	#define TMM_strchr strchr
	#define TMM_strlen strlen
	#define TMM_isdigit isdigit

	#define TMM_snprintf snprintf

#endif



#endif
