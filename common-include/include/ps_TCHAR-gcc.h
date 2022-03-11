#ifndef __ps_TCHAR_h_gcc_
#define __ps_TCHAR_h_gcc_
#ifndef __SCALACON_MSVC_MAKING_DUMB_PDB__ // used by this SDK's Makefile.umk

  // NOTE: If you're writing platform-independent code, you should #include <ps_TCHAR.h> instead.

  #ifndef __GNUC__
  	[[This file must be included GNU GCC compiler, but you are using a different compiler.]]
  #endif


// Unicode-or-not >>>>>>>
#if (defined _UNICODE) || (defined UNICODE)

#ifndef __T
 #define __T(x)	L##x
#endif

#ifndef _TCHAR_DEFINED
 typedef wchar_t     TCHAR;
 #define _TCHAR_DEFINED
#endif


//#ifndef _tmain
//# define _tmain wmain // wmain is not recognized on Linux
//#endif


#else	/* ================ Not Unicode ================ */



#ifndef __T
# define __T(x)	x
#endif

#ifndef _TCHAR_DEFINED
 typedef char     TCHAR;
 #define _TCHAR_DEFINED
#endif


//#ifndef _tmain
//# define _tmain main
//#endif


#endif 
// Unicode-or-not <<<<<<<


#endif // __SCALACON_MSVC_MAKING_DUMB_PDB__
#endif // include once guard

