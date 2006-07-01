#ifndef __ps_TCHAR_h_for_mswin_20060107_
#define __ps_TCHAR_h_for_mswin_20060107_

/*!
	This header file is intended to be included by any source file(.h or .cpp)
 that needs to be stramphibian-compiled.
	
	The word "stramphibian" is just my invention which means string-amphibian.
 A source file is said to be stramphibian if the char-type is not explicitly
 expressed with \c char or \c wchar_t, but expressed in a typedef-ed name: TCHAR.
	
	If TCHAR is typedef-ed to \c char, then the chars or strings in the source
 file becomes ANSI-chars or strings.
	If TCHAR is typedef-ed to \c wchar_t then the chars or strings in the source
 file becomes Unicode-chars or strings.

\b Characteristics of this ps_TCHAR.h file:

	1. For source files who use(#include) it, ps_TCHAR.h appears 
 platform-independent. But for the provider of ps_TCHAR.h for a specific platform,
 ps_TCHAR.h is platform-dependent(platform-specific), i.e. different platforms 
 have different ps_TCHAR.h .

	2. This header will try to introduce no pollution to its user, which means:
 although a ps_TCHAR.h itself is platform-specific, the ps_TCHAR.h should not #include 
 any platform specific headers.
	For example, ps_TCHAR.h for Windows should not include TCHAR.H provided with
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
	The user of ps_TCHAR.h may not want to see those pollution!
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
	That is, test and defining _TCHAR_DEFINED makes a second definiton(
 no matter it is ps_TCHAR.h or TCHAR.h ) peaceful.
	To accomplish this point, the system headers do have to cooperate, one 
 typical senarios is: it should associate a typedef-ed name with a macro
 name, such as _TCHAR_DEFINED for TCHAR, thus we can know from _TCHAR_DEFINED 
 whether TCHAR is typedef-ed.
	Anyhow, I suggest including our ps_TCHAR.h after system's TCHAR.h, which
 make conflicting-avoiding easier. For example, if you include ps_TCHAR.h
 before TCHAR.h, __T turns out to be a redefiniton, since __T in TCHAR.h is not
 guarded with #ifndef .
*/


#ifdef _UNICODE

#ifndef __T
 #define __T(x)	L##x
#endif

#ifndef _TCHAR_DEFINED
 typedef wchar_t     TCHAR;
 #define _TCHAR_DEFINED
#endif

// chj >>>
/*#ifndef _TINT_DEFINED
 typedef wint_t TINT;
 #define _TINT_DEFINED
#endif*/ 
	/* [2006-02-27]Chj: Windows stick wint_t to wctype_t with the 
	 protection of _WCTYPE_T_DEFINED, which makes me sick. 
	 Therefore, I'm better off getting away TINT, since we can always
	 use int instead of TINT to represent a single stramphibian character.
	 */
// chj <<<




#else	/* ================ Not _UNICODE ================ */



#ifndef __T
# define __T(x)	(x)
#endif

#ifndef _TCHAR_DEFINED
 typedef char     TCHAR;
 #define _TCHAR_DEFINED
#endif

// chj >>>
/*#ifndef _TINT_DEFINED
 typedef int TINT;
 #define _TINT_DEFINED
#endif*/
// chj <<<

#endif // #ifdef _UNICODE .else



/* Generic text macros to be used with string literals and character constants.
   Will also allow symbolic constants that resolve to same. */

#ifndef _T
# define _T(x)       __T(x)
#endif
#ifndef _TEXT
# define _TEXT(x)    __T(x)
#endif

#endif
