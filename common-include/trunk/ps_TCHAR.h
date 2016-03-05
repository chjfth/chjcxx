#ifndef __ps_TCHAR_h_20160305_
#define __ps_TCHAR_h_20160305_

/*!
	This header file is intended to be included by any source file(.h or .cpp)
 that needs to be stramphibian-compiled.
	
	The word "stramphibian" is just my invention which means string-amphibian.
 A source file is said to be stramphibian if the char-type is not explicitly
 expressed with char or wchar_t, but expressed in a typedef-ed name: TCHAR.
	
	If TCHAR is typedef-ed to char, then the chars or strings in the source
 file becomes ANSI-chars or strings.
	If TCHAR is typedef-ed to wchar_t then the chars or strings in the source
 file becomes Unicode-chars or strings.

Characteristics of this ps_TCHAR.h file:

	1. For source files who use(#include) it, ps_TCHAR.h appears to be 
 compiler independent and target-platform-independent. It achieves this by
 checking compiler-vendor macros(_MSC_VER, __GNUC__ etc) and target-OS macros 
 (_WIN32 inside _MSC_VER, __linux__ inside __GNUC__ etc).

	2. This header will try to introduce no pollution to its user, which means:
 although a ps_TCHAR.h itself is platform-specific, the ps_TCHAR.h should not 
 #include any platform specific headers.
	For example, ps_TCHAR.h for Visual C++ should not include TCHAR.H provided by 
 Microsoft Visual C++, since TCHAR.H will introduce quite a lot symbol pollution
 such as: 
	<pre>
	#define _tcsclen    strlen
	#define _tcsnccat   strncat
	...
	#define _PUC    unsigned char *
	#define _CPUC   const unsigned char *
	#define _PC     char *
	</pre>
	Of course, ``#include <windows.h>`` will introduce further mass pollution.
	The user of ps_TCHAR.h must not be force to receive those pollution!
	There may be a misunderstanding that goes: Since the user knows his code
 is to be compiled into a Windows EXE file(using Visual C++), he should be 
 ready to define his symbols(#defines, typedefs, functions) carefully 
 to not conflict with those of the system's. This theory seems quite a bit
 reasonable, but it is not always what I want(or what I want the users to comply with).
 Imagine that you've designed a large set of API wrapping most of Windows
 functions, and you want the user to write a Windows EXE all dependent on your
 API, thus the user need to include only your API's header files to see your
 API's function prototype, const definition etc, but not those of Windows(the user
 can rely just on your API's documentation to do his work, not that of Windows API's). 
 In this case should the user be allowed to define a macro of _PUC ? Of course
 he should be allowed, because _PUC is not defined by your API(from the point
 of the user, _PUC is not documented in your API's documentation). Therefore,
 you should not [ include M$-provided TCHAR.h in ps_TCHAR.h to make _PUC a 
 pollution.

	3. Although ps_TCHAR.h is not dependent on platform-specific headers(called
 system headers as well), it should coexist with them. For the implementation
 source file of a [ (platform independent) API which needs ps_TCHAR.h to have 
 TCHAR type defined ], this is required, because the implementation file will
 have to include ps_TCHAR.h as well as system's headers to accomplish the
 API's implementation. Therefore, if something in this ps_TCHAR.h have the
 same name as those of the system's, conflict-avoiding action should be taken,
 e.g. for TCHAR type definition on Windows(VC++), you should write here:
	<pre>
	#ifndef _TCHAR_DEFINED
	 typedef wchar_t     TCHAR;
	 #define _TCHAR_DEFINED
	#endif
	</pre>
	That is, testing and defining _TCHAR_DEFINED makes a second definition
 of TCHAR (no matter it is ps_TCHAR.h or TCHAR.h) peaceful.
	To accomplish this point, the system headers do have to cooperate, one 
 typical scenario is: it should associate a typedef-ed name with a macro
 name, such as _TCHAR_DEFINED for TCHAR, thus we can know from _TCHAR_DEFINED 
 whether TCHAR has been typedef-ed.

	We guarantee co-existence when ps_TCHAR.h is #included *after* system headers,
 but cannot guarantee it with reversed #include order.

*/

#if defined(_MSC_VER)     // Visual C++ compiler

 #include "ps_TCHAR-msvc.h"

#elif defined(__GNUC__)   // GNU GCC

 #include "ps_TCHAR-gcc.h"

#endif

// __T should have been defined in above ps_TCHAR-xxx.h files.
#ifndef _T
# define _T(x)       __T(x)
#endif
#ifndef _TEXT
# define _TEXT(x)    __T(x)
#endif


#endif // include once guard
