#ifndef __CxxVerCheck_h_20250429_
#define __CxxVerCheck_h_20250429_

#if (__cplusplus>=201103L) || (_MSC_VER>=1900) // C++11 or VC2015+
# define CXX11_OR_NEWER
#endif // C++11

#if (__cplusplus>=201402L) || (_MSC_VER>=1900) // C++14 or VC2015+
# define CXX11_OR_NEWER
# define CXX14_OR_NEWER
#endif


#ifdef CXX11_OR_NEWER

#define cxx11_override override

#else

#define cxx11_override

#endif // CXX11_OR_NEWER


#endif
