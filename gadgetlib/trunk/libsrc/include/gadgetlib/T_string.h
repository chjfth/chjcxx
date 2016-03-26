#ifndef __T_string_h_20160319_
#define __T_string_h_20160319_

#ifdef __cplusplus
extern"C" {
#endif


#ifndef DLLEXPORT_gadgetlib
# define DLLEXPORT_gadgetlib
#endif

DLLEXPORT_gadgetlib
int T_printf(const TCHAR *fmt, ...);
	// return the value of mm_vsnprintf's return

DLLEXPORT_gadgetlib
int T_printf_stderr(const TCHAR *fmt, ...);

DLLEXPORT_gadgetlib
int T_printf_query_bufsize(void);


DLLEXPORT_gadgetlib
int T_strlen(const TCHAR *str);

DLLEXPORT_gadgetlib
TCHAR * T_strcpy(TCHAR *dst, const TCHAR *src);

DLLEXPORT_gadgetlib
TCHAR * T_strncpy(TCHAR *dst, const TCHAR *src, int count);


DLLEXPORT_gadgetlib
int T_strcmp(const TCHAR *str1, const TCHAR *str2);

DLLEXPORT_gadgetlib // i: case-insensitive compare
int T_stricmp(const TCHAR *str1, const TCHAR *str2, size_t count);

DLLEXPORT_gadgetlib
int T_strncmp(const TCHAR *str1, const TCHAR *str2, size_t count);
	
DLLEXPORT_gadgetlib
int T_strnicmp(const TCHAR *str1, const TCHAR *str2, size_t count);

DLLEXPORT_gadgetlib
TCHAR * T_strstr(const TCHAR *str1, const TCHAR *str2);

DLLEXPORT_gadgetlib
TCHAR * T_strchr(const TCHAR *str, TCHAR chr);


//// Functions from stdlib.h

DLLEXPORT_gadgetlib
int T_atoi(const TCHAR *str);

DLLEXPORT_gadgetlib
__int64 T_atoi64(const TCHAR *str);

DLLEXPORT_gadgetlib
int T_strtoi(const TCHAR *str, TCHAR **endptr, int base);

DLLEXPORT_gadgetlib
__int64 T_strtoi64(const TCHAR *str, TCHAR **endptr, int base);


//// Filename operation (will be in dirfuncs.h)

//#define T_access _waccess
//#define T_remove _wremove
//#define T_rename _wrename


#ifdef __cplusplus
} // extern"C" {
#endif

#endif
