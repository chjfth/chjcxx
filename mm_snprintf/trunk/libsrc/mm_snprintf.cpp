/*
 * mm_snprintf.cpp - a portable implementation of snprintf
 *
 * INITIAL AUTHOR
 *   Mark Martinec <mark.martinec@ijs.si>, April 1999.
 *
 * V3.0 and above updated by:
 *	Chen Jun.
 *
 *   Copyright 1999, Mark Martinec. All rights reserved.
 *
 * TERMS AND CONDITIONS
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the "Frontier Artistic License" which comes
 *   with this Kit.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty
 *   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *   See the Frontier Artistic License for more details.
 *
 *   You should have received a copy of the Frontier Artistic License
 *   with this Kit in the file named LICENSE.txt .
 *   If not, I'll be glad to provide one.
 *
 * FEATURES
 * - careful adherence to specs regarding flags, field width and precision;
 * - good performance for large string handling (large format, large
 *   argument or large paddings). Performance is similar to system's sprintf
 *   and in several cases significantly better (make sure you compile with
 *   optimizations turned on, tell the compiler the code is strict ANSI
 *   if necessary to give it more freedom for optimizations);
 * - return value semantics per ISO/IEC 9899:1999 ("ISO C99");
 * - written in standard ISO/ANSI C - requires an ANSI C compiler.
 *
 * SUPPORTED CONVERSION SPECIFIERS AND DATA TYPES
 *
 * This snprintf only supports the following conversion specifiers:
 * s, c, d, u, o, x, X, p  (and synonyms: i, D, U, O - see below)
 * with flags: '-', '+', ' ', '0' and '#'.
 * An asterisk is supported for field width as well as precision.
 *
 * Length modifiers 'h' (short int), 'l' (long int),
 * and 'll' (long long int) are supported.
 * NOTE:
 *   If macro SNPRINTF_LONGLONG_SUPPORT is not defined (default) the
 *   length modifier 'll' is recognized but treated the same as 'l',
 *   which may cause argument value truncation! Defining
 *   SNPRINTF_LONGLONG_SUPPORT requires that your system's sprintf also
 *   handles length modifier 'll'.  long long int is a language extension
 *   which may not be portable.
 *
 * Conversion of numeric data (conversion specifiers d, u, o, x, X, p)
 * with length modifiers (none or h, l, ll) is left to the system routine
 * sprintf, but all handling of flags, field width and precision as well as
 * c and s conversions is done very carefully by this portable routine.
 * If a string precision (truncation) is specified (e.g. %.8s) it is
 * guaranteed the string beyond the specified precision will not be referenced.
 *
 * The following common synonyms for conversion characters are supported:
 *   - i is a synonym for d
 *   - D is a synonym for ld, explicit length modifiers are ignored
 *   - U is a synonym for lu, explicit length modifiers are ignored
 *   - O is a synonym for lo, explicit length modifiers are ignored
 * The D, O and U conversion characters are nonstandard, they are supported
 * for backward compatibility only, and should not be used for new code.
 *
 * The following is specifically NOT supported:
 *   - flag ' (thousands' grouping character) is recognized but ignored
 *   - numeric conversion specifiers: f, e, E, g, G and synonym F,
 *     as well as the new a and A conversion specifiers
 *   - length modifier 'L' (long double) and 'q' (quad - use 'll' instead)
 *   - wide character/string conversions: lc, ls, and nonstandard
 *     synonyms C and S
 *   - writeback of converted string length: conversion character n
 *   - the n$ specification for direct reference to n-th argument
 *   - locales
 *
 * It is permitted for str_m to be zero, and it is permitted to specify NULL
 * pointer for resulting string argument if str_m is zero (as per ISO C99).
 *
 * The return value is the number of characters which would be generated
 * for the given input, excluding the trailing null. If this value
 * is greater or equal to str_m, not all characters from the result
 * have been stored in str, output bytes beyond the (str_m-1) -th character
 * are discarded. If str_m is greater than zero it is guaranteed
 * the resulting string will be null-terminated.
 *
 * NOTE that this matches the ISO C99, OpenBSD, and GNU C library 2.1,
 * but is different from some older and vendor implementations,
 * and is also different from XPG, XSH5, SUSv2 specifications.
 * For historical discussion on changes in the semantics and standards
 * of snprintf see printf(3) man page in the Linux programmers manual.
 *
 * Routines asprintf and vasprintf return a pointer (in the ptr argument)
 * to a buffer sufficiently large to hold the resulting string. This pointer
 * should be passed to free(3) to release the allocated storage when it is
 * no longer needed. If sufficient space cannot be allocated, these functions
 * will return -1 and set ptr to be a NULL pointer. These two routines are a
 * GNU C library extensions (glibc).
 *
 * Routines asnprintf and vasnprintf are similar to asprintf and vasprintf,
 * yet, like snprintf and vsnprintf counterparts, will write at most str_m-1
 * characters into the allocated output string, the last character in the
 * allocated buffer then gets the terminating null. If the formatted string
 * length (the return value) is greater than or equal to the str_m argument,
 * the resulting string was truncated and some of the formatted characters
 * were discarded. These routines present a handy way to limit the amount
 * of allocated memory to some sane value.
 *
 * AVAILABILITY
 *   http://www.ijs.si/software/snprintf/
 *
 * REVISION HISTORY
 * 1999-04	V0.9  Mark Martinec
 *		- initial version, some modifications after comparing printf
 *		  man pages for Digital Unix 4.0, Solaris 2.6 and HPUX 10,
 *		  and checking how Perl handles sprintf (differently!);
 * 1999-04-09	V1.0  Mark Martinec <mark.martinec@ijs.si>
 *		- added main test program, fixed remaining inconsistencies,
 *		  added optional (long long int) support;
 * 1999-04-12	V1.1  Mark Martinec <mark.martinec@ijs.si>
 *		- support the 'p' conversion (pointer to void);
 *		- if a string precision is specified
 *		  make sure the string beyond the specified precision
 *		  will not be referenced (e.g. by strlen);
 * 1999-06-30	V1.3  Mark Martinec <mark.martinec@ijs.si>
 *		- fixed runaway loop (eventually crashing when str_l wraps
 *		  beyond 2^31) while copying format string without
 *		  conversion specifiers to a buffer that is too short
 *		  (thanks to Edwin Young <edwiny@autonomy.com> for
 *		  spotting the problem);
 *		- added macros PORTABLE_SNPRINTF_VERSION_(MAJOR|MINOR)
 *		  to snprintf.h
 * 2000-02-14	V2.0 (never released) Mark Martinec <mark.martinec@ijs.si>
 *		- relaxed license terms: The Artistic License now applies.
 *		  You may still apply the GNU GENERAL PUBLIC LICENSE
 *		  as was distributed with previous versions, if you prefer;
 *		- changed REVISION HISTORY dates to use ISO 8601 date format;
 *		- added vsnprintf (patch also independently proposed by
 *		  Caolan McNamara 2000-05-04, and Keith M Willenson 2000-06-01)
 * 2000-06-27	V2.1  Mark Martinec <mark.martinec@ijs.si>
 *		- removed POSIX check for str_m<1; value 0 for str_m is
 *		  allowed by ISO C99 (and GNU C library 2.1) - (pointed out
 *		  on 2000-05-04 by Caolan McNamara, caolan@ csn dot ul dot ie).
 *		  Besides relaxed license this change in standards adherence
 *		  is the main reason to bump up the major version number;
 *		- added nonstandard routines asnprintf, vasnprintf, asprintf,
 *		  vasprintf that dynamically allocate storage for the
 *		  resulting string; these routines are not compiled by default,
 *		  see comments where NEED_V?ASN?PRINTF macros are defined;
 *		- autoconf contributed by Caolan McNamara
 * 2000-10-06	V2.2  Mark Martinec <mark.martinec@ijs.si>
 *		- BUG FIX: the %c conversion used a temporary variable
 *		  that was no longer in scope when referenced,
 *		  possibly causing incorrect resulting character;
 *		- BUG FIX: make precision and minimal field width unsigned
 *		  to handle huge values (2^31 <= n < 2^32) correctly;
 *		  also be more careful in the use of signed/unsigned/size_t
 *		  internal variables - probably more careful than many
 *		  vendor implementations, but there may still be a case
 *		  where huge values of str_m, precision or minimal field
 *		  could cause incorrect behaviour;
 *		- use separate variables for signed/unsigned arguments,
 *		  and for short/int, long, and long long argument lengths
 *		  to avoid possible incompatibilities on certain
 *		  computer architectures. Also use separate variable
 *		  arg_sign to hold sign of a numeric argument,
 *		  to make code more transparent;
 *		- some fiddling with zero padding and "0x" to make it
 *		  Linux compatible;
 *		- systematically use macros fast_memcpy and fast_memset
 *		  instead of case-by-case hand optimization; determine some
 *		  breakeven string lengths for different architectures;
 *		- terminology change: 'format' -> 'conversion specifier',
 *		  'C9x' -> 'ISO/IEC 9899:1999 ("ISO C99")',
 *		  'alternative form' -> 'alternate form',
 *		  'data type modifier' -> 'length modifier';
 *		- several comments rephrased and new ones added;
 *		- make compiler not complain about 'credits' defined but
 *		  not used;
 * 2006-10-03  V3.0 by Chj <chenjun@nlscan.com>
 *		- SNPRINTF_LONGLONG_SUPPORT is always defined now, and "%lld", 
 *		  "%llu", "%llx", "%llX" can be used to format a 64-bit integer.
 *		  This is now system independent to the user due to the 
 *		  implementation of platform specific function mmps_i64_type_prefix().
 *		  Note: System's sprintf should be able to support formatting
 *		  64-bit integer in order for this to work.
 *		- Floating number formatting is now supported with the help of
 *		  system's sprintf. User can use %f %g %G %e %E now, e.g. "%-+8.4g" .
 *		  By the way, formatting "precision" of a floating number is
 *		  processed by system's sprintf, while "width" is processed by
 *		  this code.
 *		- Remove the original xx_COMPATIBLE preprocessor macros, because
 *		  I'd like the behavior of this code more unified and constant.
 *		- Add mmsnprintf_getversion() to return the implementation version
 *		  of this code.
 * 2008-05-26  V3.1 by Chj <chenjun@nlscan.com>
 *		- Unicode compile support, one library supporting both char and wchar_t .
 *		- New library available on WinCE, as well as Windows, Linux.
 * 2014-07-13 V4.0 by Chj <chenjun@nlscan.com>
 *		- New memory-dump(memdump) format specifier %m %M .
 *		- Control memdump decoration and format with %k %K %r %R %v
 * 2014-10-17 V4.2
 *		- Creative %w format, recursive va_args formatting.
 *
 * 2016-03-27  V4.3 by Chj
 *		- Add mm_strcat, demonstrating the use of %w .
 *
 * 2017-01-28  V4.4 by Chj
 *		- When compiling, USE_CPP_NEW determines whether C++ new is used for asprintf.
 *		- Add mm_free_buffer() to free the buffer returned by asprintf.
 *		- %w now has a way to consume only ONE *sprintf parameter(recommended).
 *		  and still compatible with the old(v4.2) two-param way.
 *
 * 2017-02-15  V5.0 by Chj
 *      - Formatting integer using self-sufficient code, unsigned_ntos and signed_ntos,
          so that CRT lib is not required(if you don't do floating-point formatting). 
		- Thousand separator(thousep) support for integers. 
		  %D,%U means "need thousep", default thousep string is " ", %T to customize.
		  For %d %u %x %X %o %O %p %P, customize them with %_ and %T.
 *
 * 2017-02-25  V5.1 by Chj
 *      - New function mm_snprintf_am(), updating pbuf and bufremain as a  
 *        convenient way for concatenating formatted string.
 *
 * 2017-03-21  V6.0 by Chj
 *      - %F to inject a function call. 
 *
 * 2017-08-03  V6.1 by Chj
 *      - Fix to "%.*s", now will not read beyond requested "precision". 
 *      - Avoid executing float-point instruction when not needed.
 *        This is to avoid possible subtle problems from compiler bugs.
 *        Ref: https://randomascii.wordpress.com/2016/09/16/everything-old-is-new-again-and-a-compiler-bug/
 *
 * 2017-09-07  V6.2 by Chj
 *      - Update to v5.0: We can now pad extra zeros for thousep formatting.
 *
 *          oks = t("[000,012,345]");
 *          mprint(oks, t("%t[%0.9u]"), t(","), 12345); 
 *
 *      - Add utility function: mm_printfA and mm_printfW, then there will be no more
 *        hassle using stramphibian printf/wprintf on Linux. Just mm_printf(...) to go. 
 *
 *      - Update to v6.0: An (incompatible) change to FUNC_mm_fpair callback prototype.
 *        Now, the callee can see the already formatted(stock) strings.
 *        So, %F enables you to *inject* your own code to see the complete formatting buffer 
 *        which would otherwise not possible when the mmstyle function hides the formatting
 *        buffer from the caller. Imagine, a mm_WriteLog() function does not present you
 *        his internal formatting buffer, but now you can peek and poke his buffer as such:
 *        
 *            mm_WriteLog("The ball turns %d rounds.%F", 3, MM_FPAIR_PARAM(mmF_peek, NULL));
 *
 *        Now, your mmF_peek() will see from pstock param: "The ball turns 3 rounds."
 *
 * 2017-09-30  V7.0 by Chj
 *      - New function mm_printf_ct and mm_snprintf_v7, mm_vsnprintf_v7 .
 *        The "ct" means custom-target. You can direct the formatted result to your 
 *        custom target(a file or a COM port) by providing a callback function, 
 *        so to avoid preparing a temporary buffer size to hold the result string. 
 *        What's more, user can get to know mm_snprintf's internal formatting progress 
 *        via FUNC_mmct_output() callback.
 *
 *      - Update to %*.*v semantic.
 */


/* Define SNPRINTF_LONGLONG_SUPPORT if you want to support
 * data type (long long int) and length modifier 'll' (e.g. %lld).
 * If undefined, 'll' is recognized but treated as a single 'l'.
 *
 */

// Some configuration macros:

#define SNPRINTF_LONGLONG_SUPPORT

// USE_CPP_NEW:
//	Use C++ new/delete for asprintf memory allocation.
//	If not defined, use malloc/free.


#include <assert.h>
#include <stdio.h>
#include <stdlib.h>		// for malloc
#include <stdarg.h>
//#include <sys/types.h> //Chj commented it out, for ARMCC does not have this.
#include <string.h>
//#include <errno.h> //[2008-05-16]Chj: WinCE don't have this.
#include <ctype.h>

#include <ps_Tstrdef.h> 
	//Note:
	// 1. This, on MSVC6, may include <tchar.h>, so list it before <ps_TCHAR.h>
	//otherwise, __T redefinition occurs.
	// 2. This file #defines va_copy macro if it is not defined in <stdarg.h>
	//so you MUST #include it after <stdarg.h>

//////////////////////////////
#include <ps_TCHAR.h>
#include <commdefs.h>
#include <_MINMAX_.h>
#include <mm_snprintf.h>
#include "internal.h"
#include "mm_psfunc.h"

#define DLL_AUTO_EXPORT_STUB

#ifdef NO_assert // specific for this library
# undef assert
# define assert(a) 
#endif


/* ============================================= */
/* NO USER SERVICABLE PARTS FOLLOWING THIS POINT */
/* ============================================= */

#if defined(NEED_ASPRINTF) || defined(NEED_ASNPRINTF) || defined(NEED_VASPRINTF) || defined(NEED_VASNPRINTF)
# if !defined(PREFER_PORTABLE_SNPRINTF)
# define PREFER_PORTABLE_SNPRINTF
# endif
#endif

#define Min(a,b) ((a) < (b) ? (a) : (b))
#define Max(a,b) ((a) > (b) ? (a) : (b))
#define Mid(a, c, b) Min( Max((a),(c)), (b) ) // please ensure a<b

enum // byte dump max values
{
	bdmax_hyphen = 8,
	bdmax_left = 4,
	bdmax_right = 4
};

typedef __int64 Int64;
typedef unsigned __int64 Uint64;

#define mmquan(array) ((int)(sizeof(array)/sizeof(array[0])))

#define fast_memcpy memcpy
//#define fast_memset memset


FUNC_mm_DebugProgress *g_procUserDebug;
void *g_cbexUserDebug;

void 
mm_set_DebugProgressCallback(FUNC_mm_DebugProgress *dbgproc, void *ctx_user)
{
	g_procUserDebug = dbgproc;
	g_cbexUserDebug = ctx_user;
}

static int 
_mmF_desc_widpreci(void *ctx_user, const mmv7_st &mmi)
{
	mmctexi_st &cti = *(mmctexi_st*)ctx_user;
	
	TCHAR *pbuf = mmi.buf_output;
	int bufremain = mmi.bufsize;
	
	// prepare string-form of width & precision 
	const int IW16 = 16;
	mmv7_st mmi2 = {0};
	mmi2.suppress_dbginfo = true; //!
	TCHAR sz_width[IW16], sz_preci[IW16];
	mmi2.buf_output = sz_width; mmi2.bufsize = IW16;
	mm_snprintf_v7(mmi2, _T(",%d"), cti.width);
	mmi2.buf_output = sz_preci; mmi2.bufsize = IW16;
	mm_snprintf_v7(mmi2, _T(",%d"), cti.precision);

	assert(mmi.suppress_dbginfo==true); // caller should have set this
	int len = mm_snprintf_v7(mmi, _T("width:%s%s preci:%s%s"),
		cti.has_width?_T("true"):_T("false"),     cti.has_width?sz_width:_T(""),
		cti.has_precision?_T("true"):_T("false"), cti.has_precision?sz_preci:_T("")
		);
	
	return len;
}

static void 
mmct_DebugStub(int call_count, const TCHAR *pcontent, int nchars, const mmctexi_st *pctexi)
{
	const mmctexi_st &cti = *pctexi;
	
	const int bufsize = MM_DBG_PROGRESS_LINE_MAXCHARS_;
	TCHAR dbginfo[bufsize]; 
//	dbginfo[0] = _T('\0');
//	int len0 = 0;

	mmv7_st mmi0 = {0};
	mmi0.buf_output = dbginfo;
	mmi0.bufsize = bufsize;
	mmi0.suppress_dbginfo = true; // important!

	if(call_count==0)
	{
		mm_snprintf_v7(mmi0, _T("<<fmtstring>>: %s%s"), cti.pfmt, g_mmcrlf_sz);

		g_procUserDebug(g_cbexUserDebug, dbginfo);
	}	
	
	mmv7_st mmi = {0};
	mmi.buf_output = dbginfo;
	mmi.bufsize = bufsize;
	mmi.suppress_dbginfo = true; // important!

	mm_snprintf_v7(mmi, _T("  <%.*s> fmtpos:%d+%d %F outpos:%d+%d%s"), 
		cti.fmtnc, cti.pfmt+cti.fmtpos, // <%.*s>
		cti.fmtpos, cti.fmtnc, // fmtpos:%d+%d
		MM_FPAIR_PARAM(_mmF_desc_widpreci, &cti), // %F
		cti.outpos, nchars, // output:%d+%d
		g_mmcrlf_sz);

	g_procUserDebug(g_cbexUserDebug, dbginfo);
}

static void 
ctipack_output(cti_pack_st &ctipack, const TCHAR *pcontent, int nchars)
{
	if(nchars<=0)
	{
		pcontent = _T("");
	}

	if(!ctipack.suppress_dbginfo && g_procUserDebug)
	{
		mmct_DebugStub(ctipack.call_count, pcontent, nchars, ctipack.pcti);
	}
	
	if(ctipack.ct_proc)
	{
		ctipack.ct_proc(ctipack.ct_ctx, pcontent, nchars, ctipack.pcti);
	}

	if(!ctipack.suppress_dbginfo)
		ctipack.call_count++;	

	if(nchars>0)
	{
		ctipack.pcti->outpos += nchars;
	}
}



TCHAR * mm_strncpy_(TCHAR *dst, const TCHAR *src, int ndst, bool null_end)
{
	// ndst tells dst[] buffer size, in chars
	// dst[] will always be NUL terminated if null_end=true, especially when ndst is not large enough
	int srclen = TMM_strlen(src);
	if(srclen>=ndst)
	{
		srclen = null_end ? ndst-1 : ndst;
	}	
	
	fast_memcpy(dst, src, TMM_strmembytes(srclen));
	
	if(srclen<ndst)
		dst[srclen] = 0;
	
	return dst;	
}

static bool is_zp_thoubody(bool is_zero_padding, int fmtspec_precision)
{
	if(is_zero_padding && fmtspec_precision)
		return true;
	else
		return false;
}

enum Radix_et{ RdxHex=0, RdxDec=1, RdxOct=2 };
static int unsigned_ntos(Uint64 u64, 
	Radix_et radix, bool ishex_uppercase, int stuff_zero_max,
	const TCHAR *psz_thousep, int thou_width,
	TCHAR buf[], int bufsize)
{
	// Deals with %d %u %U %x %X %p %P
	// Returns required bufsize(not counting and not filling trailing NUL).

	// stuff_zero_max, for %p %P, normally 8 or 16

	// psz_thousep: the string as thousand-separator, space, comma, dot etc.
	// thou_width: thousand-chip width in chars, normally 3, but you can customize it.
	
	assert(bufsize>1);

	int digits_gened = 0; // digits generated
	TCHAR rresult[80]; // reversed result
	unsigned div = 10;
	if(radix==RdxHex)
		div = 16; 
	else if(radix==RdxOct)
		div = 8;

	int len_thousep = 0;
	if(psz_thousep)
		len_thousep = TMM_strlen(psz_thousep);
	
	int thou_next_pos = thou_width;

	int i=0; // 'i' is the filling position into rresult[], may go beyond rresult[]
	for(;;)
	{
		if(thou_next_pos>0 && thou_next_pos==i)
		{
			// add thousand separator here (reversed order now)
			for(int j=0; j<len_thousep; j++, i++)
			{
				if(i<mmquan(rresult))
					rresult[i] = psz_thousep[len_thousep-1-j];
			}
			thou_next_pos = i + thou_width;
		}

		unsigned __int64 quotient = u64/div;
		unsigned remainder = (unsigned)(u64%div);

		if(i<mmquan(rresult))
		{
			rresult[i] = _T('0')+remainder;
			if(radix==RdxHex && remainder>=10)
			{
				rresult[i] = (remainder-10) + (ishex_uppercase?_T('A'):_T('a'));
			}
		}

		i++;
		digits_gened++;

		if(quotient==0 && digits_gened>=stuff_zero_max)
			break;
		
		u64 = quotient;
	}

	int ret = i;
	int copylen = _MIN_(i, mmquan(rresult));
	bufsize = _MIN_(bufsize, copylen);

	for(i=0; i<bufsize; i++)
	{
		buf[i] = rresult[copylen-1-i];
	}
	return ret;
}

static int signed_ntos(Int64 i64, 
	Radix_et radix, bool ishex_uppercase, int stuff_zero_max,
	const TCHAR *psz_thousep, int thou_width,
	TCHAR buf[], int bufsize)
{
	assert(bufsize>1);
	
	int filled = 0;
	if(i64<0)
	{
		buf[0] = _T('-');
		i64 = -i64;
		filled = 1;
	}

	int ret = unsigned_ntos(i64, 
		radix, ishex_uppercase, stuff_zero_max,
		psz_thousep, thou_width,
		buf+filled, bufsize-filled);

	return ret+filled;
}

struct mmfill_st
{
	TCHAR *pbuf;
	int bufmax;
	int produced; // may be larger than bufmax
};

#if 0
void mmfill_va_append(mmfill_st &f, FUNC_mmct_output *proc_output, void *ctx_output,
					  const TCHAR *fmt, ...)
{
	va_list args;
	va_start(args, fmt);

	mmv7_st mmi = {0};
	mmi.buf_output = f.pbuf+f.produced;
	mmi.bufsize = f.bufmax>f.produced ? f.bufmax-f.produced : 0;
	mmi.proc_output = proc_output;
	mmi.ctx_output = ctx_output;

	int reqlen = mm_vsnprintf_v7(mmi, fmt, args);

	va_end(args);
	f.produced += reqlen;
}
#endif

const TCHAR *
_mm_memchr(const TCHAR *buf, TCHAR c, size_t count)
{
	// memo: WinXP ntoskrnl does not have wmemchr, so I write mm_memchr() myself.
	
	size_t i=0;
	for(; i<count; i++)
	{
		if(buf[i]==c)
			return buf+i;
	}
	return NULL;
}

void 
_mm_fillchars(TCHAR *pbuf, TCHAR c, size_t n)
{
	size_t i;
	for(i=0; i<n; i++)
		pbuf[i] = c;
}

void 
_mm_fillchars_ct(cti_pack_st &ctipack, TCHAR c, size_t n)
{
	if(!ctipack.ct_proc && !g_procUserDebug)
		return; // a bit optimize

	const int chunksize = 100;
	TCHAR cbuf[chunksize];
	_mm_fillchars(cbuf, c, chunksize);
	
	int remain = (int)n;
	for(; remain>0; remain-=chunksize)
	{	
		// each callback deals with at most 100 chars.
		ctipack_output(ctipack, cbuf, _MIN_(remain, chunksize));
	}
}

void 
mmfill_fill_chars(mmfill_st &f, TCHAR c, int n, cti_pack_st &ctipack)
{
	// fill c*n chars 
	// fills f.pbuf until f.pbuf[] reaches bufmax; update f.produced by n
	
	_mm_fillchars_ct(ctipack, c, n);
	
	int nfill = Min(n, f.bufmax-f.produced);
	if(nfill>0)
		_mm_fillchars(f.pbuf+f.produced, c, nfill);

	f.produced += n;
}

TCHAR *
mmfill_strcpy(mmfill_st &f, const TCHAR *src, cti_pack_st &ctipack)
{
	// copy src to f.pbuf until f.pbuf[] reaches bufmax; update f.produced by src length
	int srclen = TMM_strlen(src);
	
	if(srclen>0)
	{
		ctipack_output(ctipack, src, srclen);

		int nfill = Min(srclen, f.bufmax-f.produced);
		if(nfill>0)
			mm_strncpy_(f.pbuf+f.produced, src, nfill, false);
		
		f.produced += srclen;
	}
	
	return f.pbuf + (f.produced - srclen);
}

int fill_adcol_text(Uint64 imagine_addr, TCHAR buf[], int bufsize, 
	int sep_width, int adcol_width, const TCHAR *adcol_sepstr)
{
	assert(bufsize>=20);
	in_snprintf(buf, bufsize, _T("%_%t%0.*llX"), 
		sep_width,  // %_ , normally 4 or 8
		adcol_sepstr, // %t , thousep string, normally ".", "`"
		adcol_width, imagine_addr // %0.*llX , pad 0s to at least this v_adcol_width
		);
	return TMM_strlen(buf);
}

int 
cal_adcol_widths(Uint64 imagine_addr, Uint64 imagine_end_,
	int v_sep_width, int v_adcol_width, const TCHAR *adcol_sepstr, 
	int *pWidth2 // output width2
	)
{
	// Calculate address-column digits(space width) after hex formatted,
	// trimming leading zeros.
	// Example: Input v_adcol_with=8, v_sep_width=4, adcol_sepstr="." ,
	// we'll produce output:
/*
	-----------00-01-02-03-04-05-06-07-08-09-0A-0B-0C-0D-0E-0F
	0040.0000:       32 33 34 35 36 37 38 39 3a 3b 3c 3d 3e 3f
	0040.0010: aa bb cc
*/
	// and we'll return two widths
	// width2 is 9 which counts "0000.0000".
	// width1 is 8 which counts count only 0s inside "0000.0000".
	// Memo: width2 must be larger or equal to width1.
	// In above example, 0x40000000 is the imagine_addr
	
	// If v_adcol_width is 0, it means use minimum(4) width2, and the result 
	// will be: (returning 4)
/*
	------00-01-02-03-04-05-06-07-08-09-0A-0B-0C-0D-0E-0F
	0000: 30 31 32 33 34 35 36 37 38 39 3a 3b 3c 3d 3e 3f
	0010: aa bb cc
*/
	
	if(v_adcol_width==0)
		v_adcol_width = 1;
	
	Uint64 imagine_end = imagine_end_-1;
	if(imagine_end<imagine_addr) 
		imagine_end = (Uint64)-1; // it fixes rewind
	
	if(!adcol_sepstr)
		adcol_sepstr = _T("");

	const int i64hsize = 20; // enough to hex-represent a 64-bit value
	TCHAR addr_end[i64hsize];
	int end_width = in_snprintf(addr_end, i64hsize, _T("%llX"), imagine_end);
	int width1 = 0;
	int adcol_width = 0;
	if(Is_IsoZeros(v_sep_width, v_adcol_width))
	{
		adcol_width = UP_ROUND(end_width, v_sep_width);
		width1 = adcol_width;
	}
	else
	{
		adcol_width = v_adcol_width;
		width1 = Max(end_width, adcol_width);
	}
	assert(adcol_width>0);

	TCHAR addrstr[40];
	*pWidth2 = fill_adcol_text(imagine_end, addrstr, mmquan(addrstr),
		v_sep_width, adcol_width, adcol_sepstr);
	return width1;
}

int 
_mm_dump_bytes(TCHAR *buf, int bufchars, 
	const void *_pbytes, int dump_bytes, bool uppercase,
	const TCHAR *mdd_hyphens, const TCHAR *mdd_left, const TCHAR *mdd_right,
	int columns, int colskip, bool ruler,
	int indents, 
	Uint64 imagine_addr, int v_sep_width, int v_adcol_width, const TCHAR *adcol_sepstr,
	cti_pack_st &ctipack)
{
	/*
	Dump hex-represented byte content into buf[], but not exceeding bufchars.
	If buf[] too small, no NUL required at buf[] end.

	pbytes: point to byte buffer.
	dump_bytes: bytes to dump.

	columns: Consume that many bytes for one output line, 
		use use this to add line breaks for large input bytes.
		If 0, dump all on one line.

	indents: spaces to add for every dump line output.

	ruler: whether add [horizontal and vertical ruler] so that byte position in dump 
		is easily recognized.
		Ruler is something like:
		
		------00-01-02-03-04-05-06-07-08-09-0A-0B-0C-0D-0E-0F
        0000: 30 31 32 33 34 35 36 37 38 39 3a 3b 3c 3d 3e 3f
		0010: aa bb cc
         ^
		 ^
		 ^
		 I call this address-column, default adcol_digits=4


	Return: how many TCHARs will be output assuming buf[] is large enough.
	*/

	if(dump_bytes<=0)
	{
		if(bufchars>0)
			buf[0] = '\0';
		return 0;
	}

	const unsigned char *pbytes = (unsigned char*)_pbytes;

	columns = Mid(0, columns, 256);
	if(columns==0) {
		columns = 0x7FFFffff;
		ruler = false;
	}
	colskip = Mid(0, colskip, columns-1);

	const int ex2 = 2;
	int i, j;
	TCHAR onehex[8]; // only to accommodate "00", "01", "0f", "ff" etc
	int len_hyphens = TMM_strlen(mdd_hyphens);
	int len_left = TMM_strlen(mdd_left);
	int len_right = TMM_strlen(mdd_right);

	int dump_width_perbyte = len_left+2+len_right+len_hyphens;

	Uint64 imagine_end_ = imagine_addr + dump_bytes;
	int adcol_width2 = 0;
	int adcol_width1 = cal_adcol_widths(imagine_addr, imagine_end_, 
		v_sep_width, v_adcol_width, adcol_sepstr, &adcol_width2);

	int consumed = 0; // consumed source bytes
	mmfill_st mmfill = {buf, bufchars, 0};

	if(ruler)
	{
		// first: indents and left ruler spaces
		mmfill_fill_chars(mmfill, _T(' '), indents,          ctipack);
		mmfill_fill_chars(mmfill, _T('-'), adcol_width2+ex2, ctipack);
			// ex2 is for ": " following the address column digits

		// second: horizontal marks
		for(i=0; i<columns; i++)
		{
			mmfill_fill_chars(mmfill, _T('-'), len_left,     ctipack);
			in_snprintf(onehex, mmquan(onehex), _T("%02X"), i);
			mmfill_strcpy(mmfill, onehex,                    ctipack);
			mmfill_fill_chars(mmfill, _T('-'), len_right,    ctipack);

			if(i<columns-1)
				mmfill_fill_chars(mmfill, _T('-'), len_hyphens, ctipack);
		}
		
		mmfill_strcpy(mmfill, g_mmcrlf_sz,                   ctipack);
	}

	Uint64 imagine_addr_to_print = imagine_addr-colskip;

	const TCHAR *fmthex = uppercase ? _T("%02X") : _T("%02x");

	for(;;)
	{
		// Every cycle generates one output line

		// first: indents
		mmfill_fill_chars(mmfill, _T(' '), indents,          ctipack);

		// second: address column
		if(ruler)
		{
			TCHAR addrstr[40];
			fill_adcol_text(imagine_addr_to_print, addrstr, mmquan(addrstr),
				v_sep_width, adcol_width1, adcol_sepstr);
				
			mmfill_strcpy(mmfill, addrstr,                   ctipack);
			mmfill_strcpy(mmfill, _T(": "),                  ctipack);
		}

		// third: column-skip blank area
		if(colskip>0)
		{
			mmfill_fill_chars(mmfill, _T(' '), dump_width_perbyte*colskip, ctipack);
		}

		// fourth: hex dumps of current line with decoration
		int colomn_remain = Min(dump_bytes-consumed, columns-colskip);
			// columns remain for this dump line
		for(j=0; j<colomn_remain; j++)
		{
			if(mdd_left[0])
				mmfill_strcpy(mmfill, mdd_left,      ctipack);
			
			in_snprintf(onehex, mmquan(onehex), fmthex, pbytes[consumed+j]);
			mmfill_strcpy(mmfill, onehex,            ctipack);
			
			if(mdd_right[0])
				mmfill_strcpy(mmfill, mdd_right,     ctipack);

			if(j<colomn_remain-1)
				mmfill_strcpy(mmfill, mdd_hyphens,   ctipack);
		}
		colskip = 0;

		consumed += colomn_remain;

		if(consumed==dump_bytes)
			break;

		mmfill_strcpy(mmfill, g_mmcrlf_sz,           ctipack);
		assert(consumed<dump_bytes);

		imagine_addr_to_print += columns;
	}

	return mmfill.produced;
}

///////////////////

int mm_vsnprintf(TCHAR *strbuf, size_t str_m, const TCHAR *fmt, va_list ap)
{
	mmv7_st mmi = {0};
	mmi.buf_output = strbuf;
	mmi.bufsize = str_m;

	int ret = mm_vsnprintf_v7(mmi, fmt, ap);
	return ret;
}


int mm_vsnprintf_v7(const mmv7_st &mmi, const TCHAR *fmt, va_list ap) 
{
	TCHAR *strbuf = mmi.buf_output;
	size_t str_max = Max(0, mmi.bufsize);
	FUNC_mmct_output *proc_output = mmi.proc_output;
	void *ctx_output = mmi.ctx_output;
	int mylevel = mmi.mmlevel + 1;

	size_t str_l = 0; // how many output length has been filled, assuming enough output buffer
	const TCHAR *p = fmt;

/* In contrast with POSIX, the ISO C99 now says
 * that str can be NULL and str_m can be 0.
 * This is more useful than the old:  if (str_m < 1) return -1; */

	if (!p) 
		p = _T("");

	int mdd_indents = 0; // mdd: memory-dump decoration
	TCHAR mdd_hyphens[bdmax_hyphen+1]=_T(""); 
	TCHAR mdd_left[bdmax_left+1]=_T(""), mdd_right[bdmax_right+1]=_T("");
	int mdf_columns = 0;  // 0 means do not dump on multiple lines
	int mdf_colskip = 0;  // memory-dump first-line column skip 
		// To have first byte appear at column 3 instead of column 0, you set mdf_colskip=3 .
	bool is_print_ruler = false;
	Uint64 imagine_maddr = 0; // %v 's data
	int v_adcol_width = 0, v_sep_width = 0; // %v's address-column-width and separator width
	bool v_is_isozeros = false; // for %v

	int thousep_width = 3; // %_ to modify
	const TCHAR *psz_thousep = NULL; // thousand separator, (%t to modify)
	const TCHAR *psz_THOUSEP = NULL; // only for %D, %U, %O, (%T to modify)

	mmctexi_st cti = {0};     // cti(a)
	cti.mmlevel = mylevel;
	cti.pfmt = fmt;
	//
	cti_pack_st ctipack = {proc_output, ctx_output, &cti, mmi.suppress_dbginfo};
	
	while (*p) 
	{
		cti.fmtpos = p - fmt;   // cti(b)
		cti.valsize = 0;
		assert(sizeof(cti.placehldr)>=sizeof(double));
		memset(cti.placehldr, 0, sizeof(cti.placehldr));

		// Copy a chunk of normal substring
		//
		if (*p != _T('%')) 
		{
		   /* if (str_l < str_m) str[str_l++] = *p++;    -- this would be sufficient */
		   /* but the following code achieves better performance for cases
			* where format string is long and contains few conversions */
			const TCHAR *q = TMM_strchr(p, _T('%'));
				//[2009-05-20]Chj Note: Don't try to search from p+1 !!
				//If p points to an MBCS char, that will break the MBCS char and possible swallow the % following that MBCS sequence.

			size_t n = !q ? TMM_strlen(p) : (q-p);

			cti.fmtnc = n; // cti(c1)
			cti.has_width = cti.has_precision = false; cti.width = cti.precision = 0;
			ctipack_output(ctipack, p, n);
			
			if (str_l < str_max) {
				size_t avail = str_max-str_l;
				fast_memcpy(strbuf+str_l, p, TMM_strmembytes(n>avail?avail:n));
			}

			p += n; str_l += n;
			continue;
		} 

		// Remaining, for (*p=='%') [until end of the outer-most while]

		const TCHAR *starting_p = NULL; // starting % position
			// Record the position of the '%', so that(if linux behavior is used)
			//when the type character following % is unrecognized(e.g. "%a"), 
			//the whole format specifier is output, e.g. "%8.4a" is output as
			//"%8.4a", not just "a" .

		Int min_field_width = 0, precision = 0;
		bool min_field_specified = false, precision_specified = false;
		bool zero_padding = false, justify_left = false;
		bool zero_padding_thou = false; // v6.2: if true, we need "001,234" instead of "1,234"
		bool alternate_form = false;
		bool force_sign = false; // whether reserve a char space(filled with ' ' or '+') for positive indication.
		bool space_for_positive = true; // use ' ' to indicate positive number instead of using '+'
			/* If both the ' ' and '+' flags appear, the ' ' flag should be ignored. */
		TCHAR length_modifier = _T('\0');   
			/* allowed values: \0, h, l, L , Chj: ll is allowed(encoded as '2').
			 '\0' means no length-modifier, e.g:
				In "%ld", 'l' is length-modifier; in "%hd", 'h' is length-modifier,
				while "%d" has no length-modifier in it.
			*/
		TCHAR tmp[128];/* temporary buffer for simple numeric->string conversion */
			//[2017-10-00]Chj: %f and %g will use this as system snprintf's output. (ff_16)

		const TCHAR *str_arg = NULL; /* string address in case of string argument */
		Int str_arg_l = 0;       /* natural field width of arg without padding
								   and sign */
		TCHAR uchar_arg; //unsigned char uchar_arg;
		/* unsigned char argument value - only defined for c conversion.
		   N.B. standard explicitly states the char argument for
		   the c conversion is unsigned --[2008-05-27]Chj: But it doesn't matter
		   for a function like snprintf, since we just copy the char value.
		*/

		Int number_of_zeros_to_pad = 0;
		/* number of zeros to be inserted for numeric conversions
		   as required by the precision or minimal field width */

		Int zero_padding_insertion_ind = 0;
		/* index into tmp[] where zero padding is to be inserted */

		bool is_UorD = false;

		///////

		TCHAR fmt_spec = _T('\0');
		/* current conversion specifier character */

		// here, p points to '%'
		str_arg = NULL;
		starting_p = p; 
		p++;  /* skip '%' */
		/* parse flags */
		while (*p == _T('0') || *p == _T('-') || *p == _T('+') ||
			 *p == _T(' ') || *p == _T('#') || *p == _T('\'')) 
		{
			switch (*p) {
			case _T('0'): zero_padding = zero_padding_thou = true; break;
			case _T('-'): justify_left = true; break;
			case _T('+'): force_sign = true; space_for_positive = false; break;
			case _T(' '): force_sign = true;
			/* If both the ' ' and '+' flags appear, the ' ' flag should be ignored */
					  break;
			case _T('#'): alternate_form = true; break;
			case _T('\''): break;
			}
			p++;
		}
		/* If the '0' and '-' flags both appear, the '0' flag should be ignored. */

		/* parse field width */
		if (*p == _T('*')) {
			p++; 
			int j = va_arg(ap, int);
			if (j >= 0) 
				min_field_width = j;
			else { //[2006-10-02] same as MSVCRT
				min_field_width = -j; 
				justify_left = true; 
			}
			min_field_specified = true;
		} 
		else if (TMM_isdigit(*p)) {
			/* size_t could be wider than unsigned int;
			   make sure we treat argument like common implementations do */
			unsigned int uj = *p++ - _T('0');
			while (TMM_isdigit(*p)) 
				uj = 10*uj + (unsigned int)(*p++ - _T('0'));
			min_field_width = uj;
			min_field_specified = true;
		}

		/* parse precision */
		if (*p == _T('.')) {
			p++; precision_specified = true;
			if (*p == _T('*')) {
			  int j = va_arg(ap, int);
			  p++;
			  if (j >= 0) 
				  precision = j;
			  else {
				precision_specified = false; precision = 0;
			 /* NOTE:
			  *   Solaris 2.6 man page claims that in this case the precision
			  *   should be set to 0.  Digital Unix 4.0, HPUX 10 and BSD man page
			  *   claim that this case should be treated as unspecified precision,
			  *   which is what we do here.
			  */
			  }
			} 
			else if (TMM_isdigit(*p)) {
			  /* size_t could be wider than unsigned int;
				 make sure we treat argument like common implementations do */
			  unsigned int uj = *p++ - '0';
			  while (TMM_isdigit(*p)) 
				  uj = 10*uj + (unsigned int)(*p++ - _T('0'));
			  precision = uj;
			}
		}
		
		/* parse 'h', 'l' and 'll' length modifiers */
		if (*p == _T('h') || *p == _T('l')) {
			length_modifier = *p; p++;
			if (length_modifier == _T('l') && *p == _T('l'))  /* double l = long long, i.e. __int64 */
			{
#ifdef SNPRINTF_LONGLONG_SUPPORT
				length_modifier = _T('2');            /* double l encoded as '2' */
#else
				length_modifier = _T('l');            /* treat it as a single 'l' */
#endif
				p++;
			}
		}
		
		fmt_spec = *p;

		cti.fmtnc = p - fmt - cti.fmtpos + 1 ;    // cti(c2)
		cti.has_width = min_field_specified, cti.has_precision = precision_specified;
		cti.width = min_field_width, cti.precision = precision;
		
		//
		// NOW, THE LENGTHY PROCESS OF fmt_spec(also called "type" in MSDN) STARTS.
		//
		
		if(fmt_spec==_T('D') || fmt_spec==_T('U')) 
		{
			fmt_spec = fmt_spec==_T('D') ? _T('d') : _T('u'); 

			is_UorD = true;
			if(!psz_THOUSEP)
				psz_THOUSEP = _T(" ");
		}

		/* get parameter value, do initial processing */
		switch (fmt_spec) // BIG CHUNK OF PROCESS!!
		{{
		case _T('%'): /* % behaves similar to 's' regarding flags and field widths */
			/*[2006-10-03]Chj: Mark Martinec processes the '%' here, which means
			 "%3.1%" would be output as "  %", which is different from MSVCRT
			 and glibc(they output just "%"). However, that's not big deal.
			*/
		case _T('c'): /* c behaves similar to 's' regarding flags and field widths */
		case _T('s'):
			{
				length_modifier = _T('\0');
				/* the result of zero padding flag with non-numeric conversion specifier*/
				/* is undefined. Solaris and HPUX 10 does zero padding in this case,    */
				/* Digital Unix and Linux does not. */

				//zero_padding = 0;    /* turn zero padding off for string conversions */
					/* [2007-03-31]Chj: On MSVCRT 6.0(32-bit):
							printf("[%03s]", "7");
						 outputs:
							[007]
						Considering that sometimes it is useful to pad zeros before a
					 string, so, I decide not to reset zero_padding=0 .
					*/

				str_arg_l = 1; // 1 means: we'll grab 1 char from str_arg[] as output, for '%c'.
				               // Of course, this will be overridden when we later see fmt_spec==_T('s').

				if(fmt_spec==_T('%')) {
					str_arg = p; // *p is '%' here.
				}

				if(fmt_spec==_T('c')) {
					int j = va_arg(ap, int);
					uchar_arg = (TCHAR) j;   /* standard demands unsigned char */
					str_arg = (TCHAR*) &uchar_arg;
					CTI_SETVAL(cti, val_TCHAR, uchar_arg);
				}

				if(fmt_spec==_T('s')){
					str_arg = va_arg(ap, TCHAR*);
					CTI_SETVAL(cti, val_ptr, str_arg);
					if (!str_arg) 
						str_arg_l = 0;
				/* make sure not to address string beyond the specified precision !!! */
					else if (!precision_specified) 
						str_arg_l = TMM_strlen(str_arg);
				/* truncate string if necessary as requested by precision */
					else if (precision == 0) 
						str_arg_l = 0; //Chj: Yes, no characters from the string will be printed if `precision' is zero.
					else { // `precision' specified and > 0 
						const TCHAR *p0 = _mm_memchr(str_arg, _T('\0'), precision);
						str_arg_l = (Int)(p0 ? (p0-str_arg) : precision);
					}
				}
				break; // end of process for type '%','c','s' .
			}

		case _T('d'): case _T('u'): case _T('o'): case _T('x'): case _T('X'): 
		case _T('p'): case _T('P'): // [2017-02-11] Chj adds 'P', upper-case pointer value
		case _T('f'): case _T('g'): case _T('G'): case _T('e'): case _T('E'):
			{
				/* NOTE: the u, o, x, X and p conversion specifiers imply
						 the value is unsigned;  d implies a signed value */

				int arg_sign = 0;
				  /* 0 if numeric argument is zero (or if pointer is NULL for 'p'),
					+1 if greater than zero (or nonzero for unsigned arguments),
					-1 if negative (unsigned argument is never negative) */

				int int_arg = 0;  unsigned int uint_arg = 0;
				  /* only defined for length modifier h (e.g. "%hd"), 
					or for no length modifiers (e.g. "%d") */

				long int long_arg = 0;  unsigned long int ulong_arg = 0;
				  /* only defined for length modifier l */

				void *ptr_arg = NULL;
				  /* pointer argument value -only defined for p conversion */

#ifdef SNPRINTF_LONGLONG_SUPPORT
				__int64 long_long_arg = 0;
				unsigned __int64 ulong_long_arg = 0;
				  /* only defined for length modifier ll */
#endif

				int isFloatingType = mmsnprintf_IsFloatingType(fmt_spec);

				Fmt_et fmtcat = fmt_unset;
				if(fmt_spec==_T('p') || fmt_spec==_T('P')) 
					fmtcat = fmt_ptr;
				else if(fmt_spec==_T('d'))
					fmtcat = fmt_decimal_signed;
				else if(!isFloatingType)
					fmtcat = fmt_decimal_unsigned;
				else
					fmtcat = fmt_float;

				double double_arg;
				if(fmtcat==fmt_float)
					double_arg = 0.0;
				else 
					; // avoid execting float-point instruction when not needed

				if(fmtcat==fmt_ptr)  // %p, %P
				{
				/* HPUX 10: An l, h, ll or L before any other conversion character
				 *   (other than d, i, u, o, x, or X) is ignored.
				 * Digital Unix:
				 *   not specified, but seems to behave as HPUX does.
				 * Solaris: If an h, l, or L appears before any other conversion
				 *   specifier (other than d, i, u, o, x, or X), the behavior
				 *   is undefined. (Actually %hp converts only 16-bits of address
				 *   and %llp treats address as 64-bit data which is incompatible
				 *   with (void *) argument on a 32-bit system).
				 */
				// We follow the HPUX 10 style.
					length_modifier = _T('\0');

					ptr_arg = va_arg(ap, void *);
					CTI_SETVAL(cti, val_ptr, ptr_arg);
					if (ptr_arg != NULL) 
						arg_sign = 1;
				} 
				else if(fmtcat==fmt_decimal_signed)
				{  /* signed */
					switch (length_modifier) {
					case _T('\0'):
					case _T('h'):
					/* It is non-portable to specify a second argument of char or short
					 * to va_arg, because arguments seen by the called function
					 * are not char or short.  C converts char and short arguments
					 * to int before passing them to a function.
					 */
						int_arg = va_arg(ap, int);
						CTI_SETVAL(cti, val_int, int_arg);
						if      (int_arg > 0) arg_sign =  1;
						else if (int_arg < 0) arg_sign = -1;
						break;
					case _T('l'):
						long_arg = va_arg(ap, long int);
						CTI_SETVAL(cti, val_long, long_arg);
						if      (long_arg > 0) arg_sign =  1;
						else if (long_arg < 0) arg_sign = -1;
						break;
#ifdef SNPRINTF_LONGLONG_SUPPORT
					case _T('2'):
						long_long_arg = va_arg(ap, __int64);
						CTI_SETVAL(cti, val_int64, long_long_arg);
						if      (long_long_arg > 0) arg_sign =  1;
						else if (long_long_arg < 0) arg_sign = -1;
						break;
#endif
					}
				} 
				else if(fmtcat==fmt_decimal_unsigned)
				{
					switch (length_modifier) 
					{
					case _T('\0'):
					case _T('h'):
						uint_arg = va_arg(ap, unsigned int);
						CTI_SETVAL(cti, val_uint, uint_arg);
						if (uint_arg) arg_sign = 1;
						break;
					case _T('l'):
						ulong_arg = va_arg(ap, unsigned long int);
						CTI_SETVAL(cti, val_ulong, ulong_arg);
						if (ulong_arg) arg_sign = 1;
						break;
#ifdef SNPRINTF_LONGLONG_SUPPORT
				  case _T('2'):
						ulong_long_arg = va_arg(ap, unsigned __int64);
						CTI_SETVAL(cti, val_uint64, ulong_long_arg);
						if (ulong_long_arg) arg_sign = 1;
						break;
#endif
					}
				}
				else // floating point number
				{
					assert(fmtcat==fmt_float);

					double_arg = va_arg(ap, double);
					CTI_SETVAL(cti, val_double, double_arg);
					if(double_arg>0) arg_sign = 1;
					else if(double_arg<0) arg_sign = -1;

					alternate_form = false;
					//[2006-10-03]Chj Set/clear More vars? Seems enough.
				}

				str_arg = tmp; str_arg_l = 0;

				/* NOTE:
				*   For d, i, u, o, x, and X conversions, if precision is specified,
				*   the '0' flag should be ignored. This is so with Solaris 2.6,
				*   Digital UNIX 4.0, HPUX 10, Linux, FreeBSD, NetBSD; but not with Perl.
				*/

				if (precision_specified) 
					zero_padding = false; // MSDN 2008 "Flag Directives" says this as well

				if(fmtcat==fmt_decimal_signed || fmtcat==fmt_float) 
				{
					if (force_sign && arg_sign >= 0)
						tmp[str_arg_l++] = space_for_positive ? _T(' ') : _T('+');
					 /* leave negative numbers for sprintf to handle,
						to avoid handling tricky cases like (short int)(-32768) */
// #ifdef LINUX_COMPATIBLE
// 				} 
// 				else if (fmt_spec == 'p' && force_sign && arg_sign > 0) {
// 					tmp[str_arg_l++] = space_for_positive ? ' ' : '+';
// #endif //[2006-10-03]Chj: I do not use Linux's behavior here.
				} 
				else if (alternate_form) 
				{
					if (arg_sign != 0 && 
						(fmt_spec==_T('x') || fmt_spec==_T('X') || fmt_spec==_T('p') || fmt_spec==_T('P')) 
						)
					{ 
						/* [2007-03-31]Chj: On MSVCRT 6.0(32-bit):
							printf("[%+#p]", (void*)16);
						 outputs:
							[+0X00000010]
						 Now, I make mm_snprintf follow this scheme(check for fmt_spec=='p')
						 but with a minor change: output small 'x' instead of the capital one.
						 That is, mm_snprintf outputs:
							[+0x00000010]
						 Using small 'x' seems more reasonable, since it corresponds to the
						 small type-char 'p'.
						*/
						tmp[str_arg_l++] = _T('0'); 
						tmp[str_arg_l++] = 
							fmt_spec==_T('x')||fmt_spec==_T('p') // if lower-case
							? _T('x') : 
							_T('X'); 
					}
					/* alternate form should have no effect for p conversion, but ... */
				}

				zero_padding_insertion_ind = str_arg_l;
					//Chj: This is the position in tmp[] where real value is formated and stored by sprintf.

				if (!precision_specified) 
				{
					if(fmtcat==fmt_float)
						precision = 6;	// default precision is 6 floating point numbers
					else
						precision = 1;   /* default precision is 1 for integer types */
				}

				if (precision == 0 && arg_sign == 0
// #if defined(LINUX_COMPATIBLE)
// 					&& fmt_spec != 'p'
// 				 /* HPUX 10 man page claims: With conversion character p the result of
// 				  * converting a zero value with a precision of zero is a null string.
// 				  * Actually HP returns all zeros, and Linux returns(outputs) "(nil)". */
// #endif //[2006-10-03]Chj: I do not use Linux's behavior here.
					) 
				{
				 /* converted to null string */
				 /* When zero value is formatted with an explicit precision 0,
					the resulting formatted string is empty (d, i, u, o, x, X, p).   */
				//[2006-10-02]Chj: This is the behavior of MSVCRT
				} 
				else 
				{
					// >>> Use signed_ntos, unsigned_ntos, or system's sprintf to format numeric string in tmp[].

					TCHAR flfmt[32]; int f_l = 0; // `flfmt' used as a temp format string for system's snprintf
					flfmt[f_l++] = _T('%');    /* construct a simple format string for sprintf */
					
					if(fmtcat==fmt_float)
					{
						// Note: If the system CRT lib does not provide snprintf, 
						// mm_snprintf should not use %f and %g, which would give empty results.
						// Limit the precision so that not to overflow tmp[].

						f_l += in_snprintf(flfmt+f_l, GetEleQuan(flfmt), _T(".%d"), precision); 
							//For floating point number, we leave precision processing to system's sprintf.

						precision_specified = false; precision = 0;
							// !!! As if we have not specified the precision 
							// so that the "precision processing for integers" will not take place.
							// And our mm_'s code will also process min_field_width ourselves.
						
						// length_modifier is ignored for floating type.
					}
					else if (!length_modifier) {
						// do nothing
					}
// 					else if (length_modifier==_T('2')) { // to format a 64-bit integer
// 						/* [2006-10-02]Chj: We use an external routine to obtain the 
// 						 format-string for 64-bit integer, because it differs on 
// 						 different systems:
// 						   * On Microsoft VC runtime: "%I64d" or "%I64u" or "%I64x"
// 						   * On GNU glibc:   "%lld" or "%llu" or "%llx"
// 						   * On ARM SDT/ADS: "%lld" or "%llu" or "%llx"
// 						*/
// 						char tmpI64fmt[8]; // Yes, not TCHAR here!
// 						int lenI64fmt = mmps_i64_type_prefix(tmpI64fmt);
// 						for(int i=0; i<lenI64fmt; i++)
// 							f[f_l++] = tmpI64fmt[i];
// 					}
					else {
						flfmt[f_l++] = length_modifier;
					}


					flfmt[f_l++] = fmt_spec; flfmt[f_l++] = '\0';

					if(fmtcat==fmt_ptr) 
					{
						//str_arg_l += TMM_sprintf(tmp+str_arg_l, f, ptr_arg); // old
						Uint64 u64 = (Uint64)ptr_arg;
						if(sizeof(ptr_arg)==sizeof(unsigned)) {
							u64 = (unsigned)u64;
							// Quite tricky! Without this: When ptr_arg=0xEEEEeeee,
							// on 32-bit VC6/VC10 and 32-bit gcc 4.7/4.8, I would get
							// u64 = 0xFFFFFFFFeeeeeeee
						}
						str_arg_l += unsigned_ntos(u64, 
							RdxHex, fmt_spec==_T('p')?false:true, sizeof(ptr_arg)*2,
							psz_thousep, thousep_width,
							tmp+str_arg_l, mmquan(tmp)-str_arg_l
							);
					}
					else if(fmtcat==fmt_decimal_signed) 
					{  
						/* signed */
						const TCHAR *psz_thousep_now = is_UorD ? psz_THOUSEP : psz_thousep;
						int thousep_width_now = is_UorD ? 3 : thousep_width;
						Int64 i64 = 0;
						switch (length_modifier) {
						case _T('\0'):
						case _T('h'): 
							//str_arg_l+=TMM_sprintf(tmp+str_arg_l, f, int_arg); // old
							i64 = int_arg;
							break;
						case _T('l'): 
							//str_arg_l+=TMM_sprintf(tmp+str_arg_l, f, long_arg); 
							i64 = long_arg;
							break; // old
#ifdef SNPRINTF_LONGLONG_SUPPORT
						case _T('2'): 
							//str_arg_l+=TMM_sprintf(tmp+str_arg_l,f,long_long_arg); 
							i64 = long_long_arg;
							break;
#endif
						}
						str_arg_l += signed_ntos(i64, 
							RdxDec, false, 
							is_zp_thoubody(zero_padding_thou, precision) ? precision : 0, // stuff_zero_max
							psz_thousep_now, thousep_width,
							tmp+str_arg_l, mmquan(tmp)-str_arg_l
							);
					} 
					else if(fmtcat==fmt_decimal_unsigned) 
					{  
						/* unsigned ( u,U,x,X,o,O ) */  
						const TCHAR *psz_thousep_now = is_UorD ? psz_THOUSEP : psz_thousep;
						int thousep_width_now = is_UorD ? 3 : thousep_width;

						Radix_et radix = RdxHex;
						if(fmt_spec==_T('u')||fmt_spec==_T('U'))
							radix = RdxDec;
						else if(fmt_spec==_T('x')||fmt_spec==_T('X')) 
							radix = RdxHex;
						else {
							radix = RdxOct;
							assert( fmt_spec==_T('o')||fmt_spec==_T('O') );
						}

						Uint64 u64 = 0;
						switch (length_modifier) {
						case '\0':
						case 'h': 
							//str_arg_l+=TMM_sprintf(tmp+str_arg_l, f, uint_arg);  
							u64 = uint_arg;
							break;
						case 'l': 
							//str_arg_l+=TMM_sprintf(tmp+str_arg_l, f, ulong_arg); 
							u64 = ulong_arg;
							break;
#ifdef SNPRINTF_LONGLONG_SUPPORT
						case '2': 
							//str_arg_l+=TMM_sprintf(tmp+str_arg_l,f,ulong_long_arg); 
							u64 = ulong_long_arg;
							break;
#endif
						}
						str_arg_l += unsigned_ntos(u64, 
							radix, fmt_spec==_T('X')?true:false, 
							is_zp_thoubody(zero_padding_thou, precision) ? precision : 0, // stuff_zero_max
							psz_thousep_now, thousep_width_now,
							tmp+str_arg_l, mmquan(tmp)-str_arg_l
							);
					}
					else // floating type
					{
						assert(fmtcat==fmt_float);

						// Quite some subtle processing here.
						// I need to work with Windows _snprintf and C99's snprintf and C99's swprintf.

						int tmpbufsize = GetEleQuan(tmp);
						tmp[tmpbufsize-1] = _T('\0'); // so to cope with _snprintf's no-NUL on buffer full

						int cf = TMM_snprintf(tmp+str_arg_l, tmpbufsize-str_arg_l-1, flfmt, double_arg);
						if(cf>=0)
						{
							str_arg_l += cf;
						}
						else
						{
							// This is to deal with [Windows _snprintf]'s and [C99 swprintf]'s 
							// wacky -1 return(when buffer is not enough).

							// We need to strlen() it bcz _snprintf and swprintf has 
							// different NUL appending policy when buffer is not enough.
							// Strlen()'s result is the best deduction we can make here.
							int flen = TMM_strlen(tmp);
							str_arg_l += flen;
						}
					}

					// <<< Use signed_ntos, unsigned_ntos, or system's sprintf to format numeric string in tmp[].
				 
					/* include the optional minus sign and possible "0x"
					  in the region before the zero padding insertion point */
					if (zero_padding_insertion_ind < str_arg_l &&
						tmp[zero_padding_insertion_ind] == '-') {
						//[2006-10-03]Chj: If the result string from system's sprintf 
						//starts with '-', then take it as if '-' is filled by us and
						//only the substring after '-' is from system's sprintf.
						// This makes counting of zeros to pad for "integer precision" 
						//a bit simpler, because how many zeros to pad has nothing
						//to do with the preceding '-'.
						zero_padding_insertion_ind++;
					}
					if (zero_padding_insertion_ind+1 < str_arg_l &&
						tmp[zero_padding_insertion_ind]   == _T('0') &&
						(tmp[zero_padding_insertion_ind+1] == _T('x') || tmp[zero_padding_insertion_ind+1] == _T('X')) 
						) {
						//[2006-10-03]Chj: If the result string from system's sprintf 
						//starts with "0x", then take it as if "0x" is filled by us and
						//only the substring after "0x" is from system's sprintf.
						// But, can system's sprintf give out a formatted string starting with "0x"??
						zero_padding_insertion_ind += 2;
					}
				}
				
				Int num_of_digits = str_arg_l - zero_padding_insertion_ind;
				if (alternate_form && fmt_spec == _T('o')
					/* unless zero is already the first character */
					&& !(zero_padding_insertion_ind < str_arg_l && tmp[zero_padding_insertion_ind] == _T('0'))
					) 
				{        /* assure leading zero for alternate-form octal numbers */
					if (!precision_specified || precision < num_of_digits+1) {
					 /* precision is increased to force the first character to be zero,
						except if a zero value is formatted with an explicit precision
						of zero */
					  precision = num_of_digits+1; precision_specified = true;
					}
				}
				
				/* zero padding to specified precision? */
				if (num_of_digits < precision) 
					number_of_zeros_to_pad = precision - num_of_digits;
				
				/* zero padding to specified minimal field width? */
				if (!justify_left && zero_padding) {
					int n = min_field_width - (str_arg_l+number_of_zeros_to_pad);
					if (n > 0) 
						number_of_zeros_to_pad += n;
				}
				break;
			} // case 'd', 'u', 'o', 'x', 'X', 'p', 'f', 'g', 'G', 'e', 'E' 

		case _T('k'): // lower-case 'k'
			{
				const TCHAR *hyphen = va_arg(ap, TCHAR*); 
				CTI_SETVAL(cti, val_ptr, hyphen);
				ctipack_output(ctipack, NULL, 0);

				mm_strncpy_(mdd_hyphens, hyphen, mmquan(mdd_hyphens), true);

				p++; continue; // v5.0 updated
			}
		case _T('K'): // upper-case 'K'
			{
				const TCHAR *brackets = va_arg(ap, TCHAR*);
				CTI_SETVAL(cti, val_ptr, brackets);
				ctipack_output(ctipack, NULL, 0);

				int Klen = TMM_strlen(brackets);
				int leftlen = Klen/2;
				mm_strncpy_(mdd_left, brackets, mmquan(mdd_left), true);
				if(leftlen<mmquan(mdd_left))
					mdd_left[leftlen] = _T('\0');
				else
					mdd_left[mmquan(mdd_left)-1] = _T('\0');
				mm_strncpy_(mdd_right, brackets+leftlen, mmquan(mdd_right), true);

				p++; continue;
			}
		case _T('r'): case _T('R'):  // 'r'uler parameters for bytes dump
			{
				// "%4.3r", 16    
				//  or
				// "%*.*r", min_field_width, precision, 16
				// means 16-byte columns and skip 3 columns before first-byte dump 
				// each line indents 4 spaces
				if(min_field_width>0) {
					mdd_indents = min_field_width;
					mdd_indents = Mid(0, mdd_indents, 64);
				}
				mdf_colskip = precision;

				mdf_columns = va_arg(ap, int);
				CTI_SETVAL(cti, val_int, mdf_columns);
				ctipack_output(ctipack, NULL, 0);

				if(fmt_spec==_T('R'))
					is_print_ruler = true;

				min_field_width = precision = 0; // reset
				p++; continue;
			}
		case _T('v'):
			{
				imagine_maddr = va_arg(ap, Uint64);
				CTI_SETVAL(cti, val_uint64, imagine_maddr);
				ctipack_output(ctipack, NULL, 0);

				assert(min_field_width>=0 && precision>=0);

				v_sep_width = min_field_width;
				v_adcol_width = precision; // = left-pad zeros to reach this width

				v_is_isozeros = precision_specified;
				
				p++; continue;
			}
		case _T('m'): case _T('M'): // These will do byte dump
			{	
				const void *pbytes = va_arg(ap, void*);
				CTI_SETVAL(cti, val_ptr, pbytes);
				if(!pbytes)
					pbytes = _T("");

				int dump_bytes;  // i.e. the first * in "%*.*m"
				if(min_field_specified) {
					dump_bytes = min_field_width;
				}
				else {
					if(min_field_width==0 && zero_padding) {
						dump_bytes = 0; 
							// for fmtspec "%0m", the "0" before "m" is considered
							// zeropadding instead of min_field_witdth
					}
					else {
						dump_bytes = TMM_strlen((TCHAR*)pbytes)*sizeof(TCHAR);
							// Yes, for wchar version, take pbytes as wchar_t string.
					}
				}

				int result_chars = _mm_dump_bytes(strbuf+str_l, (Int)(str_max-str_l), 
					pbytes, dump_bytes, fmt_spec==_T('M')?true:false,
					mdd_hyphens, mdd_left, mdd_right,
					mdf_columns, mdf_colskip, is_print_ruler,
					mdd_indents, 
					imagine_maddr, 
					v_sep_width, 
					Is_RequestIsoZeros(v_is_isozeros, v_adcol_width)?(-1):v_adcol_width, 
					psz_thousep,
					ctipack);
				str_l += result_chars;

				min_field_width = precision = 0; // reset (must?)
				imagine_maddr = 0;
				v_is_isozeros = false;
				
				p++; // step over the just processed conversion specifier
				continue; 
			}
		case _T('T'):
			{
				psz_THOUSEP = va_arg(ap, TCHAR*);
				CTI_SETVAL(cti, val_ptr, psz_THOUSEP);
				p++; continue;
			}
		case _T('t'):
			{
				psz_thousep = va_arg(ap, TCHAR*);
				CTI_SETVAL(cti, val_ptr, psz_thousep);
				p++; continue;
			}
		case _T('_'):
			{
				thousep_width = va_arg(ap, int);
				CTI_SETVAL(cti, val_int, thousep_width);
				p++; continue;
			}
		case _T('w'):
			{
				// If mm_wpair_magic detected(v4.4+), consume only one parameter.
				// If mm_wpair_magic not detected(v4.2+), consume two parameters.
				
				const TCHAR *dig_fmt = NULL;
				const va_list *dig_args = NULL;
				
				const mm_wpair_st *wpair = va_arg(ap, mm_wpair_st*);
				CTI_SETVAL(cti, val_ptr, wpair);
				
				if(wpair->magic==mm_wpair_magic) // magic detected
				{
					dig_fmt = (const TCHAR*)wpair->fmt; // BORING: cannot convert from 'const wchar_t *const ' to 'const TCHAR *'
					dig_args = wpair->pargs;
				}
				else
				{
					dig_fmt = (const TCHAR*)wpair;
					dig_args = va_arg(ap, va_list*); // old v4.2
				}
				//
				mmv7_st mmi2 = {0};
				mmi2.buf_output = strbuf+str_l;
				mmi2.bufsize = str_max>str_l ? str_max-str_l : 0;
				mmi2.proc_output = proc_output;
				mmi2.ctx_output = ctx_output;
				mmi2.suppress_dbginfo = mmi.suppress_dbginfo;
				mmi2.mmlevel = mylevel;
				//
				int fills = mm_vsnprintf_v7(mmi2, 
					dig_fmt, *(va_list*)dig_args); // extra (va_list*) type-conversion(drop const) to make gcc 4.8 x64 happy.
				str_l += fills;
				p++;
				continue;
			}
		case _T('F'): // inject function call
			{
				FUNC_mmF_pair *func = NULL;
				void *func_param = NULL;

				const mmF_pair_st *fpair = va_arg(ap, mmF_pair_st*);
				CTI_SETVAL(cti, val_ptr, fpair);

				if(fpair->magic==mm_fpair_magic)
				{
					func = fpair->func;
					func_param = fpair->func_param;
				}

				if(func)
				{
					// Before calling back %F, add trailing NUL so that callee can do strlen()
					// Callee can verify: 	assert(bufsize==0 || buf[0]==0);
					if(str_max>str_l)
						strbuf[str_l] = _T('\0');
					else if(str_max>0)
						strbuf[str_max-1] = _T('\0');

					mmv7_st mmii = {0};
					mmii.buf_output = strbuf+str_l;
					mmii.bufsize = str_max>str_l ? str_max-str_l : 0;
					mmii.proc_output = proc_output;
					mmii.ctx_output = ctx_output;
					mmii.suppress_dbginfo = mmi.suppress_dbginfo;
					mmii.mmlevel = mylevel;
					mmii.pstock = strbuf;
					int fills = func(func_param, mmii);

					str_l += _MAX_(0, fills);
				}
				
				p++;
				continue;
			}
		default: /* unrecognized conversion specifier, keep format string as-is*/
			{
				zero_padding = false;  /* turn zero padding off for non-numeric converts. */

				justify_left = true; min_field_width = 0;                /* reset flags */

//#if defined(LINUX_COMPATIBLE) //[2006-10-03]Chj: I think the Linux-like behavior is more faithful to user.
				/* keep the entire format string unchanged */
				str_arg = starting_p; 
				str_arg_l = (Int)(p - starting_p);
				/* well, not exactly so for Linux, which does something in-between,
				* and I don't feel an urge to imitate it: "%+++++hy" -> "%+y"  */
// #else
// 				/* discard the unrecognized conversion, just keep *
// 				* the unrecognized conversion character          */
// 				str_arg = p; str_arg_l = 0;
// #endif
				if (*p) {
					str_arg_l++;  /* include invalid conversion specifier unchanged
									 if not at end-of-string */
				}

				cti.has_width = cti.has_precision = false; cti.width = cti.precision = 0;
				memset(cti.placehldr, 0, sizeof(cti.placehldr));

				break;
			}
		}} // switch (fmt_spec) // BIG CHUNK OF PROCESS!! 
		//
		// Chj hint: When to use 'break' and when to use 'continue' in the above 'switch'?
		// If str_l needs update but has not been update, you should execute the code below,
		// otherwise, you can 'continue' to process next fmt-specifier.
		// Note: If you 'continue', be sure to do 'p++' first.

		if (*p) 
			p++;      /* step over the just processed conversion specifier */
		
		/* insert padding to the left as requested by min_field_width;
		  this does not include the zero padding in case of numerical conversions*/
		if (!justify_left) {                /* left padding with blank or zero */
			int n = min_field_width - (str_arg_l+number_of_zeros_to_pad);
			if (n > 0) {
				
				TCHAR cfill = zero_padding ? _T('0') : _T(' ');
				_mm_fillchars_ct(ctipack, cfill, n);

				if (str_l < str_max) {
					Int avail = (Int)(str_max-str_l);
					_mm_fillchars(strbuf+str_l, 
						cfill, 
						n>avail?avail:n);
				}
				
				str_l += n;
			}
		}

		/* zero padding as requested by the precision or by the minimal field width
		* for numeric conversions required? */
		if ((int)number_of_zeros_to_pad <= 0) {
			/* will not copy first part of numeric right now, *
			 * force it to be copied later in its entirety    */
			zero_padding_insertion_ind = 0;
		} 
		else {
			/* insert first part of numerics (sign or '0x') before zero padding */
			int n = zero_padding_insertion_ind;
			if (n > 0) {
		
				ctipack_output(ctipack, str_arg, n);
				
				if (str_l < str_max) {
					Int avail = (Int)(str_max-str_l);
					fast_memcpy(strbuf+str_l, str_arg, TMM_strmembytes(n>avail?avail:n));
				}

				str_l += n;
			}
			/* insert zero padding as requested by the precision or min field width */
			n = number_of_zeros_to_pad;
			if (n > 0) {
				
				TCHAR cfill = _T('0');
				_mm_fillchars_ct(ctipack, cfill, n);

				if (str_l < str_max) {
					Int avail = (Int)(str_max-str_l);
					_mm_fillchars(strbuf+str_l, cfill, (n>avail?avail:n));
				}
				
				str_l += n;
			}
		}
		/* insert formatted string
		* (or as-is conversion specifier for unknown conversions) */
		int n = str_arg_l - zero_padding_insertion_ind;
		if (n > 0) {
			
			ctipack_output(ctipack, str_arg+zero_padding_insertion_ind, n);

			if (str_l < str_max) {
				Int avail = (Int)(str_max-str_l);
				fast_memcpy(strbuf+str_l, str_arg+zero_padding_insertion_ind,
					TMM_strmembytes(n>avail?avail:n));
			}

			str_l += n;
		}
		
		/* insert right padding */
		if (justify_left) {          /* right blank padding to the field width */
			int n = min_field_width - (str_arg_l+number_of_zeros_to_pad);
			if (n > 0) {
				
				TCHAR cfill = _T(' ');
				_mm_fillchars_ct(ctipack, cfill, n);
				
				if (str_l < str_max) {
					Int avail = (Int)(str_max-str_l); //Chj
					_mm_fillchars(strbuf+str_l, cfill, (n>avail?avail:n));
				}

				str_l += n;
			}
		}
	} // while(*p)

	/* make sure the string is null-terminated
     even at the expense of overwriting the last character
     (shouldn't happen, but just in case) */
	if (str_max > 0) { 
		strbuf[str_l <= str_max-1 ? str_l : str_max-1] = _T('\0');
	}

	/* Return the number of characters formatted (excluding trailing null
	 * character), that is, the number of characters that would have been
	 * written to the buffer if it were large enough.
	 *
	 * The value of str_l should be returned, but str_l is of unsigned type
	 * size_t, and snprintf is int, possibly leading to an undetected
	 * integer overflow, resulting in a negative return value, which is illegal.
	 * Both XSH5 and ISO C99 (at least the draft) are silent on this issue.
	 * Should errno be set to EOVERFLOW and EOF returned in this case???
	 */
	return (int) str_l;
}


int 
mm_snprintf(TCHAR *str, size_t str_m, const TCHAR *fmt, /*args*/ ...) 
{
	va_list ap;
	int str_l;

	va_start(ap, fmt);
	str_l = mm_vsnprintf(str, str_m, fmt, ap);
	va_end(ap);
	return str_l;
}


int mm_asprintf(TCHAR **ptr, const TCHAR *fmt, /*args*/ ...) 
{
	va_list ap;
	size_t str_m;
	int str_l;

	*ptr = NULL;
	va_start(ap, fmt);                            /* measure the required size */
	str_l = mm_vsnprintf(NULL, (size_t)0, fmt, ap);
	va_end(ap);
	
	//assert(str_l >= 0);        /* possible integer overflow if str_m > INT_MAX */
	if(str_l<0)
		return -2;

	size_t need_bytes = (str_m = (size_t)str_l + 1) * sizeof(TCHAR);
#ifdef USE_CPP_NEW
	*ptr = (TCHAR*)new unsigned char[need_bytes];
#else
	*ptr = (TCHAR*) malloc( need_bytes );
#endif 
	if (*ptr == NULL) {
		str_l = -1; 
	}
	else {
		int str_l2;
		va_start(ap, fmt);
		str_l2 = mm_vsnprintf(*ptr, str_m, fmt, ap);
		va_end(ap);
		assert(str_l2 == str_l);
	}
	return str_l;
}

int mm_vasprintf(TCHAR **ptr, const TCHAR *fmt, va_list ap) 
{
	size_t str_m;
	int str_l;

	*ptr = NULL;
	{ 
		va_list ap2;
		va_copy(ap2, ap);  /* don't consume the original ap, we'll need it again */
		str_l = mm_vsnprintf(NULL, (size_t)0, fmt, ap2);/*get required size*/
		va_end(ap2);
	}
	
	//assert(str_l >= 0);        /* possible integer overflow if str_m > INT_MAX */
	if(str_l<0)
		return -2;

	size_t need_bytes = (str_m = (size_t)str_l + 1) *sizeof(TCHAR);
#ifdef USE_CPP_NEW
	*ptr = (TCHAR*)new unsigned char[need_bytes];
#else
	*ptr = (TCHAR*) malloc( need_bytes );
#endif 
	if (*ptr == NULL) { 
		str_l = -1; 
	}
	else {
		int str_l2 = mm_vsnprintf(*ptr, str_m, fmt, ap);
		assert(str_l2 == str_l);
	}
	return str_l;
}

int mm_asnprintf (TCHAR **ptr, size_t str_m, const TCHAR *fmt, /*args*/ ...) 
{
	va_list ap;
	int str_l;

	*ptr = NULL;
	va_start(ap, fmt);                            /* measure the required size */
	str_l = mm_vsnprintf(NULL, (size_t)0, fmt, ap);
	va_end(ap);

	//assert(str_l >= 0);        /* possible integer overflow if str_m > INT_MAX */
	if(str_l<0)
		return -2;

	if ((size_t)str_l + 1 < str_m) 
		str_m = (size_t)str_l + 1;      /* truncate */

	/* if str_m is 0, no buffer is allocated, just set *ptr to NULL */
	if (str_m == 0) {  /* not interested in resulting string, just return size */
	} 
	else {
		size_t need_bytes = str_m * sizeof(TCHAR);
#ifdef USE_CPP_NEW
		*ptr = (TCHAR*)new unsigned char[need_bytes];
#else
		*ptr = (TCHAR*) malloc( need_bytes );
#endif 
		if (*ptr == NULL) {
			str_l = -1; 
		}
		else {
		  int str_l2;
		  va_start(ap, fmt);
		  str_l2 = mm_vsnprintf(*ptr, str_m, fmt, ap);
		  va_end(ap);
		  assert(str_l2 == str_l);
		}
	}
  return str_l;
}

int mm_vasnprintf (TCHAR **ptr, size_t str_m, const TCHAR *fmt, va_list ap) 
{
	int str_l;

	*ptr = NULL;
	{ 
		va_list ap2;
		va_copy(ap2, ap);  /* don't consume the original ap, we'll need it again */
		str_l = mm_vsnprintf(NULL, (size_t)0, fmt, ap2);/*get required size*/
		va_end(ap2);
	}

	//assert(str_l >= 0);        /* possible integer overflow if str_m > INT_MAX */
	if(str_l<0)
		return -2;

	if ((size_t)str_l + 1 < str_m) 
		str_m = (size_t)str_l + 1;      /* truncate */
	
	/* if str_m is 0, no buffer is allocated, just set *ptr to NULL */
	if (str_m == 0) {  /* not interested in resulting string, just return size */
	} 
	else {
		size_t need_bytes = str_m * sizeof(TCHAR);
#ifdef USE_CPP_NEW
		*ptr = (TCHAR*)new unsigned char[need_bytes];
#else
		*ptr = (TCHAR*) malloc( need_bytes );
#endif 
		if (*ptr == NULL) {
			str_l = -1; 
		}
		else {
			int str_l2 = mm_vsnprintf(*ptr, str_m, fmt, ap);
			assert(str_l2 == str_l);
		}
	}
	return str_l;
}


int 
mm_snprintf_am(TCHAR * &pbuf, int &bufremain, const TCHAR *fmt, ...)
{
	if(bufremain<=0)
		return -1;
	
	va_list args;
	va_start(args, fmt);
	int allchars = mm_vsnprintf(pbuf, bufremain, fmt, args);
	va_end(args);
	
	if(allchars<bufremain)
	{
		pbuf += allchars;
		bufremain -= allchars;
	}
	else
	{
		pbuf += (bufremain-1);
		bufremain = 1;
	}
	
	return allchars;
}



int 
mmsnprintf_IsFloatingType(TCHAR fmt_spec)
{
	if(fmt_spec==_T('f')||fmt_spec==_T('g')||fmt_spec==_T('G')||fmt_spec==_T('e')||fmt_spec==_T('E'))
		return 1; // yes
	else
		return 0; // no
}

//////////////////////////////////////////////////////////////////////////


int 
mm_strcat(TCHAR *dest, size_t bufsize, const TCHAR *fmt, ...)
{
	va_list args;
	va_start(args, fmt);

	mm_wpair_st wpair(fmt, &args);
	int ret = mm_snprintf(dest, bufsize, _T("%s%w"), dest, &wpair);

	va_end(args);
	
	return ret;
}

int 
mm_printf_ct(FUNC_mmct_output proc_output, void *ctx_output, const TCHAR *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	
	mmv7_st mmi = {0};
	mmi.proc_output = proc_output;
	mmi.ctx_output = ctx_output;
	int ret = mm_vsnprintf_v7(mmi, fmt, args);
	
	va_end(args);
	return ret;
}

int mm_snprintf_v7(const mmv7_st &mmi, const TCHAR *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	
	int ret = mm_vsnprintf_v7(mmi, fmt, args);
	
	va_end(args);
	return ret;

}

int in_snprintf(TCHAR *buf, size_t bufsize, const TCHAR *fmt, ...)
{
	mmv7_st mmi = {0};
	mmi.buf_output = buf;
	mmi.bufsize = bufsize;
	mmi.suppress_dbginfo = true; // key!

	va_list args;
	va_start(args, fmt);

	int ret = mm_vsnprintf_v7(mmi, fmt, args);

	va_end(args);
	return ret;
}

