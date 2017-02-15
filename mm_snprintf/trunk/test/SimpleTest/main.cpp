#ifdef __linux__
#define _tmain main
#endif

#include <assert.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <locale.h>

#include <wchar.h> // for wprintf under linux
#include <ps_TCHAR.h> // for _tmain macro
#include <commdefs.h>

#include <mm_snprintf.h>


#ifdef UNICODE

#define t(str) L##str 
const wchar_t *oks = 0; 
#define mprint mprintW
#define mprintN mprintNW
#define t_strcmp wcscmp

#else

const char *oks = 0; 
#define t(str) str
#define mprint mprintA
#define mprintN mprintNA
#define t_strcmp strcmp

#endif

__int64 i64 = (__int64)(0xF12345678); // decimal 64729929336
double d = 1.2345678;

#define IS64BIT ( sizeof(void*)==8 ? true : false )


int mprintA(const char *cmp, const char *fmt, ...)
{
	char buf[8000];
	int bufsize = sizeof(buf);
	va_list args;
	va_start(args, fmt);
	int ret = mm_vsnprintfA(buf, bufsize, fmt, args);
	printf("%s\n", buf);
	va_end(args);

	if(cmp && strcmp(cmp, buf)!=0)
	{
		printf("\nExpect:\n%s", cmp);
		printf("\nError:\n%s", buf);
		assert(0);
	}

	return ret;
}

int mprintNA(const char *cmp, char buf[], int bufsize, char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	int ret = mm_vsnprintfA(buf, bufsize, fmt, args);
	printf("%s\n", buf);
	va_end(args);
	
	if(cmp && strcmp(cmp, buf)!=0)
	{
		printf("\nExpect:\n%s", cmp);
		printf("\nError:\n%s", buf);
		assert(0);
	}
	return ret;
}


int mprintW(const wchar_t *cmp, const wchar_t *fmt, ...)
{
	wchar_t buf[8000];
	int bufsize = sizeof(buf)/sizeof(buf[0]);
	va_list args;
	va_start(args, fmt);
	int ret = mm_vsnprintfW(buf, bufsize, fmt, args);
#if defined WIN32 || defined WINCE
	wprintf(L"%s\n", buf); // lower-case '%s'
#else // linux
	wprintf(L"%S\n", buf); // upper-case '%S'
#endif
	va_end(args);

	if(cmp && wcscmp(cmp, buf)!=0)
	{
		wprintf(L"\nExpect:\n%S", cmp);
		wprintf(L"\nError:\n%S", buf);
		assert(0);
	}
	return ret;
}

int mprintNW(const wchar_t *cmp, wchar_t buf[], int bufsize, const wchar_t *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	int ret = mm_vsnprintfW(buf, bufsize, fmt, args);
#if defined WIN32 || defined WINCE
	wprintf(L"%s\n", buf); // lower-case '%s'
#else // linux
	wprintf(L"%S\n", buf); // upper-case '%S'
#endif
	va_end(args);
	
	if(cmp && wcscmp(cmp, buf)!=0)
	{
		wprintf(L"\nExpect:\n%s", cmp);
		wprintf(L"\nError:\n%s", buf);
		assert(0);
	}
	return ret;
}


int test_memdump()
{
	// [2014-07-12] bytes dump
	const TCHAR *str=t("ABN");
	int i;
	unsigned char mem[1024];
	for(i=0; i<sizeof(mem); i++) mem[i]=i;
//	mprintA("Hexdump1:\n%k%8r%*b!\n", "_", 4, 17, bytes);

//	mprint(t("%9m"), mem);

//	mprint(t("%*m"), 9, mem);

//	mprint(t("%m"), str);
//	mprint(t("[%0m]"), str); // should print only "[]" (mm4.1)
//	mprint(t("[%*m]"), 0, str); // should print only "[]" (mm4.1)
	

//	mprint(t("%k%5m,%M"), t(" "), mem, str);

//	mprint(t("%k%5m"), t("--"), mem);

//	mprint(t("%K%5m"), t("<>"), mem);

//	mprint(t("%k%K%5m"), t("-"), t("<>"), mem);

//	mprint(t("%k%r%17m"), t(" "), 8, mem);

//	mprint(t("%k%R%17m"), t(" "), 8, mem);

//	mprint(t("%k%4r%17m"), t(" "), 8, mem);
//	mprint(t("%k%4R%17m"), t(" "), 8, mem);

//	mprint(t("%k%0.3r%17m"), t(" "), 8, mem);
//	mprint(t("%k%*.*R%17m"), t(" "), 4, 3, 8, mem);

//	mprint(t("%0*lld%s\n"), 6, (__int64)345, "!"); // 00345! 

//	mprint(t("%k%R%v%17m"), t(" "), 8, (void*)0x1400, mem);

	oks = t("\
    ------00-01-02-03-04-05-06-07\n\
    000C:          00 01 02 03 04\n\
    0014: 05 06 07 08 09 0a 0b 0c\n\
    001C: 0d 0e 0f 10");
	mprint(oks, t("%k%*.*R%v%17m"), t(" "), 4, 3, 8, (void*)0xF, mem);

	oks = t("\
    --------00-01-02-03-04-05-06-07\n\
    427BFD:          00 01 02 03 04\n\
    427C05: 05 06 07 08 09 0a 0b 0c\n\
    427C0D: 0d 0e 0f 10");
	mprint(oks, t("%k%*.*R%v%17m"), t(" "), 4, 3, 8, (void*)0x427c00, mem);

	mprint(NULL, t(""));

	oks = t("\
             00 01 02 03 04\n\
    05 06 07 08 09 0a 0b 0c\n\
    0d 0e 0f 10");
	mprint(oks, t("%k%*.*r%v%17m"), t(" "), 4, 3, 8, (void*)0x427c00, mem);

//	mprintW(L"%k%*.*R%v%17m", " ", 4, 3, 8, 0x7777123, mem);


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


int print_with_prefix_suffix(TCHAR *buf, int bufsize, const TCHAR *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	int alen = mm_snprintf(buf, bufsize, 
		t("%c%w%s"), t('['), fmt, &args, t("]")); // %w consumes two arguments
	va_end(args);
	return alen;
}

int test_w_specifier()
{
	TCHAR buf[100]; 
	int bufsize = sizeof(buf)/sizeof(buf[0]);
	int alen = print_with_prefix_suffix(buf, bufsize,
		t("Hello '%%%c' spec"), t('w')); // 15
	if(alen==17 && t_strcmp(buf, t("[Hello '%w' spec]"))==0)
		printf("test_w_specifier() ok\n");
	else
	{
		printf("test_w_specifier() ERROR\n");
		assert(0);
	}
	return 0;
}

void test_v3()
{
	oks = t("[+12][ 34][-0056]");
	mprint(oks, t("[%+d][% d][%05d]"), 12, 34, -56);

	oks = t("[  +12][   34][  -56]");
	mprint(oks, t("[%+5d][% 5d][% 5d]"), 12, 34, -56);

	oks = t("hex[+ab][AB]");
	mprint(oks, t("hex[+%x][%X]"), 0xAB, 0xAB);
	
	oks =  IS64BIT ? t("hex[00000000eeeeeeee][000000000FFFEEEE]") : t("hex[eeeeeeee][0FFFEEEE]");
	mprint(oks, t("hex[%+p][%P]"), (void*)0xEEEEeeee, (void*)0x0FFFeeee);

	oks = t("__int64 [64729929336, 0xf12345678]");
	mprint(oks, t("__int64 [%lld, 0x%llx]"), i64, i64);

	oks = t("[12]");
	mprint(oks, t("[%d]"), 12);

	oks = t("[xyz]");
	mprint(oks, t("[%s]"), t("xyz"));

	mprint(NULL, t("float [%f, %g, %e]"), d, d, d);

}

void test_v5()
{
	oks = IS64BIT ? t("[0000000000000000]") : t("[00000000]");
	mprint(oks, t("[%p]"), NULL);

	oks = t("[0]");
	mprint(oks, t("[%D]"), 0);

	oks = t("[1 234][567]");
	mprint(oks, t("[%D][%D]"), 1234, 567);

	oks = t("[1,234][56,789]");
	mprint(oks, t("%T[%D][%D]"), t(","), 1234, 56789);

	// Check for %_ %t
	if(IS64BIT)
	{
		oks = t("[fffff980`08ee0d80][00001FFF`BFAF13D8]"); // windbg 64-bit pointer
		mprint(oks, t("%_%t[%p][%P]"), 8, t("`"), 0xFFFFF98008EE0D80, 0x00001FFFBFAF13D8);
	}
	else
	{
		oks = t("[f8ee.0d80][BFAF.13D8]"); 
		mprint(oks, t("%_%t[%p][%P]"), 4, t("."), 0xF8EE0D80, 0xBFAF13D8);
	}


	oks = t("[0xBA1.2345.6789][55,912,345,678][12.7867.0939.1241]"); 
	mprint(oks, t("%_%t%T[0x%llX][%llU][%llu]"), 4, t("."), t(","), 
		0xBA123456789, 55912345678 ,0xBA123456789);
	
	oks = t("[   0xba1 2345 6789]"); 
	mprint(oks, t("%_%t[%#*llx]"), 4, t(" "), 18, 0xBA123456789); // [18-chars]
	
	// sorry, the padded '0's for %0*X is not thousand-separated
	oks = t("[0x000ba1 2345 6789]");
	mprint(oks, t("%_%t[%#0*llx]"), 4, t(" "), 18, 0xBA123456789);

	oks = t("[123456]");
	mprint(oks, t("%T[%U]"), t(""), 123456); // deliberately using an empty %T string

	oks = t("[123:456]");
	mprint(oks, t("%t[%o]"), t(":"), 0123456); // octal

	// trigger internal buffer overrun
	int ret = 0;
	TCHAR smbuf1[10];
	oks = t("[123(*)45");
	ret = mprintN(oks, smbuf1, GetEleQuan_i(smbuf1), t("%T[%U]"), t("(*)"), 123456);
	assert(ret==11);
}

int _tmain()
{
	// note: glibc bans mixing printf and wprintf, so avoid using mprintA here. (?)

	setlocale(LC_ALL, "");

#ifdef UNICODE
	wprintf(L"Running Unicode build!\n");
#else
	printf("Running MBCS build!\n");
#endif

//	mprint(L"[12]", L"[%d]", 12);

	test_v3();
	
	test_memdump();

	mprint(NULL, t("")); // print a empty line

	test_w_specifier();

	test_v5();	
	
	return 0;
}

