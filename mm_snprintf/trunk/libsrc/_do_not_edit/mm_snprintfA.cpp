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
 * 1999-04-13	V1.2  Mark Martinec <mark.martinec@ijs.si>
 *		- support synonyms %D=%ld, %U=%lu, %O=%lo;
 *		- speed up the case of long format string with few conversions;
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
 *      - New function mm_snprintf_am(), updating pbuf and bufsize as a  
 *        convenient way for concatenating formatted string.
 */


/* Define SNPRINTF_LONGLONG_SUPPORT if you want to support
 * data type (long long int) and length modifier 'll' (e.g. %lld).
 * If undefined, 'll' is recognized but treated as a single 'l'.
 *
 */

// Some configuration macros:

// USE_CPP_NEW:
//	Use C++ new/delete for asprintf memory allocation.
//	If not defined, use malloc/free.


#include <assert.h>
#include <stdio.h>
#include <stdlib.h>		// for malloc
#include <stdarg.h>

#include <ps_Tstrdef.h> 
	//Note:
	// 1. This, on MSVC6, may include <tchar.h>, so list it before <ps_TCHAR.h>
	//otherwise, __T redefinition occurs.
	// 2. This file #defines va_copy macro if it is not defined in <stdarg.h>
	//so you MUST #include it after <stdarg.h>

//////////////////////////////
#include <ps_TCHAR.h>
#include <mm_snprintf.h>
#include "internal.h"
#include "mm_psfunc.h"

#define DLL_AUTO_EXPORT_STUB
extern"C" void mmsnprintf_lib__mm_snprintfA__DLL_AUTO_EXPORT_STUB(void){}

#ifdef NO_assert // specific for this library
# undef assert
# define assert(a) 
#endif


#define SNPRINTF_LONGLONG_SUPPORT

/* ============================================= */
/* NO USER SERVICABLE PARTS FOLLOWING THIS POINT */
/* ============================================= */

#if defined(NEED_ASPRINTF) || defined(NEED_ASNPRINTF) || defined(NEED_VASPRINTF) || defined(NEED_VASNPRINTF)
# if !defined(PREFER_PORTABLE_SNPRINTF)
# define PREFER_PORTABLE_SNPRINTF
# endif
#endif

//#include <sys/types.h> //Chj commented it, for ARMCC does not have this.
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <assert.h>
//#include <errno.h> //[2008-05-16]Chj: WinCE don't have this.
#include <ctype.h>

#define Min(a,b) ((a) < (b) ? (a) : (b))
#define Max(a,b) ((a) > (b) ? (a) : (b))
#define Mid(a, c, b) Min( Max((a),(c)), (b) ) // please ensure a<b

enum // byte dump max values
{
	bdmax_hyphen = 8,
	bdmax_left = 4,
	bdmax_right = 4
};

#define fast_memcpy memcpy
//#define fast_memset memset
void mm_chrset(TCHAR *p, TCHAR c, int count)
{
	int i; 
	for(i=0; i<count; i++) 
		p[i] = c;
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

#define mmquan(array) ((int)(sizeof(array)/sizeof(array[0])))


static char credits[] = "\n\
@(#)snprintf.c: Mark Martinec, <mark.martinec@ijs.si>, and Chen Jun.\n\
@(#)snprintf.c: Copyright 1999, Mark Martinec. Frontier Artistic License applies.\n\
@(#)snprintf.c: http://www.ijs.si/software/snprintf/\n";


int mm_vsnprintf(TCHAR *str, size_t str_m, const TCHAR *fmt, va_list ap) 
{

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
	void *imagine_maddr = 0;

	while (*p) 
	{
		if (*p != _T('%')) 
		{
		   /* if (str_l < str_m) str[str_l++] = *p++;    -- this would be sufficient */
		   /* but the following code achieves better performance for cases
			* where format string is long and contains few conversions */
			const TCHAR *q = TMM_strchr(p, _T('%'));
				//[2009-05-20]Chj Note: Don't try to search from p+1 !!
				//If p points to an MBCS char, that will break the MBCS char and possible swallow the % following that MBCS sequence.

			size_t n = !q ? TMM_strlen(p) : (q-p);
			if (str_l < str_m) {
				size_t avail = str_m-str_l;
				fast_memcpy(str+str_l, p, TMM_strmembytes(n>avail?avail:n));
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
			//[2006-10-03]Chj: Be aware of the overflow of this buffer.
			//User can assign a very high precision(e.g. "%.400f") to format a number,
			//which cannot be afforded by this small buffer. Therefore, we will
			//later limit the precision before invoking system's sprintf.

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

		TCHAR fmt_spec = _T('\0');
		/* current conversion specifier character */

#ifndef _UNICODE
		str_arg = credits;/* just to make compiler happy (defined but not used)*/
#endif

		// here, p points to '%'
		str_arg = NULL;
		starting_p = p; 
		p++;  /* skip '%' */
		/* parse flags */
		while (*p == _T('0') || *p == _T('-') || *p == _T('+') ||
			 *p == _T(' ') || *p == _T('#') || *p == _T('\'')) 
		{
			switch (*p) {
			case _T('0'): zero_padding = true; break;
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

		//
		// NOW, THE LENGTHY PROCESS OF fmt_spec(also called "type" in MSDN) STARTS.
		//
		
		/* common synonyms: fmt_spec(also called "type" in MSDN): */
		switch (fmt_spec) {
		case _T('i'): 
			fmt_spec = _T('d'); break; //[2006-10-02]Chj: Why not length_modifier = 'l'; ?
		case _T('D'): 
			fmt_spec = _T('d'); 
			length_modifier = _T('l'); break;
		case _T('U'): 
			fmt_spec = _T('u'); 
			length_modifier = _T('l'); break;
		case _T('O'): 
			fmt_spec = _T('o'); 
			length_modifier = _T('l'); break;
		default: break;
		}

		/* get parameter value, do initial processing */
		switch (fmt_spec) // BIG CHUNK OF PROCESS!!
		{
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

				str_arg_l = 1; // Chj: ?? !! and following

				if(fmt_spec==_T('%')) {
					str_arg = p; // *p is '%' here.
				}
				if(fmt_spec==_T('c')) {
					int j = va_arg(ap, int);
					uchar_arg = (TCHAR) j;   /* standard demands unsigned char */
					str_arg = (TCHAR*) &uchar_arg;
				}
				if(fmt_spec==_T('s')){
					str_arg = va_arg(ap, TCHAR*);
					if (!str_arg) 
						str_arg_l = 0;
				/* make sure not to address string beyond the specified precision !!! */
					else if (!precision_specified) 
						str_arg_l = TMM_strlen(str_arg);
				/* truncate string if necessary as requested by precision */
					else if (precision == 0) 
						str_arg_l = 0; //Chj: Yes, no characters from the string will be printed if `precision' is zero.
					else { // `precision' specified and > 0 
						Int lenInput = TMM_strlen(str_arg);
						str_arg_l = lenInput<precision ? lenInput : precision;
					}
				}
				break; // end of process for type '%','c','s' .
			}

		case _T('d'): case _T('u'): case _T('o'): case _T('x'): case _T('X'): case _T('p'): 
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

				double double_arg = 0;
				int isFloatingType = mmsnprintf_IsFloatingType(fmt_spec);

#ifdef SNPRINTF_LONGLONG_SUPPORT
				__int64 long_long_arg = 0;
				unsigned __int64 ulong_long_arg = 0;
				  /* only defined for length modifier ll */
#endif
				if (fmt_spec == _T('p')) 
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
					if (ptr_arg != NULL) 
						arg_sign = 1;
				} 
				else if (fmt_spec == _T('d')) 
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
						if      (int_arg > 0) arg_sign =  1;
						else if (int_arg < 0) arg_sign = -1;
						break;
					case _T('l'):
						long_arg = va_arg(ap, long int);
						if      (long_arg > 0) arg_sign =  1;
						else if (long_arg < 0) arg_sign = -1;
						break;
#ifdef SNPRINTF_LONGLONG_SUPPORT
					case _T('2'):
						long_long_arg = va_arg(ap, __int64);
						if      (long_long_arg > 0) arg_sign =  1;
						else if (long_long_arg < 0) arg_sign = -1;
						break;
#endif
					}
				} 
				else if(!isFloatingType) /* unsigned */
				{
					switch (length_modifier) 
					{
					case _T('\0'):
					case _T('h'):
						uint_arg = va_arg(ap, unsigned int);
						if (uint_arg) arg_sign = 1;
						break;
					case _T('l'):
						ulong_arg = va_arg(ap, unsigned long int);
						if (ulong_arg) arg_sign = 1;
						break;
#ifdef SNPRINTF_LONGLONG_SUPPORT
				  case _T('2'):
						ulong_long_arg = va_arg(ap, unsigned __int64);
						if (ulong_long_arg) arg_sign = 1;
						break;
#endif
					}
				}
				else // floating point number
				{
					double_arg = va_arg(ap, double);
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
					zero_padding = false;

				if (fmt_spec == _T('d') || isFloatingType) {
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
				else if (alternate_form) {
					if (arg_sign != 0 && (fmt_spec == _T('x') || fmt_spec == _T('X') || fmt_spec==_T('p')) )
					{ 
						/* [2007-03-31]Chj: On MSVCRT 6.0(32-bit):
							printf("[%+#p]", (void*)16);
						 outputs:
							[+0X00000010]
						 Now, I make mm_snprintf follow this scheme(check for fmt_spec=='p')
						 but with a minor change: outpout small 'x' instead of the capital one.
						 That is, mm_snprintf outputs:
							[+0x00000010]
						 Using small 'x' seems more reasonable, since it corresponds to the
						 small type-char 'p'.
						*/
						tmp[str_arg_l++] = _T('0'); 
						tmp[str_arg_l++] = fmt_spec==_T('p') ? _T('x') : fmt_spec; 
					}
					/* alternate form should have no effect for p conversion, but ... */
				}

				zero_padding_insertion_ind = str_arg_l;
					//Chj: This is the position in tmp[] where real value is formated and stored by sprintf.

				if (!precision_specified) 
				{
					if(isFloatingType)
						precision = 6;	// default precision is 6 floating point numbers
					else
						precision = 1;   /* default precision is 1 for integer types */
				}

				if (precision == 0 && arg_sign == 0
// #if defined(LINUX_COMPATIBLE)
// 					&& fmt_spec != 'p'
// 				 /* HPUX 10 man page claims: With conversion character p the result of
// 				  * converting a zero value with a precision of zero is a null string.
// 				  * Actually HP returns all zeroes, and Linux returns(outputs) "(nil)". */
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
					// >>> Use system's sprintf to format numeric string in tmp[].

					TCHAR f[32]; int f_l = 0; // `f' used as a temp format string for system's sprintf
					f[f_l++] = _T('%');    /* construct a simple format string for sprintf */
					
					if(isFloatingType)
					{
						// Limit the precision so that not to overflow tmp[].
						const int MaxPrecision = mmquan(tmp)-16;

						f_l += TMM_sprintf(f+f_l, _T(".%d"), precision<MaxPrecision ? precision : MaxPrecision); 
							//For floating point number, we leave precision processing to system's sprintf.

						precision_specified = false; precision = 0;
							// !!! As if we have not specified the precision 
							//so that the "precision processing for integers" will not take place.
						
						// length_modifier is ignored for floating type.
					}
					else if (!length_modifier) { }
					else if (length_modifier==_T('2')) { // to format a 64-bit integer
						/* [2006-10-02]Chj: We use an external routine to obtain the 
						 format-string for 64-bit integer, because it differs on 
						 different systems:
						   * On Microsoft VC runtime: "%I64d" or "%I64u" or "%I64x"
						   * On GNU glibc:   "%lld" or "%llu" or "%llx"
						   * On ARM SDT/ADS: "%lld" or "%llu" or "%llx"
						*/
						char tmpI64fmt[8]; // Yes, not TCHAR here!
						int lenI64fmt = mmps_i64_type_prefix(tmpI64fmt);
						for(int i=0; i<lenI64fmt; i++)
							f[f_l++] = tmpI64fmt[i];
					}
					else 
						f[f_l++] = length_modifier;

					f[f_l++] = fmt_spec; f[f_l++] = '\0';

					if (fmt_spec == _T('p')) {
						str_arg_l += TMM_sprintf(tmp+str_arg_l, f, ptr_arg);
					}
					else if (fmt_spec == _T('d')) {  /* signed */
						switch (length_modifier) {
						case _T('\0'):
						case _T('h'): str_arg_l+=TMM_sprintf(tmp+str_arg_l, f, int_arg);  break;
						case _T('l'): str_arg_l+=TMM_sprintf(tmp+str_arg_l, f, long_arg); break;
#ifdef SNPRINTF_LONGLONG_SUPPORT
						case _T('2'): str_arg_l+=TMM_sprintf(tmp+str_arg_l,f,long_long_arg); break;
#endif
						}
					} 
					else if (!isFloatingType){  /* unsigned */
						switch (length_modifier) {
						case '\0':
						case 'h': str_arg_l+=TMM_sprintf(tmp+str_arg_l, f, uint_arg);  break;
						case 'l': str_arg_l+=TMM_sprintf(tmp+str_arg_l, f, ulong_arg); break;
#ifdef SNPRINTF_LONGLONG_SUPPORT
						case '2': str_arg_l+=TMM_sprintf(tmp+str_arg_l,f,ulong_long_arg); break;
#endif
						}
					}
					else // floating type
					{
						str_arg_l += TMM_sprintf(tmp+str_arg_l, f, double_arg);
					}

					// <<< Use system's sprintf to format integer string in tmp[].
				 
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
				mm_strncpy_(mdd_hyphens, hyphen, mmquan(mdd_hyphens), true);
				break;
			}
		case _T('K'): // upper-case 'K'
			{
				const TCHAR *brackets = va_arg(ap, TCHAR*);
				int Klen = TMM_strlen(brackets);
				int leftlen = Klen/2;
				mm_strncpy_(mdd_left, brackets, mmquan(mdd_left), true);
				if(leftlen<mmquan(mdd_left))
					mdd_left[leftlen] = _T('\0');
				else
					mdd_left[mmquan(mdd_left)-1] = _T('\0');
				mm_strncpy_(mdd_right, brackets+leftlen, mmquan(mdd_right), true);
				break;
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

				if(fmt_spec==_T('R'))
					is_print_ruler = true;

				min_field_width = precision = 0; // reset
				break;
			}
		case _T('v'):
			{
				imagine_maddr = va_arg(ap, void*);
				break;
			}
		case _T('m'): case _T('M'): // These will do byte dump
			{	
				const void *pbytes = va_arg(ap, void*);
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

				int result_chars = mm_dump_bytes(str+str_l, (Int)(str_m-str_l), 
					pbytes, dump_bytes, fmt_spec==_T('M')?true:false,
					mdd_hyphens, mdd_left, mdd_right,
					mdf_columns, mdf_colskip, is_print_ruler,
					mdd_indents, imagine_maddr);
				str_l += result_chars;

				min_field_width = precision = 0; // reset (must?)
				imagine_maddr = 0;
				p++; // step over the just processed conversion specifier
				continue; 
			}
		case _T('w'):
			{
				// If mm_wpair_magic detected(v4.4+), consume only one parameter.
				// If mm_wpair_magic not detected(v4.2+), consume two parameters.
				
				const TCHAR *dig_fmt = NULL;
				const va_list *dig_args = NULL;
				
				const mm_wpair_st *wpair = va_arg(ap, mm_wpair_st*);
				
				if(wpair->magic==mm_wpair_magic) // magic detected
				{
					dig_fmt = (const TCHAR*)wpair->fmt; // BORING: cannot convert from 'const wchar_t *const ' to 'const TCHAR *'
					dig_args = wpair->pargs;
				}
				else
				{
					dig_fmt = (const TCHAR*)wpair;
					dig_args = va_arg(ap, va_list*); 
				}
				int fills = mm_vsnprintf(str+str_l, 
					str_m>str_l ? str_m-str_l : 0, 
					dig_fmt, *(va_list*)dig_args); // extra (va_list*) type-conversion(drop const) to make gcc 4.8 x64 happy.
				str_l += fills;
				p++;
				continue;
			}
		default: /* unrecognized conversion specifier, keep format string as-is*/
			{
				zero_padding = false;  /* turn zero padding off for non-numeric converts. */

				justify_left = true; min_field_width = 0;                /* reset flags */

//#if defined(LINUX_COMPATIBLE) //[2006-10-03]Chj: I think the Linux-like behavior is more faithful to user.
				/* keep the entire format string unchanged */
				str_arg = starting_p; str_arg_l = (Int)(p - starting_p);
				/* well, not exactly so for Linux, which does something inbetween,
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
				break;
			}
		} // switch (fmt_spec) // BIG CHUNK OF PROCESS!! 
		
		// chj hint: When to use 'break' and when to use 'continue' in the above 'switch'?
		// If str_l has not been update, you should execute the code below,
		// otherwise, you can 'continue' to process next fmt-specifier.
		// Note: If you 'continue', be sure to prepend 'p++' .

		if (*p) 
			p++;      /* step over the just processed conversion specifier */
		
		/* insert padding to the left as requested by min_field_width;
		  this does not include the zero padding in case of numerical conversions*/
		if (!justify_left) {                /* left padding with blank or zero */
			int n = min_field_width - (str_arg_l+number_of_zeros_to_pad);
			if (n > 0) {
				if (str_l < str_m) {
					Int avail = (Int)(str_m-str_l);
					mmsnprintf_fillchar(str+str_l, 
						zero_padding ? _T('0') : _T(' '), 
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
				if (str_l < str_m) {
					Int avail = (Int)(str_m-str_l);
					fast_memcpy(str+str_l, str_arg, TMM_strmembytes(n>avail?avail:n));
				}
				str_l += n;
			}
			/* insert zero padding as requested by the precision or min field width */
			n = number_of_zeros_to_pad;
			if (n > 0) {
				if (str_l < str_m) {
					Int avail = (Int)(str_m-str_l);
					mm_chrset(str+str_l, _T('0'), (n>avail?avail:n));
				}
				str_l += n;
			}
		}
		/* insert formatted string
		* (or as-is conversion specifier for unknown conversions) */
		int n = str_arg_l - zero_padding_insertion_ind;
		if (n > 0) {
			if (str_l < str_m) {
				Int avail = (Int)(str_m-str_l);
				fast_memcpy(str+str_l, str_arg+zero_padding_insertion_ind,
					TMM_strmembytes(n>avail?avail:n));
			}
			str_l += n;
		}
		
		/* insert right padding */
		if (justify_left) {          /* right blank padding to the field width */
			int n = min_field_width - (str_arg_l+number_of_zeros_to_pad);
			if (n > 0) {
				if (str_l < str_m) {
					Int avail = (Int)(str_m-str_l); //Chj
					mmsnprintf_fillchar(str+str_l, _T(' '), (n>avail?avail:n));
				}
				str_l += n;
			}
		}
	} // while(*p)

	/* make sure the string is null-terminated
     even at the expense of overwriting the last character
     (shouldn't happen, but just in case) */
	if (str_m > 0) { 
		str[str_l <= str_m-1 ? str_l : str_m-1] = _T('\0');
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
mm_snprintf_am(TCHAR * &pbuf, int &bufsize, const TCHAR *fmt, ...)
{
	if(bufsize<=0)
		return 1<<31; // the smallest int
	
	va_list args;
	va_start(args, fmt);
	
	int allchars = mm_vsnprintf(pbuf, bufsize, fmt, args);
	
	// move pbuf and bufsize within user's range
	int ret = 0;
	
	if(allchars<bufsize)
	{
		ret = 0;

		pbuf += allchars;
		bufsize -= allchars;
	}
	else
	{
		ret = bufsize-1 - allchars;

		pbuf += (bufsize-1);
		bufsize = 1;
		
		// Example: Returning -3 means 3 TCHARs is truncated due to insufficient bufsize.
	}
	
	va_end(args);
	return ret;
}



int 
mmsnprintf_IsFloatingType(TCHAR fmt_spec)
{
	if(fmt_spec==_T('f')||fmt_spec==_T('g')||fmt_spec==_T('G')||fmt_spec==_T('e')||fmt_spec==_T('E'))
		return 1; // yes
	else
		return 0; // no
}

void 
mmsnprintf_fillchar(TCHAR *pbuf, TCHAR c, int n)
{
	int i;
	for(i=0; i<n; i++)
		pbuf[i] = c;
}

//////////////////////////////////////////////////////////////////////////

struct mmfill_st
{
	TCHAR *pbuf;
	int bufmax;
	int produced; // may be larger than bufmax
};

void 
mmfill_fill_chars(mmfill_st &f, TCHAR c, int n)
{
	// fills f.pbuf until f.pbuf[] reaches bufmax; update f.produced by n
	int nfill = Min(n, f.bufmax-f.produced);
	if(nfill>0)
		mmsnprintf_fillchar(f.pbuf+f.produced, c, nfill);
	f.produced += n;
}

TCHAR *
mmfill_strcpy(mmfill_st &f, const TCHAR *src)
{
	// copy src to f.pbuf until f.pbuf[] reaches bufmax; update f.produced by src length
	int srclen = TMM_strlen(src);
	int nfill = Min(srclen, f.bufmax-f.produced);
	if(nfill>0)
		mm_strncpy_(f.pbuf+f.produced, src, nfill, false);

	f.produced += srclen;
	return f.pbuf + (f.produced - srclen);
}

//inline int dump_width_per_byte(int left, int right, int hyphen)
//{
//	return left+2+right+hyphen;
//}


int 
mm_dump_bytes(TCHAR *buf, int bufbytes, 
	const void *pbytes_, int dump_bytes, bool uppercase,
	const TCHAR *mdd_hyphens, const TCHAR *mdd_left, const TCHAR *mdd_right,
	int columns, int colskip, bool ruler,
	int indents, 
	const void *imagine_addr_)
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

	ruler: whether add horizontal and vertical ruler so that byte position in dump 
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

	const unsigned char *pbytes = (unsigned char*)pbytes_;

	columns = Mid(0, columns, 256);
	if(columns==0) {
		columns = 0x7FFFffff;
		ruler = false;
	}
	colskip = Mid(0, colskip, columns-1);

	const int ex2 = 2;
	int i, j;
	TCHAR tmp[20]; // don't be less than 20
	const int tmpchars = sizeof(tmp)/sizeof(tmp[0]);
	int len_hyphens = TMM_strlen(mdd_hyphens);
	int len_left = TMM_strlen(mdd_left);
	int len_right = TMM_strlen(mdd_right);

	int dump_width_perbyte = len_left+2+len_right+len_hyphens;

	const char *imagine_addr = (const char*)imagine_addr_;
	int adcol_digits = cal_adcol_digits(imagine_addr, bufbytes);

	int consumed = 0; // consumed source bytes
	mmfill_st mmfill = {buf, bufbytes, 0};

	if(ruler)
	{
		// first: indents and left ruler spaces
		mmfill_fill_chars(mmfill, _T(' '), indents); 
		mmfill_fill_chars(mmfill, _T('-'), adcol_digits+ex2); 
			// ex2 is for ": " following the address column digits

		// second: horizontal marks
		for(i=0; i<columns; i++)
		{
			mmfill_fill_chars(mmfill, _T('-'), len_left);
			TMM_sprintf(tmp, _T("%02X"), i);
			mmfill_strcpy(mmfill, tmp);
			mmfill_fill_chars(mmfill, _T('-'), len_right);

			if(i<columns-1)
				mmfill_fill_chars(mmfill, _T('-'), len_hyphens);
		}
		
		mmfill_strcpy(mmfill, _T("\n"));
	}

	const char *imagine_addr_to_print = imagine_addr-colskip;

	const TCHAR *fmthex = uppercase ? _T("%02X") : _T("%02x");

	for(;;)
	{
		// Every cycle generates one output line

		// first: indents
		mmfill_fill_chars(mmfill, _T(' '), indents);

		// second: address column
		if(ruler)
		{
			if(is64bit)
			{
				mm_snprintf(tmp, tmpchars, _T("%016llX"), (unsigned __int64)imagine_addr_to_print);
				// avoid using "%p" because Linux will prefix "0x" while MSVC does not
			}
			else
			{
				mm_snprintf(tmp, tmpchars, _T("%08X"), 
					(unsigned int)(unsigned __int64)imagine_addr_to_print
					);
				// direct cast from pointer to unsigned int is banned by gcc 4.5 x64
			}
			int droplen = (is64bit?16:8)-adcol_digits;
			mmfill_strcpy(mmfill, tmp+droplen); // leading verbose '0's dropped
			mmfill_strcpy(mmfill, _T(": "));
		}

		// third: column-skip blank area
		if(colskip>0)
		{
			mmfill_fill_chars(mmfill, _T(' '), dump_width_perbyte*colskip);
		}

		// fourth: hex dumps of current line with decoration
		int colomn_remain = Min(dump_bytes-consumed, columns-colskip);
			// columns remain for this dump line
		for(j=0; j<colomn_remain; j++)
		{
			if(mdd_left[0])
				mmfill_strcpy(mmfill, mdd_left);
			
			TMM_sprintf(tmp, fmthex, pbytes[consumed+j]);
			mmfill_strcpy(mmfill, tmp);
			
			if(mdd_right[0])
				mmfill_strcpy(mmfill, mdd_right);

			if(j<colomn_remain-1)
				mmfill_strcpy(mmfill, mdd_hyphens);
		}
		colskip = 0;

		consumed += colomn_remain;

		if(consumed==dump_bytes)
			break;

		mmfill_strcpy(mmfill, _T("\n"));
		assert(consumed<dump_bytes);

		imagine_addr_to_print += columns;
	}

	return mmfill.produced;
}

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
