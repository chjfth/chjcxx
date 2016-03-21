#ifndef __ps_TypeDecl_h_INCLUDED_
#define __ps_TypeDecl_h_INCLUDED_


#if defined(_MSC_VER)     // Visual C++ compiler

// Define 64-bit integer
#define DECL_INT64(n) n##i64
#define DECL_UINT64(n) n##ui64


#elif defined(__GNUC__)   // GNU GCC

// Define 64-bit integer
#define DECL_INT64(n) n##LL
#define DECL_UINT64(n) n##ULL

#endif


#endif
