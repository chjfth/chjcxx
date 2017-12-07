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

#define MM_DBG_PROGRESS_LINE_MAXCHARS_ 200


enum mm_crlf_et
{
	mm_crlf_default = 0,
	mm_crlf_lf = 0,   // \n only, Linux
	mm_crlf_crlf = 1, // \r\n, Windows
	mm_crlf_cr = 2,   // \r only, MAC
};

DLLEXPORT_mmsnprintf
void mm_set_crlf_style(mm_crlf_et style);
	// This affects %m's newline style.

typedef int mmbufsize_t;

DLLEXPORT_mmsnprintf
int mm_snprintfA(char *buf, mmbufsize_t bufsize, const char *format, ...);

DLLEXPORT_mmsnprintf
int mm_vsnprintfA(char *buf, mmbufsize_t bufsize, const char *format, va_list);

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
int mm_asnprintfA(char **ptr, mmbufsize_t max_alloc_size, const char *fmt, ...);
	// Similar to mm_asprintf, but mm_asnprintf will malloc a buffer size no larger than 
	// max_alloc_size of characters(that can hold max_alloc_size-1 chars and a terminating NUL-char),
	// If max_alloc_size==0, *ptr will be 0 on return.
	// The return value, like mm_snprintf, tells the total chars of the resulting string,
	// -- assuming the buffer size is sufficient. 

DLLEXPORT_mmsnprintf
int mm_vasnprintfA(char **ptr, mmbufsize_t max_alloc_size, const char *fmt, va_list ap);

DLLEXPORT_mmsnprintf
int mm_snprintf_amA(char * &pbuf, mmbufsize_t &bufsize, const char *fmt, ...); // am means auto-update of pbuf & bufsize

DLLEXPORT_mmsnprintf
int mm_StrcatA(char *dest, mmbufsize_t bufsize, const char *fmt, ...);

DLLEXPORT_mmsnprintf
int mm_printfA(const char *fmt, ...);


// v7 co functions. co: custom output(write to file, serial port etc).
struct mmctexi_stA // ctexi: custom target extra info
{
	const char *pfmt; // original input szfmt
	int fmtpos; // current advancing position in szfmt
	int fmtnc;  // nc: number of chars, e.g. "%d" is 2 , "%llX" is 4.
	bool has_width;
	int width;
	bool has_precision;
	int precision;
	
	int valsize; // tell the meaningful data size(1,2,4,8,12) of the union:
	union // the corresponding value of current 'type'
	{
		char val_char;
		unsigned char val_uchar;
		short val_short;
		unsigned short val_ushort;
		int val_int;
		unsigned int val_uint;
		long val_long;
		unsigned long val_ulong;
		__int64 val_int64;
		unsigned __int64 val_uint64;
		const void * val_ptr;
		double val_double;
		
		wchar_t val_wchar;
		char val_TCHAR; // char!
		unsigned char placehldr[16]; 
	};

	int mmlevel;

	int outpos;
	int nchars_stock; // already output chars from mmlevel 0
};
//
typedef void (FUNC_mmct_outputA)(void *ctx_user, const char *pcontent, int nchars, 
								 const mmctexi_stA *pctexi);
// -- note: pcontent is not NUL terminated.
//
DLLEXPORT_mmsnprintf
int mm_printf_ctA(FUNC_mmct_outputA proc_output, void *ctx_output, const char *fmt, ...);
//
struct mmv7_stA
{
	FUNC_mmct_outputA *proc_output; // If NULL, callee should fill .buf_output[]
	void *ctx_output;
	
	char *buf_output;
	mmbufsize_t bufsize;

	int mmlevel;	
	int nchars_stock; // only meaningful to %F callee
	
	bool suppress_dbginfo; // internal use, user should set 0
};
//
DLLEXPORT_mmsnprintf
int mm_vsnprintf_v7A(mmv7_stA &mmi, const char *fmt, va_list ap);
//
DLLEXPORT_mmsnprintf
int  mm_snprintf_v7A(mmv7_stA &mmi, const char *fmt, ...);

typedef void FUNC_mm_DebugProgressA(void *ctx_user, const char *psz_dbginfo);

DLLEXPORT_mmsnprintf
void mm_set_DebugProgressCallbackA(FUNC_mm_DebugProgressA *dbgproc, void *ctx_user);


#ifndef MMSNPRINTF_NO_UNICODE
//
// Unicode version of the above mm_ functions
//


DLLEXPORT_mmsnprintf
int mm_snprintfW(wchar_t *buf, mmbufsize_t bufsize, const wchar_t *format, ...);

DLLEXPORT_mmsnprintf
int mm_vsnprintfW(wchar_t *buf, mmbufsize_t bufsize, const wchar_t *format, va_list);
	// `bufsize' in count of wchar_t

DLLEXPORT_mmsnprintf
int mm_asprintfW  (wchar_t **ptr, const wchar_t *fmt, ...);

DLLEXPORT_mmsnprintf
int mm_vasprintfW (wchar_t **ptr, const wchar_t *fmt, va_list ap);

DLLEXPORT_mmsnprintf
int mm_asnprintfW (wchar_t **ptr, mmbufsize_t max_alloc_size, const wchar_t *fmt, ...);

DLLEXPORT_mmsnprintf
int mm_vasnprintfW(wchar_t **ptr, mmbufsize_t max_alloc_size, const wchar_t *fmt, va_list ap);
	// `max_alloc_size' also in count of wchar_t

DLLEXPORT_mmsnprintf
int mm_snprintf_amW(wchar_t * &pbuf, int &bufsize, const wchar_t *fmt, ...); // am means auto-update of pbuf & bufsize

DLLEXPORT_mmsnprintf
int mm_StrcatW(wchar_t *dest, mmbufsize_t bufsize, const wchar_t *fmt, ...);

DLLEXPORT_mmsnprintf
int mm_printfW(const wchar_t *fmt, ...);

// v7 co functions. co: custom output(write to file, serial port etc).
struct mmctexi_stW // ctexi: custom target extra info
{
	const wchar_t *pfmt; // original input szfmt
	int fmtpos; // current advancing position in szfmt
	int fmtnc;  // nc: number of chars, e.g. "%d" is 2 , "%llX" is 4.
	bool has_width;
	int width;
	bool has_precision;
	int precision;

	int valsize; // tell the meaningful data size(1,2,4,8,12) of the union:
	union // the corresponding value of current 'type'
	{
		char val_char;
		unsigned char val_uchar;
		short val_short;
		unsigned short val_ushort;
		int val_int;
		unsigned int val_uint;
		long val_long;
		unsigned long val_ulong;
		__int64 val_int64;
		unsigned __int64 val_uint64;
		const void * val_ptr;
		double val_double;
		
		wchar_t val_wchar;
		wchar_t val_TCHAR; // wchar_t
		unsigned char placehldr[16];
	};

	int mmlevel;

	int outpos;
	int nchars_stock;
};
//
typedef void (FUNC_mmct_outputW)(void *ctx_user, const wchar_t *pcontent, int nchars, 
								 const mmctexi_stW *pctexi);
// -- pcontent is not NUL terminated.
//
DLLEXPORT_mmsnprintf
int mm_printf_ctW(FUNC_mmct_outputW proc_output, void *ctx_output, const wchar_t *fmt, ...);
//
struct mmv7_stW
{
	FUNC_mmct_outputW *proc_output; // If NULL, callee should fill .buf_output[]
	void *ctx_output;
	
	wchar_t *buf_output;
	mmbufsize_t bufsize;
	
	int mmlevel; // debugging purpose
	int nchars_stock; // only meaningful to %F callee

	bool suppress_dbginfo; // internal use, user should set 0
};
//
DLLEXPORT_mmsnprintf
int mm_vsnprintf_v7W(mmv7_stW &mmi, const wchar_t *fmt, va_list ap);
//
DLLEXPORT_mmsnprintf
int  mm_snprintf_v7W(mmv7_stW &mmi, const wchar_t *fmt, ...);

typedef void FUNC_mm_DebugProgressW(void *ctx_user, const wchar_t *psz_dbginfo);

DLLEXPORT_mmsnprintf
void mm_set_DebugProgressCallbackW(FUNC_mm_DebugProgressW *dbgproc, void *ctx_user);

#endif // #ifndef MMSNPRINTF_NO_UNICODE


DLLEXPORT_mmsnprintf
int mm_free_buf(void *ptr);
	// Free the buffer pointer returned by mm_asprintf, mm_asnprintf, mm_vasprintf, mm_vasnprintf
	// Return 0 on success.

#define mm_strcpy(dst, bufsize, src) mm_snprintf(dst, bufsize, _T("%s"), src)
#define mm_strcat(dst, bufsize, src) mm_Strcat(dst, bufsize, _T("%s"), src)



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
#ifndef MMSNPRINTF_NO_UNICODE
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
#endif // #ifndef MMSNPRINTF_NO_UNICODE


// The struct used for %F (function call injection), since v6.0
//
enum { mm_fpair_magic = 0xEF170321 };
//
typedef int (FUNC_mmF_FormatterA)(void *ctx_formatter, mmv7_stA &mmi);
// -- Return value tells output characters count, assuming buffer is enough (not counting ending NUL).
struct mmF_pair_stA
{
	unsigned int magic;
	FUNC_mmF_FormatterA *func;
	void *func_param;
#ifdef __cplusplus
	mmF_pair_stA(FUNC_mmF_FormatterA *f, void *p) : magic(mm_fpair_magic), func(f), func_param(p) {}
	mmF_pair_stA* operator&(){ return this; }
#define MM_FPAIR_PARAMA(f, p) &mmF_pair_stA( (f), ((void*)(p)) ) // only for C++
#endif
};
//
#ifndef MMSNPRINTF_NO_UNICODE
typedef int (FUNC_mmF_FormatterW)(void *ctx_formatter, mmv7_stW &mmi);
// -- Return value tells output characters count, assuming buffer is enough (not counting ending NUL).
struct mmF_pair_stW
{
	unsigned int magic;
	FUNC_mmF_FormatterW *func;
	void *func_param;
#ifdef __cplusplus
	mmF_pair_stW(FUNC_mmF_FormatterW *f, void *p) : magic(mm_fpair_magic), func(f), func_param(p) {}
	mmF_pair_stW* operator&(){ return this; }
#define MM_FPAIR_PARAMW(f, p) &mmF_pair_stW( (f), ((void*)(p)) ) // only for C++
#endif
};
#endif // #ifndef MMSNPRINTF_NO_UNICODE

////

#if (defined _UNICODE) || (defined UNICODE)

# define mm_snprintf mm_snprintfW
# define mm_vsnprintf mm_vsnprintfW
# define mm_Strcat mm_StrcatW
# define mm_asprintf mm_asprintfW
# define mm_vasprintf mm_vasprintfW
# define mm_asnprintf mm_asnprintfW
# define mm_vasnprintf mm_vasnprintfW
# define mm_snprintf_am mm_snprintf_amW
# define mm_printf mm_printfW
//
# define mm_wpair_st mm_wpair_stW
# define MM_WPAIR_PARAM MM_WPAIR_PARAMW
# define mmF_pair_st mmF_pair_stW
# define MM_FPAIR_PARAM MM_FPAIR_PARAMW
# define FUNC_mmF_Formatter FUNC_mmF_FormatterW
# define FUNC_mmct_output FUNC_mmct_outputW
# define mmv7_st mmv7_stW
# define mm_printf_ct mm_printf_ctW
# define mm_vsnprintf_v7 mm_vsnprintf_v7W
# define mm_snprintf_v7 mm_snprintf_v7W
# define mmctexi_st mmctexi_stW

#define FUNC_mm_DebugProgress FUNC_mm_DebugProgressW
#define mm_set_DebugProgressCallback mm_set_DebugProgressCallbackW

#else

# define mm_snprintf mm_snprintfA
# define mm_vsnprintf mm_vsnprintfA
# define mm_Strcat mm_StrcatA
# define mm_asprintf mm_asprintfA
# define mm_vasprintf mm_vasprintfA
# define mm_asnprintf mm_asnprintfA
# define mm_vasnprintf mm_vasnprintfA
# define mm_snprintf_am mm_snprintf_amA
# define mm_printf mm_printfA
//
# define mm_wpair_st mm_wpair_stA
# define MM_WPAIR_PARAM MM_WPAIR_PARAMA
# define mmF_pair_st mmF_pair_stA
# define MM_FPAIR_PARAM MM_FPAIR_PARAMA
# define FUNC_mmF_Formatter FUNC_mmF_FormatterA
# define FUNC_mmct_output FUNC_mmct_outputA
# define mmv7_st mmv7_stA
# define mm_printf_ct mm_printf_ctA
# define mm_vsnprintf_v7 mm_vsnprintf_v7A
# define mm_snprintf_v7 mm_snprintf_v7A
# define mmctexi_st mmctexi_stA

#define FUNC_mm_DebugProgress FUNC_mm_DebugProgressA
#define mm_set_DebugProgressCallback mm_set_DebugProgressCallbackA

#endif

typedef unsigned __int64 Uint64_mmv; // implies %v need a Uint64 data.

#ifdef __cplusplus
}//extern"C"{
#endif
	
#endif

