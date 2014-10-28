#ifndef __mm_snprintf_internal_h_
#define __mm_snprintf_internal_h_

#include <ps_TCHAR.h>

enum {
	mmsnprintf_vmajor = 4,
	mmsnprintf_vminor = 2,
	mmsnprintf_vbuild = 0
};

typedef int Int;

#define TMM_strmembytes(chars) ((chars)*sizeof(TCHAR))


#ifdef _UNICODE
# define mmsnprintf_IsFloatingType mmsnprintf_IsFloatingTypeW
# define mmsnprintf_fillchar mmsnprintf_fillcharW
#else
# define mmsnprintf_IsFloatingType mmsnprintf_IsFloatingTypeA
# define mmsnprintf_fillchar mmsnprintf_fillcharA
#endif

#define is64bit (sizeof(void*)==8)

int mmsnprintf_IsFloatingType(TCHAR fmt_spec);
//int mmsnprintf_IsFloatingTypeW(wchar_t fmt_spec);

void mmsnprintf_fillchar(TCHAR *pbuf, TCHAR c, int n);

int cal_adcol_digits(const void *imagine_addr, int bufbytes);

int mm_dump_bytes(TCHAR *buf, int bufchars, 
			  const void *pbytes_, int dump_bytes, bool uppercase,
			  const TCHAR *bdd_hyphens, const TCHAR *bdd_left, const TCHAR *bdd_right,
			  int columns, int colskip, bool ruler,
			  int indents, 
			  const void *imagine_addr);

#endif
