#ifndef __msvc_extras_h_20251113_20260404_
#define __msvc_extras_h_20251113_20260404_

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

/////////////////////////////////////////////////////////////////////////////
// Override some MSVC stramphibian CRT function names. (wrapped in !_UNOCODE)
/////////////////////////////////////////////////////////////////////////////
#if !defined(_UNICODE)

#define _tprintf     printf

#endif // !defined(_UNICODE)


#endif
