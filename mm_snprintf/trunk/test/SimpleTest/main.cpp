#include <stdarg.h>
#include <stdio.h>
#include <mm_snprintf.h>

__int64 i64 = 0x912345678;
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
	wprintf(L"%s\n", buf);
	va_end(args);
}

int main()
{

	mprintA("__int64 [%lld, 0x%llx]", i64, i64);

	mprintW(L"[%d]", 12);
	mprintW(L"[%s]", L"xyz");
	mprintW(L"float [%f, %g, %e]", d, d, d);

	return 0;
}
