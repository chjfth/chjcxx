#ifndef __msvc_extras_h_
#define __msvc_extras_h_
#define __msvc_extras_h_created_ 20260404
#define __msvc_extras_h_updated_ 20260526

// This file brings those useful MSVC header stuff onto Linux.

#ifndef __int64
#define __int64 long long
#endif


extern"C++"
template <typename TElement, int N>
char (*
    RtlpNumberOf( TElement (& rparam)[N] )
)[N];

#define ARRAYSIZE(arr)  ( sizeof(*RtlpNumberOf(arr)) )


// [2026-05-26]
#define MSVCRT_MemCheckStart(varprefix) // empty
#define MSVCRT_MemCheckEnd_IsLeak(varprefix) false


/////////////////////////////////////////////////////////////////////////////
// Override some MSVC stramphibian CRT function names. (wrapped in !_UNOCODE)
/////////////////////////////////////////////////////////////////////////////
#if !defined(_UNICODE)

#define _tmain       main
#define _tprintf     printf

#define _tcslen      strlen
#define _tcscmp      strcmp
#define _tcsncmp     strncmp
#define _tcsstr      strstr
#define _tcschr      strchr
#define _tcstol      strtol
#define _tcstoul     strtoul
#define _ttof        atof


#endif // !defined(_UNICODE)


#endif
