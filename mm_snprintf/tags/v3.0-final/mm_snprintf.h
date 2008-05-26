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

int mm_snprintf(char *buf, size_t bufsize, const char *format, /*args*/ ...);
int mm_vsnprintf(char *buf, size_t bufsize, const char *format, va_list);

/*
int mm_asprintf  (char **ptr, const char *fmt,  ...);
int mm_vasprintf (char **ptr, const char *fmt, va_list ap);
int mm_asnprintf (char **ptr, size_t str_m, const char *fmt, ...);
int mm_vasnprintf(char **ptr, size_t str_m, const char *fmt, va_list ap);
*/

#ifdef __cplusplus
}//extern"C"{
#endif
	
#endif

