#ifndef _MM_SNPRINTF_H_
#define _MM_SNPRINTF_H_

#ifdef __cplusplus
extern"C"{
#endif


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

DLLEXPORT_mmsnprintf
int mm_strcatA(char *dest, size_t bufsize, const char *fmt, ...);


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

DLLEXPORT_mmsnprintf
int mm_strcatW(wchar_t *dest, size_t bufsize, const wchar_t *fmt, ...);

#endif // #ifndef __CC_NORCROFT 

DLLEXPORT_mmsnprintf
int mm_free_buf(void *ptr);
	// Free the buffer pointer returned by mm_asprintf, mm_asnprintf, mm_vasprintf, mm_vasnprintf
	// Return 0 on success.

// The struct used for %w, since v4.4
//
enum { mm_wpair_magic = 0xEF160913 };
//
struct mm_wpair_stA
{
	unsigned int magic;
	const char *fmt;
	const va_list *pargs;
#ifdef __cplusplus
	mm_wpair_stA(const char *f, const va_list *a) : magic(mm_wpair_magic), fmt(f), pargs(a) {}
	#define MM_WPAIR_PARAMA(fmt, args) &mm_wpair_stA((fmt), &(args)) // only for C++
#endif
};
struct mm_wpair_stW
{
	unsigned int magic;
	const wchar_t *fmt;
	const va_list *pargs;
#ifdef __cplusplus
	mm_wpair_stW(const wchar_t *f, const va_list *a) : magic(mm_wpair_magic), fmt(f), pargs(a) {}
	#define MM_WPAIR_PARAMW(fmt, args) &mm_wpair_stW((fmt), &(args)) // only for C++
#endif
};


#if (defined _UNICODE) || (defined UNICODE)
# define mm_snprintf mm_snprintfW
# define mm_vsnprintf mm_vsnprintfW
# define mm_strcat mm_strcatW
# define mm_asprintf mm_asprintfW
# define mm_vasprintf mm_vasprintfW
# define mm_asnprintf mm_asnprintfW
# define mm_vasnprintf mm_vasnprintfW
# define mm_wpair_st mm_wpair_stW
# define MM_WPAIR_PARAM MM_WPAIR_PARAMW
#else
# define mm_snprintf mm_snprintfA
# define mm_vsnprintf mm_vsnprintfA
# define mm_strcat mm_strcatA
# define mm_asprintf mm_asprintfA
# define mm_vasprintf mm_vasprintfA
# define mm_asnprintf mm_asnprintfA
# define mm_vasnprintf mm_vasnprintfA
# define mm_wpair_st mm_wpair_stA
# define MM_WPAIR_PARAM MM_WPAIR_PARAMA
#endif


#ifdef __cplusplus
}//extern"C"{
#endif
	
#endif

