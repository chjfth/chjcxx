#ifndef __mm_snprintf_internal_h_
#define __mm_snprintf_internal_h_

#include <ps_TCHAR.h>

#define PORTABLE_SNPRINTF_VERSION_MAJOR 3
#define PORTABLE_SNPRINTF_VERSION_MINOR 2

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

#endif
