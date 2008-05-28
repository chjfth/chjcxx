#include <stdarg.h>
#include <stdio.h>
#include <locale.h>

#include <wchar.h> // for wprintf under linux
#include <ps_TypeDecl.h>
#include <mm_snprintf.h>

//#if

__int64 i64 = DECL_INT64(0x912345678);
double d = 1.2345678;

void mprintA(const char *fmt, ...)
{
	char buf[200];
	va_list args;
	va_start(args, fmt);
	mm_vsnprintfA(buf, sizeof(buf), fmt, args);
	printf("%s\n", buf);
	va_end(args);
}

void mprintW(const wchar_t *fmt, ...)
{
	wchar_t buf[200];
	va_list args;
	va_start(args, fmt);
	mm_vsnprintfW(buf, sizeof(buf), fmt, args);
#ifdef _WIN32
	wprintf(L"%s\n", buf);
#else // linux
	wprintf(L"%S\n", buf);
#endif
	va_end(args);
}

int main()
{
//	setlocale(LC_ALL, "");

#ifdef _WIN32
	mprintA("__int64 [%lld, 0x%llx]", i64, i64);
#else // linux
	mprintW(L"__int64 [%lld, 0x%llx]", i64, i64);
	// glibc ban mixing printf and wprintf, so avoid using mprintA here.
#endif

	mprintW(L"[%d]", 12);
	mprintW(L"[%s]", L"xyz");
	mprintW(L"float [%f, %g, %e]", d, d, d);

	return 0;
}
