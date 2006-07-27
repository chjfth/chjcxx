#ifndef _MM_SNPRINTF_H_
#define _MM_SNPRINTF_H_

#ifdef __cplusplus
extern"C"{
#endif

#define PORTABLE_SNPRINTF_VERSION_MAJOR 2
#define PORTABLE_SNPRINTF_VERSION_MINOR 2

#include <stdio.h>
#include <stdarg.h>

int mm_snprintf(char *, size_t, const char *, /*args*/ ...);
int mm_vsnprintf(char *, size_t, const char *, va_list);

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

