#ifndef __simple_TCHAR_h_
#define __simple_TCHAR_h_


#ifdef LINUX_NO_TCHAR

  // [2024-01-18] on a pristine Linux

  #define __T(x) x
  #define  _T(x) __T(x)

  typedef char TCHAR;

#else

  #include <ps_TCHAR.h>

#endif


#endif
