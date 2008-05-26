#include <stdarg.h>
#include <stdio.h>
#include <mm_snprintf.h>

__int64 i64 = 0x912345678;

void mprintA(const char *fmt, ...)
{
	char buf[200];
	va_list args;
	va_start(args, fmt);
	mm_vsnprintfA(buf, sizeof(buf), fmt, args);
	printf("%s\n", buf);
	va_end(args);
}

int main()
{

	mprintA("__int64 [%lld, 0x%llx]", i64, i64);

	return 0;
}
