#ifndef __mm_snprintf_internal_h_
#define __mm_snprintf_internal_h_

#include <ps_TCHAR.h>

typedef int Int;

enum Fmt_et { fmt_unset, fmt_ptr, fmt_decimal_signed, fmt_decimal_unsigned, fmt_float };

#define is64bit (sizeof(void*)==8)

#define TMM_strmembytes(chars) ((chars)*sizeof(TCHAR))


int mmsnprintf_IsFloatingType(TCHAR fmt_spec);
//int mmsnprintf_IsFloatingTypeW(wchar_t fmt_spec);

void _mm_fillchars(TCHAR *pbuf, TCHAR c, size_t n);
void _mm_fillchars_opt(FUNC_mm_output proc, void *ctx, TCHAR c, size_t n);

const TCHAR *_mm_memchr(const TCHAR *buf, TCHAR c, size_t count);


int cal_adcol_digits(const void *imagine_addr, int bufbytes);

int _mm_dump_bytes(TCHAR *buf, int bufchars, 
			  const void *pbytes_, int dump_bytes, bool uppercase,
			  const TCHAR *bdd_hyphens, const TCHAR *bdd_left, const TCHAR *bdd_right,
			  int columns, int colskip, bool ruler,
			  int indents, 
			  const void *imagine_addr,
			  FUNC_mm_output *proc_output, void *ctx_output);

#endif
