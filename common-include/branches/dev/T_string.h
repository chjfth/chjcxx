#ifndef __T_string_h_20110218_
#define __T_string_h_20110218_

// This file has most of its code from MSVC6 tchar.h

#ifdef UNICODE 

/* Formatted i/o */

#define T_printf    wprintf
#define T_fprintf   fwprintf
#define T_sprintf   swprintf
//#define _sntprintf  _snwprintf  // Use mm_snprintf instead
#define T_vprintf   vwprintf
#define T_vfprintf  vfwprintf
#define T_vsprintf  vswprintf
//#define _vsntprintf _vsnwprintf
#define T_scanf     wscanf
#define T_fscanf    fwscanf
#define T_sscanf    swscanf

/* Unformatted i/o (not suggested by Chj) */
/*
#define _fgettc     fgetwc
#define _fgettchar  _fgetwchar
#define _fgetts     fgetws
#define _fputtc     fputwc
#define _fputtchar  _fputwchar
#define _fputts     fputws
#define _gettc      getwc
#define _gettchar   getwchar
#define _getts      _getws
#define _puttc      putwc
#define _puttchar   putwchar
#define _putts      _putws
#define _ungettc    ungetwc
*/

/* String conversion functions */

#define T_strtod     wcstod		// convert to double precision number
#define T_strtol     wcstol
#define T_strtoul    wcstoul
#define T_strtoi64   _wcstoi64	// Chj add
#define T_strtoui64  _wcstoui64	// Chj add
/* (Not ANSI functions, can be replaced by sprintf, mm_snprintf, strtod etc)
#define _itot       _itow
#define _ltot       _ltow
#define _ultot      _ultow
#define _ttoi       _wtoi
#define _ttol       _wtol

#define _ttoi64     _wtoi64
#define _i64tot     _i64tow
#define _ui64tot    _ui64tow
*/
/* String functions */

#define T_strcat     wcscat
#define T_strchr     wcschr
#define T_strcpy     wcscpy
#define T_strcspn    wcscspn
#define T_strlen     wcslen
#define T_strncat    wcsncat
#define T_strncpy    wcsncpy
#define T_strpbrk    wcspbrk
#define T_strrchr    wcsrchr
#define T_strspn     wcsspn
#define T_strstr     wcsstr
#define T_strtok     wcstok
/*
#define _tcsdup     _wcsdup
#define _tcsnset    _wcsnset
#define _tcsrev     _wcsrev
#define _tcsset     _wcsset
*/
#define T_strcmp     wcscmp
#define T_stricmp    _wcsicmp
//#define _tcsnccmp   wcsncmp // redundant
#define T_strncmp    wcsncmp
//#define _tcsncicmp  _wcsnicmp // redundant
#define T_strnicmp   _wcsnicmp

#define T_strcoll    wcscoll
#define T_stricoll   _wcsicoll
//#define _tscnccoll  _wcsncoll
#define T_strncoll   _wcsncoll
//#define _tcsncicoll _wcsnicoll
#define T_strnicoll  _wcsnicoll

/* Time functions */

#define _tasctime   _wasctime
#define _tctime     _wctime
//#define _tstrdate   _wstrdate
//#define _tstrtime   _wstrtime
//#define _tutime     _wutime
#define T_strftime   wcsftime

/* Environment/Path functions */

//#define _tfullpath  _wfullpath
#define _tgetenv    _wgetenv
//#define _tmakepath  _wmakepath
//#define _tputenv    _wputenv
//#define _tsearchenv _wsearchenv
//#define _tsplitpath _wsplitpath

/* Io functions */

#define T_remove    _wremove
#define T_rename    _wrename

/* ctype functions */

#define T_isalnum   iswalnum
#define T_isalpha   iswalpha
#define T_isascii   iswascii
#define T_iscntrl   iswcntrl
#define T_isdigit   iswdigit
#define T_isgraph   iswgraph
#define T_islower   iswlower
#define T_isprint   iswprint
#define T_ispunct   iswpunct
#define T_isspace   iswspace
#define T_isupper   iswupper
#define T_isxdigit  iswxdigit

#define T_toupper   towupper
#define T_tolower   towlower

/* Setlocale functions */

#define T_setlocale _wsetlocale



#else // Not UNICODE below



/* Formatted i/o */

#define T_printf    printf
#define T_fprintf   fprintf
#define T_sprintf   sprintf
//#define _sntprintf  _snwprintf  // Use mm_snprintf instead
#define T_vprintf   vprintf
#define T_vfprintf  vfprintf
#define T_vsprintf  vsprintf
//#define _vsntprintf _vsnwprintf
#define T_scanf     scanf
#define T_fscanf    fscanf
#define T_sscanf    sscanf

/* Unformatted i/o (not suggested by Chj) */
/*
#define _fgettc     fgetc
#define _fgettchar  _fgetchar
#define _fgetts     fgets
#define _fputtc     fputc
#define _fputtchar  _fputchar
#define _fputts     fputs
#define _gettc      getc
#define _gettchar   getchar
#define _getts      gets
#define _puttc      putc
#define _puttchar   putchar
#define _putts      puts
#define _ungettc    ungetc
*/

/* String conversion functions */

#define T_strtod     strtod		// convert to double precision number
#define T_strtol     strtol
#define T_strtoul    strtoul
#define T_strtoi64   _strtoi64	// Chj add
#define T_strtoui64  _strtoui64	// Chj add
/* (Not ANSI functions, can be replaced by sprintf, mm_snprintf, strtod etc)
#define _itot       _itoa
#define _ltot       _ltoa
#define _ultot      _ultoa
#define _ttoi       atoi
#define _ttol       atol

#define _ttoi64     _atoi64
#define _i64tot     _i64toa
#define _ui64tot    _ui64toa
*/
/* String functions */

#define T_strcat     strcat
#define T_strchr     strchr
#define T_strcpy     strcpy
#define T_strcspn    strcspn
#define T_strlen     strlen
#define T_strncat    strncat
#define T_strncpy    strncpy
#define T_strpbrk    strpbrk
#define T_strrchr    strrchr
#define T_strspn     strspn
#define T_strstr     strstr
#define T_strtok     strtok
/*
#define _tcsdup     _strdup
#define _tcsnset    _strnset  
#define _tcsrev     _strrev
#define _tcsset     _strset
*/
#define T_strcmp     strcmp
#define T_stricmp    _stricmp
//#define _tcsnccmp   strncmp   // redundant
#define T_strncmp    strncmp
//#define _tcsncicmp  _strnicmp // redundant
#define T_strnicmp   _strnicmp

#define T_strcoll    strcoll
#define T_stricoll   _stricoll
//#define _tscnccoll  _strncoll
#define T_strncoll   _strncoll
//#define _tcsncicoll _strnicoll
#define T_strnicoll  _strnicoll

/* Time functions */

#define _tasctime   asctime
#define _tctime     ctime
//#define _tstrdate   _strdate
//#define _tstrtime   _strtime
//#define _tutime     _wutime
#define T_strftime   strftime

/* Environment/Path functions */

//#define _tfullpath  _fullpath
#define T_getenv    getenv
//#define _tmakepath  _makepath
//#define _tputenv    _putenv
//#define _tsearchenv _searchenv
//#define _tsplitpath _splitpath

/* Io functions */

#define T_access    access
#define T_remove    remove
#define T_rename    rename

/* ctype functions (Note: some different to MSVC's, MSVC has _ismbcupper instead of isupper etc) */

#define T_isalnum   isalnum
#define T_isalpha   isalpha
#define T_isascii   isascii
#define T_iscntrl   iscntrl
#define T_isdigit   isdigit
#define T_isgraph   isgraph
#define T_islower   islower
#define T_isprint   isprint
#define T_ispunct   ispunct
#define T_isspace   isspace
#define T_isupper   isupper
#define T_isxdigit  isxdigit

#define T_toupper   toupper
#define T_tolower   tolower

/* Setlocale functions */

#define T_setlocale setlocale

#endif

#endif // #ifndef __T_string_h_20110218_

