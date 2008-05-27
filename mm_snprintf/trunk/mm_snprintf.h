#ifndef _MM_SNPRINTF_H_
#define _MM_SNPRINTF_H_

#ifdef __cplusplus
extern"C"{
#endif

//#define PORTABLE_SNPRINTF_VERSION_MAJOR 2 
//#define PORTABLE_SNPRINTF_VERSION_MINOR 2 
	// These are defined in the source, and queried by mmsnprintf_getversion().

#include <stdio.h>
#include <stdarg.h>

unsigned short mmsnprintf_getversion(void);
	// Returns: Major version in higher byte and minor version in lower byte.

int mm_snprintfA(char *buf, size_t bufsize, const char *format, /*args*/ ...);
int mm_vsnprintfA(char *buf, size_t bufsize, const char *format, va_list);

int mm_snprintfW(wchar_t *buf, size_t bufsize, const wchar_t *format, /*args*/ ...);
int mm_vsnprintfW(wchar_t *buf, size_t bufsize, const wchar_t *format, va_list);
	// `bufsize' in TCHARs

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

