#ifndef __ps_TCHAR_h_msvc_
#define __ps_TCHAR_h_msvc_

  // NOTE: If you're writing platform-independent code, you should #include <ps_TCHAR.h> instead.

  #ifndef _MSC_VER
  	[[This file must be included Visual C++ compiler, but you are using a different compiler.]]
  #endif

#ifndef _INC_TCHAR // <tchar.h> defines it. If <tchar.h> has been included, no need to have the code here.

#if _MSC_VER <= 1310  // VC7.1(2003) and earlier
typedef unsigned short wchar_t;
	// So that wchar_t can be used without including <tchar.h>
#endif


// Unicode-or-not >>>>>>>
#if (defined _UNICODE) || (defined UNICODE)

typedef wchar_t TCHAR;

#ifndef __T
 #define __T(x)	L##x
#endif

#ifndef _tmain
# define _tmain wmain
#endif


#else	/* ================ Not Unicode ================ */

typedef char TCHAR;

#ifndef __T
#define __T(x)	x
#endif

#ifndef _tmain
# define _tmain main
#endif


#endif // #ifdef UNICODE .else
// Unicode-or-not <<<<<<<



#endif //#ifndef _INC_TCHAR

#endif // include once guard

