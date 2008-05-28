#ifndef __Tstrdef_h_
#define __Tstrdef_h_

//#include <tchar.h> glibc don't have this.

#ifdef _UNICODE

	#define TMM_strchr mmsnpr_wcschr
	#define TMM_strlen mmsnpr_wcslen
	#define TMM_isdigit mmsnpr_iswdigit

	#define TMM_sprintf mmsnpr_swprintf

#else

	#define TMM_strchr strchr
	#define TMM_strlen strlen
	#define TMM_isdigit isdigit

	#define TMM_sprintf sprintf

#endif



#endif
