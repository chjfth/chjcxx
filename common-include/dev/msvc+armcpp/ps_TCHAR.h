#ifndef __ps_TCHAR_h_for_msvc_armcpp_20110417_
#define __ps_TCHAR_h_for_msvc_armcpp_20110417_


// NOTE from Chj: You should carefully synchronize the content from msvc/ps_TCHAR.h and armcpp/ps_TCHAR.h .



#ifdef _MSC_VER
//vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
// msvc
//vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv

#ifndef _INC_TCHAR // <tchar.h> defines it. If <tchar.h> has been included, no need to have the code here.

#ifdef UNICODE

#ifndef __T
 #define __T(x)	L##x
#endif

#if _MSC_VER <= 1310  // VC7.1(2003) and earlier
typedef unsigned short TCHAR;
#else //   VC8(2005) and later, wchar_t is a keyword now
typedef wchar_t TCHAR;
#endif

#ifndef _tmain
# define _tmain wmain
#endif


#else	/* ================ Not _UNICODE ================ */


#ifndef __T
#define __T(x)	x
#endif

typedef char TCHAR;

// chj >>>
/*#ifndef _TINT_DEFINED
 typedef int TINT;
 #define _TINT_DEFINED
#endif*/
// chj <<<

#ifndef _tmain
# define _tmain main
#endif


#endif // #ifdef UNICODE .else



/* Generic text macros to be used with string literals and character constants.
   Will also allow symbolic constants that resolve to same. */

#ifndef _T
# define _T(x)       __T(x)
#endif
#ifndef _TEXT
# define _TEXT(x)    __T(x)
#endif


#endif //#ifndef _INC_TCHAR



//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
// msvc
//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
#else
//vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
// armcpp
//vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
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
# define __T(x)	x
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

//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
// armcpp
//^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

#endif



#endif // __ps_TCHAR_h_for_msvc_armcpp_20110417_
