#ifndef _MM_SNPRINTF_H_
#define _MM_SNPRINTF_H_

#ifdef __cplusplus
extern"C"{
#endif

//#define PORTABLE_SNPRINTF_VERSION_MAJOR 2 
//#define PORTABLE_SNPRINTF_VERSION_MINOR 2 
	// Now, these are defined in the source, and queried by mmsnprintf_getversion().

#include <stdio.h>
#include <stdarg.h>

#ifndef DLLEXPORT_mmsnprintf
#define DLLEXPORT_mmsnprintf
#endif

DLLEXPORT_mmsnprintf
unsigned short mmsnprintf_getversion(void);
	// Returns: Major version in higher byte and minor version in lower byte.

DLLEXPORT_mmsnprintf
int mm_snprintfA(char *buf, size_t bufsize, const char *format, ...);

DLLEXPORT_mmsnprintf
int mm_vsnprintfA(char *buf, size_t bufsize, const char *format, va_list);

DLLEXPORT_mmsnprintf
int asprintfA(char **ptr, const char *fmt, ...);
	// malloc() is called inside to hold the formatted string, the malloc-ed buffer is returned in *ptr.
	// User has to call free(*ptr) later to release the buffer.
	// Return:
	// * On success, return value is the same meaning as mm_snprintf
	// * On malloc fail, -1 is returned. 
	// * Other negative value: unknown error occurred.

DLLEXPORT_mmsnprintf
int mm_vasprintfA(char **ptr, const char *fmt, va_list ap);
	// Similar to mm_asprintf, but parameters express in va_list .

DLLEXPORT_mmsnprintf
int mm_asnprintfA(char **ptr, size_t max_alloc_size, const char *fmt, ...);
	// Similar to mm_asprintf, but mm_asnprintf will malloc a buffer size no larger than 
	// max_alloc_size of characters(that can hold max_alloc_size-1 chars and a terminating NUL-char),
	// If max_alloc_size==0, *ptr will be 0 on return.
	// The return value, like mm_snprintf, tells the total chars of the resulting string,
	// -- assuming the buffer size is sufficient. 

DLLEXPORT_mmsnprintf
int mm_vasnprintfA(char **ptr, size_t max_alloc_size, const char *fmt, va_list ap);


#ifndef __CC_NORCROFT // workaround for the old ARM SDT 2.50 compiler, who don't support wchar_t

// Unicode version of the above mm_ functions

DLLEXPORT_mmsnprintf
int mm_snprintfW(wchar_t *buf, size_t bufsize, const wchar_t *format, ...);

DLLEXPORT_mmsnprintf
int mm_vsnprintfW(wchar_t *buf, size_t bufsize, const wchar_t *format, va_list);
	// `bufsize' in count of wchar_t

DLLEXPORT_mmsnprintf
int mm_asprintfW  (wchar_t **ptr, const wchar_t *fmt, ...);

DLLEXPORT_mmsnprintf
int mm_vasprintfW (wchar_t **ptr, const wchar_t *fmt, va_list ap);

DLLEXPORT_mmsnprintf
int mm_asnprintfW (wchar_t **ptr, size_t max_alloc_size, const wchar_t *fmt, ...);

DLLEXPORT_mmsnprintf
int mm_vasnprintfW(wchar_t **ptr, size_t max_alloc_size, const wchar_t *fmt, va_list ap);
	// `max_alloc_size' also in count of wchar_t

#endif // #ifndef __CC_NORCROFT 

/*
int mm_asprintf  (char **ptr, const char *fmt,  ...);
int mm_vasprintf (char **ptr, const char *fmt, va_list ap);
int mm_asnprintf (char **ptr, size_t str_m, const char *fmt, ...);
int mm_vasnprintf(char **ptr, size_t str_m, const char *fmt, va_list ap);
*/

#ifdef _UNICODE
# define mm_snprintf mm_snprintfW
# define mm_vsnprintf mm_vsnprintfW
#else
# define mm_snprintf mm_snprintfA
# define mm_vsnprintf mm_vsnprintfA
#endif


#ifdef __cplusplus
}//extern"C"{
#endif
	
#endif

