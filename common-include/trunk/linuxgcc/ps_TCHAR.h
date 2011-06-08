#ifndef __ps_TCHAR_h_for_linuxgcc_20110304_
#define __ps_TCHAR_h_for_linuxgcc_20110304_

// IMPORTANT: See msvc/ps_TCHAR.h for important comment of using this header.

#ifdef _UNICODE		/* ================ _UNICODE ================ */

#ifndef __T
 #define __T(x)	L##x
#endif

#ifndef _TCHAR_DEFINED
  #ifndef __cplusplus
  #include <wchar.h>
	// Reason, when compiling a C file, wchar_t seems not to be a keyword, 
	// so we have to include <wchar.h> for wchar_t definition.
	// (verified with gcc 4.1.0)
  #endif
  typedef wchar_t     TCHAR;
  #define _TCHAR_DEFINED
#endif


#ifndef _tmain
# define _tmain wmain //Just write it here, although wmain is not the recognized on Linux
#endif


#else	/* ================ Not _UNICODE ================ */



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
