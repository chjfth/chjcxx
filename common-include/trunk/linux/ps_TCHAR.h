#ifndef __ps_TCHAR_h_for_linux_20060109_
#define __ps_TCHAR_h_for_linux_20060109_

// IMPORTANT: See mswin/ps_TCHAR.h for important comment of using this header.

#if (defined _UNICODE) || (defined UNICODE)

#ifndef __T
 #define __T(x)	L##x
#endif

#ifndef _TCHAR_DEFINED
 typedef wchar_t     TCHAR;
 #define _TCHAR_DEFINED
#endif


#ifndef _tmain
# define _tmain wmain //Just write it here, although wmain is not the recognized on Linux
#endif


#else	/* ================ Not Unicode ================ */



#ifndef __T
# define __T(x)	x
#endif

#ifndef _TCHAR_DEFINED
 typedef char     TCHAR;
 #define _TCHAR_DEFINED
#endif


#ifndef _tmain
# define _tmain main
#endif


#endif // #ifdef _UNICODE .else



/* Generic text macros to be used with string literals and character constants.
   Will also allow symbolic constants that resolve to same. */

#define _T(x)       __T(x)
#define _TEXT(x)    __T(x)





#endif
