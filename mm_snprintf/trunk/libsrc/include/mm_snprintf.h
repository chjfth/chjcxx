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
int mm_snprintf_amA(char * &pbuf, int &bufsize, const char *fmt, ...); // am means auto-update of pbuf & bufsize

DLLEXPORT_mmsnprintf
int mm_strcatA(char *dest, size_t bufsize, const char *fmt, ...);

DLLEXPORT_mmsnprintf
int mm_printfA(const char *fmt, ...);

// v7 co functions. co: custom output(write to file, serial port etc).
typedef void (FUNC_mm_outputA)(void *user_ctx, const char *pcontent, int nchars);
// -- pcontent is not NUL terminated.
//
DLLEXPORT_mmsnprintf
int mm_snprintf_ctA(FUNC_mm_outputA proc_output, void *ctx_output, const char *fmt, ...);
//
struct mmv7_stA
{
	FUNC_mm_outputA *proc_output; // If NULL, callee should fill .buf_output[]
	void *ctx_output;
	
	char *buf_output;
	size_t bufsize;
	
	const char *pstock; // only for %F callee
};
//
DLLEXPORT_mmsnprintf
int mm_vsnprintf_v7A(const mmv7_stA &mmi, const char *fmt, va_list ap);
//
DLLEXPORT_mmsnprintf
int  mm_snprintf_v7A(const mmv7_stA &mmi, const char *fmt, ...);


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
int mm_snprintf_amW(wchar_t * &pbuf, int &bufsize, const wchar_t *fmt, ...); // am means auto-update of pbuf & bufsize

DLLEXPORT_mmsnprintf
int mm_strcatW(wchar_t *dest, size_t bufsize, const wchar_t *fmt, ...);

DLLEXPORT_mmsnprintf
int mm_printfW(const wchar_t *fmt, ...);

// v7 co functions. co: custom output(write to file, serial port etc).
typedef void (FUNC_mm_outputW)(void *user_ctx, const wchar_t *pcontent, int nchars);
// -- pcontent is not NUL terminated.
//
DLLEXPORT_mmsnprintf
int mm_snprintf_ctW(FUNC_mm_outputW proc_output, void *ctx_output, const wchar_t *fmt, ...);
//
struct mmv7_stW
{
	FUNC_mm_outputW *proc_output; // If NULL, callee should fill .buf_output[]
	void *ctx_output;
	
	wchar_t *buf_output;
	size_t bufsize;
	
	const wchar_t *pstock; // only for %F callee
};
//
DLLEXPORT_mmsnprintf
int mm_vsnprintf_v7W(const mmv7_stW &mmi, const wchar_t *fmt, va_list ap);
//
DLLEXPORT_mmsnprintf
int  mm_snprintf_v7W(const mmv7_stW &mmi, const wchar_t *fmt, ...);



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
	// overload '&' to workaround gcc 4.8+ 'taking address of temporary' error
	// Thanks to http://stackoverflow.com/a/11618639/151453 
	mm_wpair_stA* operator&(){ return this; } 
	#define MM_WPAIR_PARAMA(fmt, args) &mm_wpair_stA((fmt), &(args)) // only for C++
#endif
};
//
struct mm_wpair_stW
{
	unsigned int magic;
	const wchar_t *fmt;
	const va_list *pargs;
#ifdef __cplusplus
	mm_wpair_stW(const wchar_t *f, const va_list *a) : magic(mm_wpair_magic), fmt(f), pargs(a) {}
	mm_wpair_stW* operator&(){ return this; }
	#define MM_WPAIR_PARAMW(fmt, args) &mm_wpair_stW((fmt), &(args)) // only for C++
#endif
};


// The struct used for %F (function call injection), since v6.0
//
enum { mm_fpair_magic = 0xEF170321 };
//
typedef int (FUNC_mm_fpairA)(void *user_ctx, const mmv7_stA &mmi);
typedef int (FUNC_mm_fpairW)(void *user_ctx, const mmv7_stW &mmi);
// -- Return value tells output characters count, assuming buffer is enough (not counting ending NUL).
//
struct mm_fpair_stA
{
	unsigned int magic;
	FUNC_mm_fpairA *func;
	void *func_param;
#ifdef __cplusplus
	mm_fpair_stA(FUNC_mm_fpairA *f, void *p) : magic(mm_fpair_magic), func(f), func_param(p) {}
	mm_fpair_stA* operator&(){ return this; }
#define MM_FPAIR_PARAMA(f, p) &mm_fpair_stA( (f), ((void*)(p)) ) // only for C++
#endif
};
//
struct mm_fpair_stW
{
	unsigned int magic;
	FUNC_mm_fpairW *func;
	void *func_param;
#ifdef __cplusplus
	mm_fpair_stW(FUNC_mm_fpairW *f, void *p) : magic(mm_fpair_magic), func(f), func_param(p) {}
	mm_fpair_stW* operator&(){ return this; }
#define MM_FPAIR_PARAMW(f, p) &mm_fpair_stW( (f), ((void*)(p)) ) // only for C++
#endif
};

////

#if (defined _UNICODE) || (defined UNICODE)

# define mm_snprintf mm_snprintfW
# define mm_vsnprintf mm_vsnprintfW
# define mm_strcat mm_strcatW
# define mm_asprintf mm_asprintfW
# define mm_vasprintf mm_vasprintfW
# define mm_asnprintf mm_asnprintfW
# define mm_vasnprintf mm_vasnprintfW
# define mm_snprintf_am mm_snprintf_amW
# define mm_printf mm_printfW
//
# define mm_wpair_st mm_wpair_stW
# define MM_WPAIR_PARAM MM_WPAIR_PARAMW
# define mm_fpair_st mm_fpair_stW
# define MM_FPAIR_PARAM MM_FPAIR_PARAMW
# define FUNC_mm_fpair FUNC_mm_fpairW
# define FUNC_mm_output FUNC_mm_outputW
# define mmv7_st mmv7_stW
# define mm_snprintf_ct mm_snprintf_ctW
# define mm_vsnprintf_v7 mm_vsnprintf_v7W
# define mm_snprintf_v7 mm_snprintf_v7W

#else

# define mm_snprintf mm_snprintfA
# define mm_vsnprintf mm_vsnprintfA
# define mm_strcat mm_strcatA
# define mm_asprintf mm_asprintfA
# define mm_vasprintf mm_vasprintfA
# define mm_asnprintf mm_asnprintfA
# define mm_vasnprintf mm_vasnprintfA
# define mm_snprintf_am mm_snprintf_amA
# define mm_printf mm_printfA
//
# define mm_wpair_st mm_wpair_stA
# define MM_WPAIR_PARAM MM_WPAIR_PARAMA
# define mm_fpair_st mm_fpair_stA
# define MM_FPAIR_PARAM MM_FPAIR_PARAMA
# define FUNC_mm_fpair FUNC_mm_fpairA
# define FUNC_mm_output FUNC_mm_outputA
# define mmv7_st mmv7_stA
# define mm_snprintf_ct mm_snprintf_ctA
# define mm_vsnprintf_v7 mm_vsnprintf_v7A
# define mm_snprintf_v7 mm_snprintf_v7A

#endif

typedef unsigned __int64 Uint64_mmv; // implies %v need a Uint64 data.

#ifdef __cplusplus
}//extern"C"{
#endif
	
#endif

