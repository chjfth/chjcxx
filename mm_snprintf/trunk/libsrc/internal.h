#ifndef __mm_snprintf_internal_h_
#define __mm_snprintf_internal_h_

#include <ps_TCHAR.h>

typedef int Int;
typedef unsigned __int64 Uint64;

enum Fmt_et { fmt_unset, fmt_ptr, fmt_decimal_signed, fmt_decimal_unsigned, fmt_float };

#define is64bit (sizeof(void*)==8)

#define TMM_strmembytes(chars) ((chars)*sizeof(TCHAR))


int mmsnprintf_IsFloatingType(TCHAR fmt_spec);
//int mmsnprintf_IsFloatingTypeW(wchar_t fmt_spec);

void _mm_fillchars(TCHAR *pbuf, TCHAR c, size_t n);
void _mm_fillchars_opt(FUNC_mm_output proc, void *ctx, TCHAR c, size_t n);

const TCHAR *_mm_memchr(const TCHAR *buf, TCHAR c, size_t count);


#endif
