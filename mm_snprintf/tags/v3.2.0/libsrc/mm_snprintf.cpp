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

#include <ps_TCHAR.h>
#include <mm_snprintf.h>
#include "internal.h"
#include "mm_psfunc.h"

#define DLL_AUTO_EXPORT_STUB

#ifdef NO_assert // specific for this library
# undef assert
# define assert(a) 
#endif

// Chj: redefine the function names :
#ifdef _UNICODE
#  define portable_snprintf  mm_snprintfW
#  define portable_vsnprintf mm_vsnprintfW
#  define asprintf mm_asprintfW
#  define vasprintf mm_vasprintfW
#  define asnprintf mm_asnprintfW
#  define vasnprintf mm_vasnprintfW
#else
#  define portable_snprintf  mm_snprintfA
#  define portable_vsnprintf mm_vsnprintfA
#  define asprintf mm_asprintfA
#  define vasprintf mm_vasprintfA
#  define asnprintf mm_asnprintfA
#  define vasnprintf mm_vasnprintfA
#endif


/*
 * snprintf.c - a portable implementation of snprintf
 *
 * INITIAL AUTHOR
 *   Mark Martinec <mark.martinec@ijs.si>, April 1999.
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
 * 2006-10-03  V3.0 by Chj <chenjun@mail.nlscan.com>
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
 */


/* Define SNPRINTF_LONGLONG_SUPPORT if you want to support
 * data type (long long int) and length modifier 'll' (e.g. %lld).
 * If undefined, 'll' is recognized but treated as a single 'l'.
 *
 * If the system's sprintf does not handle 'll'
 * the SNPRINTF_LONGLONG_SUPPORT must not be defined!
 *
 * This is off by default as (long long int) is a language extension.
 */

#define SNPRINTF_LONGLONG_SUPPORT

/* Define NEED_VASNPRINTF macros if you need library extension
 * routines asprintf, vasprintf, asnprintf, vasnprintf respectively,
 * and your system library does not provide them. They are all small
 * wrapper routines around portable_vsnprintf. Defining any of the four
 * NEED_VASNPRINTF macros automatically turns off NEED_SNPRINTF_ONLY
 * and turns on PREFER_PORTABLE_SNPRINTF.
 *
 * Watch for name conflicts with the system library if these routines
 * are already present there.
 *
 * NOTE: vasprintf and vasnprintf routines need va_copy() from stdarg.h, as
 * specified by C99, to be able to traverse the same list of arguments twice.
 * I don't know of any other standard and portable way of achieving the same.
 * With some versions of gcc you may use __va_copy(). You might even get away
 * with "ap2 = ap", in this case you must not call va_end(ap2) !
 *   #define va_copy(ap2,ap) ap2 = ap
 */
/* #define NEED_ASPRINTF   */
/* #define NEED_ASNPRINTF  */
/* #define NEED_VASPRINTF  */
/* #define NEED_VASNPRINTF */


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


#define fast_memcpy memcpy
//#define fast_memset memset
inline void mm_chrset(TCHAR *p, TCHAR c, int count)
{
	int i; for(i=0; i<count; i++) p[i] = c;
}


#define mm_GetEleQuan(array) ((int)(sizeof(array)/sizeof(array[0])))

/* prototypes */

#if defined(NEED_ASPRINTF)
int asprintf   (char **ptr, const char *fmt, /*args*/ ...);
#endif
#if defined(NEED_VASPRINTF)
int vasprintf  (char **ptr, const char *fmt, va_list ap);
#endif
#if defined(NEED_ASNPRINTF)
int asnprintf  (char **ptr, size_t str_m, const char *fmt, /*args*/ ...);
#endif
#if defined(NEED_VASNPRINTF)
int vasnprintf (char **ptr, size_t str_m, const char *fmt, va_list ap);
#endif


/*#if !defined(HAVE_SNPRINTF) || defined(PREFER_PORTABLE_SNPRINTF)
int portable_snprintf(char *str, size_t str_m, const char *fmt, ...);
int portable_vsnprintf(char *str, size_t str_m, const char *fmt, va_list ap);
#endif
*/
/* declarations */

static char credits[] = "\n\
@(#)snprintf.c: Mark Martinec, <mark.martinec@ijs.si>, and Chen Jun.\n\
@(#)snprintf.c: Copyright 1999, Mark Martinec. Frontier Artistic License applies.\n\
@(#)snprintf.c: http://www.ijs.si/software/snprintf/\n";

#if defined(NEED_ASPRINTF)
int asprintf(char **ptr, const char *fmt, /*args*/ ...) {
  va_list ap;
  size_t str_m;
  int str_l;

  *ptr = NULL;
  va_start(ap, fmt);                            /* measure the required size */
  str_l = portable_vsnprintf(NULL, (size_t)0, fmt, ap);
  va_end(ap);
  assert(str_l >= 0);        /* possible integer overflow if str_m > INT_MAX */
  *ptr = (char *) malloc(str_m = (size_t)str_l + 1);
  if (*ptr == NULL) { errno = ENOMEM; str_l = -1; }
  else {
    int str_l2;
    va_start(ap, fmt);
    str_l2 = portable_vsnprintf(*ptr, str_m, fmt, ap);
    va_end(ap);
    assert(str_l2 == str_l);
  }
  return str_l;
}
#endif

#if defined(NEED_VASPRINTF)
int vasprintf(char **ptr, const char *fmt, va_list ap) {
  size_t str_m;
  int str_l;

  *ptr = NULL;
  { va_list ap2;
    va_copy(ap2, ap);  /* don't consume the original ap, we'll need it again */
    str_l = portable_vsnprintf(NULL, (size_t)0, fmt, ap2);/*get required size*/
    va_end(ap2);
  }
  assert(str_l >= 0);        /* possible integer overflow if str_m > INT_MAX */
  *ptr = (char *) malloc(str_m = (size_t)str_l + 1);
  if (*ptr == NULL) { errno = ENOMEM; str_l = -1; }
  else {
    int str_l2 = portable_vsnprintf(*ptr, str_m, fmt, ap);
    assert(str_l2 == str_l);
  }
  return str_l;
}
#endif

#if defined(NEED_ASNPRINTF)
int asnprintf (char **ptr, size_t str_m, const char *fmt, /*args*/ ...) {
  va_list ap;
  int str_l;

  *ptr = NULL;
  va_start(ap, fmt);                            /* measure the required size */
  str_l = portable_vsnprintf(NULL, (size_t)0, fmt, ap);
  va_end(ap);
  assert(str_l >= 0);        /* possible integer overflow if str_m > INT_MAX */
  if ((size_t)str_l + 1 < str_m) str_m = (size_t)str_l + 1;      /* truncate */
  /* if str_m is 0, no buffer is allocated, just set *ptr to NULL */
  if (str_m == 0) {  /* not interested in resulting string, just return size */
  } else {
    *ptr = (char *) malloc(str_m);
    if (*ptr == NULL) { errno = ENOMEM; str_l = -1; }
    else {
      int str_l2;
      va_start(ap, fmt);
      str_l2 = portable_vsnprintf(*ptr, str_m, fmt, ap);
      va_end(ap);
      assert(str_l2 == str_l);
    }
  }
  return str_l;
}
#endif

#if defined(NEED_VASNPRINTF)
int vasnprintf (char **ptr, size_t str_m, const char *fmt, va_list ap) {
  int str_l;

  *ptr = NULL;
  { va_list ap2;
    va_copy(ap2, ap);  /* don't consume the original ap, we'll need it again */
    str_l = portable_vsnprintf(NULL, (size_t)0, fmt, ap2);/*get required size*/
    va_end(ap2);
  }
  assert(str_l >= 0);        /* possible integer overflow if str_m > INT_MAX */
  if ((size_t)str_l + 1 < str_m) str_m = (size_t)str_l + 1;      /* truncate */
  /* if str_m is 0, no buffer is allocated, just set *ptr to NULL */
  if (str_m == 0) {  /* not interested in resulting string, just return size */
  } else {
    *ptr = (char *) malloc(str_m);
    if (*ptr == NULL) { errno = ENOMEM; str_l = -1; }
    else {
      int str_l2 = portable_vsnprintf(*ptr, str_m, fmt, ap);
      assert(str_l2 == str_l);
    }
  }
  return str_l;
}
#endif

/*
 * If the system does have snprintf and the portable routine is not
 * specifically required, this module produces no code for snprintf/vsnprintf.
 */
#if !defined(HAVE_SNPRINTF) || defined(PREFER_PORTABLE_SNPRINTF)

int portable_vsnprintf(TCHAR *str, size_t str_m, const TCHAR *fmt, va_list ap) 
{

	size_t str_l = 0;
	const TCHAR *p = fmt;

/* In contrast with POSIX, the ISO C99 now says
 * that str can be NULL and str_m can be 0.
 * This is more useful than the old:  if (str_m < 1) return -1; */

	if (!p) 
		p = _T("");

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
				fast_memcpy(str+str_l, p, TMM_strmemsize(n>avail?avail:n));
			}
			p += n; str_l += n;

			continue;
		} 

		// Remaining, for (*p=='%') [until end of the outer-most while]

		const TCHAR *starting_p; // starting % position
			// Record the position of the '%', so that(if linux behavior is used)
			//when the type character following % is unrecognized(e.g. "%a"), 
			//the whole format specifier is output, e.g. "%8.4a" is output as
			//"%8.4a", not just "a" .

		size_t min_field_width = 0, precision = 0;
		int zero_padding = 0, precision_specified = 0, justify_left = 0;
		int alternate_form = 0;
		int force_sign = 0; // whether reserve a char space(filled with ' ' or '+') for positive indication.
		int space_for_positive = 1; // use ' ' to indicate positive number instead of using '+'
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
		size_t str_arg_l = 0;       /* natural field width of arg without padding
								   and sign */
		TCHAR uchar_arg; //unsigned char uchar_arg;
		/* unsigned char argument value - only defined for c conversion.
		   N.B. standard explicitly states the char argument for
		   the c conversion is unsigned --[2008-05-27]Chj: But it doesn't matter
		   for a function like snprintf, since we just copy the char value.
		*/

		size_t number_of_zeros_to_pad = 0;
		/* number of zeros to be inserted for numeric conversions
		   as required by the precision or minimal field width */

		size_t zero_padding_insertion_ind = 0;
		/* index into tmp[] where zero padding is to be inserted */

		TCHAR fmt_spec = _T('\0');
		/* current conversion specifier character */

#ifndef _UNICODE
		str_arg = credits;/* just to make compiler happy (defined but not used)*/
#endif
		str_arg = NULL;
		starting_p = p; p++;  /* skip '%' */
		/* parse flags */
		while (*p == _T('0') || *p == _T('-') || *p == _T('+') ||
			 *p == _T(' ') || *p == _T('#') || *p == _T('\'')) 
		{
			switch (*p) {
			case _T('0'): zero_padding = 1; break;
			case _T('-'): justify_left = 1; break;
			case _T('+'): force_sign = 1; space_for_positive = 0; break;
			case _T(' '): force_sign = 1;
			/* If both the ' ' and '+' flags appear, the ' ' flag should be ignored */
					  break;
			case _T('#'): alternate_form = 1; break;
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
				justify_left = 1; 
			}
		} 
		else if (TMM_isdigit(*p)) {
			/* size_t could be wider than unsigned int;
			   make sure we treat argument like common implementations do */
			unsigned int uj = *p++ - _T('0');
			while (TMM_isdigit(*p)) 
				uj = 10*uj + (unsigned int)(*p++ - _T('0'));
			min_field_width = uj;
		}

		/* parse precision */
		if (*p == _T('.')) {
			p++; precision_specified = 1;
			if (*p == _T('*')) {
			  int j = va_arg(ap, int);
			  p++;
			  if (j >= 0) 
				  precision = j;
			  else {
				precision_specified = 0; precision = 0;
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
						size_t lenInput = TMM_strlen(str_arg);
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

					alternate_form = 0;
					//[2006-10-03]Chj Set/clear More vars? Seems enough.
				}

				str_arg = tmp; str_arg_l = 0;

				/* NOTE:
				*   For d, i, u, o, x, and X conversions, if precision is specified,
				*   the '0' flag should be ignored. This is so with Solaris 2.6,
				*   Digital UNIX 4.0, HPUX 10, Linux, FreeBSD, NetBSD; but not with Perl.
				*/

				if (precision_specified) 
					zero_padding = 0;

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
						const int MaxPrecision = mm_GetEleQuan(tmp)-16;

						f_l += TMM_sprintf(f+f_l, _T(".%d"), precision<MaxPrecision ? precision : MaxPrecision); 
							//For floating point number, we leave precision processing to system's sprintf.

						precision_specified = 0; precision = 0;
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
				
				size_t num_of_digits = str_arg_l - zero_padding_insertion_ind;
				if (alternate_form && fmt_spec == _T('o')
					/* unless zero is already the first character */
					&& !(zero_padding_insertion_ind < str_arg_l && tmp[zero_padding_insertion_ind] == _T('0'))
					) 
				{        /* assure leading zero for alternate-form octal numbers */
					if (!precision_specified || precision < num_of_digits+1) {
					 /* precision is increased to force the first character to be zero,
						except if a zero value is formatted with an explicit precision
						of zero */
					  precision = num_of_digits+1; precision_specified = 1;
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

		default: /* unrecognized conversion specifier, keep format string as-is*/
			{
				zero_padding = 0;  /* turn zero padding off for non-numeric convers. */

				justify_left = 1; min_field_width = 0;                /* reset flags */

//#if defined(LINUX_COMPATIBLE) //[2006-10-03]Chj: I think the Linux-like behavior is more faithful to user.
				/* keep the entire format string unchanged */
				str_arg = starting_p; str_arg_l = p - starting_p;
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

		if (*p) 
			p++;      /* step over the just processed conversion specifier */
		
		/* insert padding to the left as requested by min_field_width;
		  this does not include the zero padding in case of numerical conversions*/
		if (!justify_left) {                /* left padding with blank or zero */
			int n = min_field_width - (str_arg_l+number_of_zeros_to_pad);
			if (n > 0) {
				if (str_l < str_m) {
					int /*size_t*/ avail = str_m-str_l;
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
					int /*size_t*/ avail = str_m-str_l;
					fast_memcpy(str+str_l, str_arg, TMM_strmemsize(n>avail?avail:n));
				}
				str_l += n;
			}
			/* insert zero padding as requested by the precision or min field width */
			n = number_of_zeros_to_pad;
			if (n > 0) {
				if (str_l < str_m) {
					int /*size_t*/ avail = str_m-str_l;
					mm_chrset(str+str_l, _T('0'), TMM_strmemsize(n>avail?avail:n));
				}
				str_l += n;
			}
		}
		/* insert formatted string
		* (or as-is conversion specifier for unknown conversions) */
		int n = str_arg_l - zero_padding_insertion_ind;
		if (n > 0) {
			if (str_l < str_m) {
				int /*size_t*/ avail = str_m-str_l;
				fast_memcpy(str+str_l, str_arg+zero_padding_insertion_ind,
					TMM_strmemsize(n>avail?avail:n));
			}
			str_l += n;
		}
		
		/* insert right padding */
		if (justify_left) {          /* right blank padding to the field width */
			int n = min_field_width - (str_arg_l+number_of_zeros_to_pad);
			if (n > 0) {
				if (str_l < str_m) {
					int /*size_t*/ avail = str_m-str_l; //Chj
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
#endif

int 
portable_snprintf(TCHAR *str, size_t str_m, const TCHAR *fmt, /*args*/ ...) 
{
	va_list ap;
	int str_l;

	va_start(ap, fmt);
	str_l = portable_vsnprintf(str, str_m, fmt, ap);
	va_end(ap);
	return str_l;
}

int asprintf(TCHAR **ptr, const TCHAR *fmt, /*args*/ ...) 
{
	va_list ap;
	size_t str_m;
	int str_l;

	*ptr = NULL;
	va_start(ap, fmt);                            /* measure the required size */
	str_l = portable_vsnprintf(NULL, (size_t)0, fmt, ap);
	va_end(ap);
	
	//assert(str_l >= 0);        /* possible integer overflow if str_m > INT_MAX */
	if(str_l<0)
		return -2;

	*ptr = (TCHAR *) malloc( (str_m = (size_t)str_l + 1) * sizeof(TCHAR) );
	if (*ptr == NULL) {
		str_l = -1; 
	}
	else {
		int str_l2;
		va_start(ap, fmt);
		str_l2 = portable_vsnprintf(*ptr, str_m, fmt, ap);
		va_end(ap);
		assert(str_l2 == str_l);
	}
	return str_l;
}

int vasprintf(TCHAR **ptr, const TCHAR *fmt, va_list ap) 
{
	size_t str_m;
	int str_l;

	*ptr = NULL;
	{ 
		va_list ap2;
		va_copy(ap2, ap);  /* don't consume the original ap, we'll need it again */
		str_l = portable_vsnprintf(NULL, (size_t)0, fmt, ap2);/*get required size*/
		va_end(ap2);
	}
	
	//assert(str_l >= 0);        /* possible integer overflow if str_m > INT_MAX */
	if(str_l<0)
		return -2;

	*ptr = (TCHAR *) malloc( (str_m = (size_t)str_l + 1) *sizeof(TCHAR) );
	if (*ptr == NULL) { 
		str_l = -1; 
	}
	else {
		int str_l2 = portable_vsnprintf(*ptr, str_m, fmt, ap);
		assert(str_l2 == str_l);
	}
	return str_l;
}

int asnprintf (TCHAR **ptr, size_t str_m, const TCHAR *fmt, /*args*/ ...) 
{
	va_list ap;
	int str_l;

	*ptr = NULL;
	va_start(ap, fmt);                            /* measure the required size */
	str_l = portable_vsnprintf(NULL, (size_t)0, fmt, ap);
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
		*ptr = (TCHAR *) malloc(str_m * sizeof(TCHAR));
		if (*ptr == NULL) {
			str_l = -1; 
		}
		else {
		  int str_l2;
		  va_start(ap, fmt);
		  str_l2 = portable_vsnprintf(*ptr, str_m, fmt, ap);
		  va_end(ap);
		  assert(str_l2 == str_l);
		}
	}
  return str_l;
}

int vasnprintf (TCHAR **ptr, size_t str_m, const TCHAR *fmt, va_list ap) 
{
	int str_l;

	*ptr = NULL;
	{ 
		va_list ap2;
		va_copy(ap2, ap);  /* don't consume the original ap, we'll need it again */
		str_l = portable_vsnprintf(NULL, (size_t)0, fmt, ap2);/*get required size*/
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
		*ptr = (TCHAR *) malloc(str_m * sizeof(TCHAR));
		if (*ptr == NULL) {
			str_l = -1; 
		}
		else {
			int str_l2 = portable_vsnprintf(*ptr, str_m, fmt, ap);
			assert(str_l2 == str_l);
		}
	}
	return str_l;
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
