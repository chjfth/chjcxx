#ifndef __mm_snprintf_internal_h_
#define __mm_snprintf_internal_h_

#include <ps_TCHAR.h>

enum {
	mmsnprintf_vmajor = 3,
	mmsnprintf_vminor = 2,
	mmsnprintf_vbuild = 0,
};

#define TMM_strmemsize(chars) ((chars)*sizeof(TCHAR))


#ifdef _UNICODE
# define mmsnprintf_IsFloatingType mmsnprintf_IsFloatingTypeW
# define mmsnprintf_fillchar mmsnprintf_fillcharW
#else
# define mmsnprintf_IsFloatingType mmsnprintf_IsFloatingTypeA
# define mmsnprintf_fillchar mmsnprintf_fillcharA
#endif


int mmsnprintf_IsFloatingType(TCHAR fmt_spec);
//int mmsnprintf_IsFloatingTypeW(wchar_t fmt_spec);

void mmsnprintf_fillchar(TCHAR *pbuf, TCHAR c, int n);

int mm_dump_bytes(TCHAR *buf, int bufchars, 
			  const void *pbytes_, int dump_bytes, bool uppercase,
			  const TCHAR *bdd_hyphens, const TCHAR *bdd_left, const TCHAR *bdd_right,
			  int columns, int indents, bool ruler);

#endif
