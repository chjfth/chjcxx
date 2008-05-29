#ifndef __ps_TCHAR_h_for_linux_20060109_
#define __ps_TCHAR_h_for_linux_20060109_

// IMPORTANT: See mswin/ps_TCHAR.h for important comment of using this header.

#ifdef _UNICODE		/* ================ _UNICODE ================ */

#ifndef __T
 #define __T(x)	L##x
#endif

#ifndef _TCHAR_DEFINED
 typedef wchar_t     TCHAR;
 #define _TCHAR_DEFINED
#endif



#else	/* ================ Not _UNICODE ================ */



#ifndef __T
# define __T(x)	(x)
#endif

#ifndef _TCHAR_DEFINED
 typedef char     TCHAR;
 #define _TCHAR_DEFINED
#endif


#endif // #ifdef _UNICODE .else



/* Generic text macros to be used with string literals and character constants.
   Will also allow symbolic constants that resolve to same. */

#define _T(x)       __T(x)
#define _TEXT(x)    __T(x)





#endif
