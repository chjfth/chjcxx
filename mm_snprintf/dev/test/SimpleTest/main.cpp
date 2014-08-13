#include <stdarg.h>
#include <stdio.h>
#include <locale.h>

#include <wchar.h> // for wprintf under linux
#include <ps_TCHAR.h> // for _tmain macro
#include <ps_TypeDecl.h>
#include <mm_snprintf.h>

//#if

__int64 i64 = DECL_INT64(0x912345678); // decimal 38960125560
double d = 1.2345678;

void mprintA(const char *fmt, ...)
{
	char buf[2000];
	int bufsize = sizeof(buf);
	va_list args;
	va_start(args, fmt);
	int ret = mm_vsnprintfA(buf, bufsize, fmt, args);
	printf("%s\n", buf);
	va_end(args);
}

void mprintW(const wchar_t *fmt, ...)
{
	wchar_t buf[2000];
	int bufsize = sizeof(buf)/sizeof(buf[0]);
	va_list args;
	va_start(args, fmt);
	int ret = mm_vsnprintfW(buf, bufsize, fmt, args);
#if defined WIN32 || defined WINCE
	wprintf(L"%s\n", buf);
#else // linux
	wprintf(L"%S\n", buf);
#endif
	va_end(args);
}

int _tmain()
{
//	setlocale(LC_ALL, "");

#if defined WIN32 || defined WINCE
	mprintA("__int64 [%lld, 0x%llx]", i64, i64);
#else // linux
	mprintW(L"__int64 [%lld, 0x%llx]", i64, i64);
	// glibc ban mixing printf and wprintf, so avoid using mprintA here.
#endif

	mprintW(L"[%d]", 12);
	mprintW(L"[%s]", L"xyz");
	mprintW(L"float [%f, %g, %e]", d, d, d);

	// [2014-07-12] bytes dump
	const char *str="ABN";
	int i;
	unsigned char mem[256];
	for(i=0; i<sizeof(mem); i++) mem[i]=i;
//	mprintA("Hexdump1:\n%k%8r%*b!\n", "_", 4, 17, bytes);

//	mprintA("%9m", mem);

//	mprintA("%*m", 9, mem);

//	mprintA("%m", str);

//	mprintA("%k%5m,%M", " ", mem, str);

//	mprintA("%k%5m", "--", mem);

//	mprintA("%K%5m", "<>", mem);

//	mprintA("%k%K%5m", "-", "<>", mem);

//	mprintA("%k%r%17m", " ", 8, mem);

//	mprintA("%k%R%17m", " ", 8, mem);

//	mprintA("%k%4r%17m", " ", 8, mem);
//	mprintA("%k%4R%17m", " ", 8, mem);

//	mprintA("%k%0.3r%17m", " ", 8, mem);
//	mprintA("%k%*.*R%17m", " ", 4, 3, 8, mem);

//	mprintA("%0*lld\n", 6, (__int64)345); // 00345 

//	mprintA("%k%*.*R%v%17m!", " ", 4, 3, 8, 0x7777123, mem);
	mprintW(L"%k%*.*R%v%17m!", " ", 4, 3, 8, 0x7777123, mem);

/*	mprintW(L"%k%K"
		L"%*.*R"
		L"%M!\n"
		,
		L"_", L"<>",
		0, 1, 8, 
		L"0123456789a");
*/
/*	mprintA("%k%K"
		"%*.*R"
		"%B!\n"
		,
		"_._", "<<>>",
		4, 1, 8, 
		"0123456789abcdefghijklmnopqr");
*/
/*	
	mprintA("%k%K"
		"%*.*R"
		"%*b!\n"
		,
		"_", "<<>>",
		7, 1, 4, 
		18, bytes);
*/
	return 0;
}
