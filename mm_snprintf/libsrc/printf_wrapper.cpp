#ifndef MMSNPRINTF_NO_UNICODE
#include <wchar.h> // Linux glibc wprintf needs this.
#endif

#include "mm_psfunc.h"
#include <mm_snprintf.h>

#define DLL_AUTO_EXPORT_STUB
extern"C" void mmsnprintf_lib__printf_wrapper__DLL_AUTO_EXPORT_STUB(void){}

void mmct_os_printfA(void *ctx_user, const char *pcontent, int nchars, 
					const mmctexi_stA *pctexi)
{
	(void)ctx_user; (void)pctexi;
	printf("%.*s", nchars, pcontent);
}

int mm_printfA(const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);

	mmv7_stA mmi = {0};
	mmi.proc_output = mmct_os_printfA;
	mmi.ctx_output = NULL;
	int ret = mm_vsnprintf_v7A(mmi, fmt, args);

	va_end(args);
	return ret;
}

//////////////////////////////////////////////////////////////////////////
#ifndef MMSNPRINTF_NO_UNICODE
#ifndef OS_DONT_HAVE_wprintf

void mmct_os_printfW(void *ctx_user, const wchar_t *pcontent, int nchars, 
					const mmctexi_stW *pctexi)
{
	(void)ctx_user; (void)pctexi;

	wchar_t szfmt[] = L"%.*ls";
		// wprintf with %ls is both OK for VC6+ and glibc.
		// So don't bother with %s and %S which are inconsistent on VC and glibc

	wprintf(szfmt, nchars, pcontent);
}


int mm_printfW(const wchar_t *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	
	mmv7_stW mmi = {0};
	mmi.proc_output = mmct_os_printfW;
	mmi.ctx_output = NULL;
	int ret = mm_vsnprintf_v7W(mmi, fmt, args);
	
	va_end(args);
	return ret;
}

#endif
#endif
