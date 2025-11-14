#ifndef __msvc_extras_h_20251113_
#define __msvc_extras_h_20251113_

// This file brings those useful MSVC header stuff onto Linux.


#define __int64 long long


extern"C++"
template <typename TElement, int N>
char (*
    RtlpNumberOf( TElement (& rparam)[N] )
)[N];

#define ARRAYSIZE(arr)  ( sizeof(*RtlpNumberOf(arr)) )



#endif
