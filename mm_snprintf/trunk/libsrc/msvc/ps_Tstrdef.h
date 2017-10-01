#ifndef __Tstrdef_h_
#define __Tstrdef_h_

// Configuration macro: USE_ONLY_SBCS,
//	USE_ONLY_SBCS means don't use MBCS functions. Windows kernel don't have MBCS functions like mbschr

#include <tchar.h>

#ifndef USE_ONLY_SBCS // ########################### (the default, MBCS):

#define TMM_strchr _tcschr
#define TMM_strlen(str) ((int)_tcslen(str))

#ifndef WINCE
# define TMM_isdigit _istdigit
#else
# define TMM_isdigit iswdigit // Just always use the wide-char variant on WinCE
#endif
	// Damn M$, hide _istdigit for WinCE app. See NLscan Wiki pageId=17432594 .

#define TMM_snprintf _sntprintf


#else                // ########################### (SBCS):


#ifdef UNICODE
# define TMM_strchr wcschr
# define TMM_strlen(str) ((int)wcslen(str))
# define TMM_isdigit iswdigit
# define TMM_snprintf swprintf
#else
# define TMM_strchr strchr
# define TMM_strlen(str) ((int)strlen(str))
# define TMM_isdigit isdigit
# define TMM_sprintf _snprintf
#endif


#endif               // ###########################


/// >>> special for va_copy macro, MSVC 6,7,8 don't have this macro, but other compiler provides this
#define va_copy(dest, src) ((dest) = (src))
/// <<< special for va_copy macro


#endif
