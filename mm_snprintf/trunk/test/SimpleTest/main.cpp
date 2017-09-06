#ifdef __linux__
#define _tmain main
#endif

#include <assert.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <locale.h>
#include <time.h>

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

int g_cases = 0;

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
		printf("\n");
		assert(0);
	}

	g_cases++;
	return ret;
}

int mprintNA(const char *cmp, char buf[], int bufsize, const char *fmt, ...)
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
		printf("\n");
		assert(0);
	}

	g_cases++;
	return ret;
}


int mprintW(const wchar_t *cmp, const wchar_t *fmt, ...)
{

#if defined WIN32 || defined WINCE
#define FMT_WS L"%s"	// lower-case '%s'
#else // linux
#define FMT_WS L"%S"	// upper-case '%S'
#endif

	wchar_t buf[8000];
	int bufsize = sizeof(buf)/sizeof(buf[0]);
	va_list args;
	va_start(args, fmt);
	int ret = mm_vsnprintfW(buf, bufsize, fmt, args);
	wprintf(FMT_WS L"\n", buf);
	va_end(args);

	if(cmp && wcscmp(cmp, buf)!=0)
	{
		wprintf(L"\nExpect:\n" FMT_WS, cmp);
		wprintf(L"\nError:\n" FMT_WS, buf);
		wprintf(L"\n");
		assert(0);
	}

	g_cases++;
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
		wprintf(L"\n");
		assert(0);
	}

	g_cases++;
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


int print_with_prefix_suffix_oldstyle(TCHAR *buf, int bufsize, const TCHAR *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	int alen = mm_snprintf(buf, bufsize, 
		t("%c%w%s"), t('['), fmt, &args, t("]")); // %w consumes two arguments
	va_end(args);
	return alen;
}

int print_with_prefix_suffix(TCHAR *buf, int bufsize, const TCHAR *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	int alen = mm_snprintf(buf, bufsize, 
		t("%c%w%s"), t('['), MM_WPAIR_PARAM(fmt, args), t("]"));
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
	
	alen = print_with_prefix_suffix_oldstyle(buf, bufsize,
		t("Hello '%%%c' spec"), t('w')); // 15
	if(alen==17 && t_strcmp(buf, t("[Hello '%w' spec]"))==0)
		printf("test_w_specifier() old-style ok\n");
	else
	{
		printf("test_w_specifier() ERROR\n");
		assert(0);
	}
	return 0;
}

void test_fms_s()
{
	oks = t("[  abc]");
	mprint(oks, t("[%*s]"), 5, t("abc")); // width extended by width-spec
    
	oks = t("[123]");
	mprint(oks, t("[%*s]"), 2, t("123"));
	
	oks = t("[xyz]");
	mprint(oks, t("[%.*s]"), 5, t("xyz"));

	oks = t("[78]");
    mprint(oks, t("[%.*s]"), 2, t("789")); // truncate by precision-spec

	oks = t("[   ]");
	mprint(oks, t("[%3.0s]"), t("mmm"));
}

void test_v2()
{
	oks = t("");
	mprint(oks, t("%.0d"), 0);

	oks = t("12345");
	mprint(oks, t("%.3d"), 12345);

	oks = t("[123  ]"); // tell "left"-justify dynamically
	mprint(oks, t("[%*d]"), -5, 123);

	oks = t("[-123 ]"); // tell "left"-justify dynamically
	mprint(oks, t("[%*d]"), -5, -123);
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

	// Disable thousep with a "" %T
	oks = t("[1,234][56789]");
	mprint(oks, t("%T[%D]%T[%D]"), t(","), 1234, t(""), 56789);
	
	// Switch thousep in the middle with another %T
	oks = t("[1,234][56  789]");
	mprint(oks, t("%T[%D]%T[%D]"), t(","), 1234, t("  "), 56789);

	__int64 i64 = 4123;
	unsigned __int64 u64 = -2;
	oks = t("[4,123][18,446,744,073,709,551,614]");
	mprint(oks, t("%T[%llD][%llU]"), t(","), i64, u64);

	// test with length specifier

	oks = t("[  12 345]");
	mprint(oks, t("[%8D]"), 12345);

	oks = t("[ -12 345]"); // width is 8
	mprint(oks, t("[%*D]"), 8, -12345);

	oks = t("[-12,345 ]"); // width is 8
	mprint(oks, t("%T[%-*D]"), t(","), 8, -12345);
	
	oks = t("[-12,346 ]");
	mprint(oks, t("%t[%-*d]"), t(","), 8, -12346);

	oks = t("[-12,347 ]");
	mprint(oks, t("%t[%*d]"), t(","), -8, -12347);
	
	// ... we need 9 digits with 0s pre-padded, 9 does not count sepers ... 
	oks = t("[000,012,345]");
	mprint(oks, t("%t[%0.9u]"), t(","), 12345); 

	oks = t("[-000,012,345]");
	mprint(oks, t("%t[%0.9d]"), t(","), -12345); 

	oks = t("[ -000,012,345]");
	mprint(oks, t("%t[%013.9d]"), t(","), -12345); 
		// The joint effect of "0" and ".9" in "%013.9d" says that
		// we need to do zero-padding in our thousand number.
		// The digits(including padded zeros) should reach at least 9.
		// "13" says we need extra space-padding to a whole print width of 13.

	oks = t("[-000,012,345 ]"); // left justify
	mprint(oks, t("%t[%-013.9d]"), t(","), -12345); 
	mprint(oks, t("%t[%0*.9d]"), t(","), -13, -12345); 

	__int64 x9 = 9123456789;
	oks = t("[ 00,009,123,456,789]");
	mprint(oks, t("%t[%0*.*lld]"), t(","), 19, 14, x9);

	// compare to prev case, forgetting the "0" flag will cause unexpected result.
	oks = t("[     09,123,456,789]");
	mprint(oks, t("%t[%*.*lld]"), t(","), 19, 14, x9);
	//
	oks = t("[   0009,123,456,789]");
	mprint(oks, t("%t[%*.*lld]"), t(","), 19, 16, x9);

	// Check for %_ %t

	oks = t("[1 234][56 789]");
	mprint(oks, t("%t[%d][%d]"), t(" "), 1234, 56789);

	oks = t("[1,234][56789]");
	mprint(oks, t("%t[%d]%t[%d]"), t(","), 1234, t(""), 56789);

	if(IS64BIT)
	{
		oks = t("[fffff980`08ee0d80][00001FFF`BFAF13D8]"); // windbg 64-bit pointer
		mprint(oks, t("%_%t[%p][%P]"), 8, t("`"), 0xFFFFF98008EE0D80, 0x00001FFFBFAF13D8);

		oks = t("[0xfffff980`08ee0d80][0X00001FFF`BFAF13D8]"); // windbg 64-bit pointer
		mprint(oks, t("%_%t[%#p][%#P]"), 8, t("`"), 0xFFFFF98008EE0D80, 0x00001FFFBFAF13D8);
	}
	else
	{
		oks = t("[f8ee.0d80][BFAF.13D8]"); 
		mprint(oks, t("%_%t[%p][%P]"), 4, t("."), 0xF8EE0D80, 0xBFAF13D8);

		oks = t("[0xf8ee.0d80][0XBFAF.13D8]"); 
		mprint(oks, t("%_%t[%#p][%#P]"), 4, t("."), 0xF8EE0D80, 0xBFAF13D8);
	}


	oks = t("[0xBA1.2345.6789][55,912,345,678][12.7867.0939.1241]"); 
	mprint(oks, t("%_%t%T[0x%llX][%llU][%llu]"), 4, t("."), t(","), 
		0xBA123456789, 55912345678 ,0xBA123456789);
	
	oks = t("[   0xba1 2345 6789]"); 
	mprint(oks, t("%_%t[%#*llx]"), 4, t(" "), 18, 0xBA123456789); // [18-chars]
	
	// flag '0' as width, the padded '0's has nothing to do with thousand-body
	oks = t("[0x000ba1 2345 6789]");
	mprint(oks, t("%_%t[%#0*llx]"), 4, t(" "), 18, 0xBA123456789);

	// flag '0' as precision, the padded '0's will go into thousand-body, 
	// and outer part is padded by spaces.
	oks = t("[0x00 0000 0ba1 2345 6789]");
	mprint(oks, t("%_%t[%#0.*llx]"), 4, t(" "), 18, 0xBA123456789);

	oks = t("[0x00 0000 0ba1 2345 6789 ]");
	mprint(oks, t("%_%t[%#0*.*llx]"), 4, t(" "), -25, 18, 0xBA123456789);
	
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

void test_am()
{
	const int bufsize = 5;
	TCHAR buf[bufsize] = t("");
	TCHAR *pbuf = buf;
	int bufremain = bufsize;

	int needed = mm_snprintf_am(pbuf, bufremain, t("%s"), t("012"));
	assert(needed==3);
	assert(pbuf==buf+3 && bufremain==bufsize-3);

	needed += mm_snprintf_am(pbuf, bufremain, t("%d"), 345);
	assert(needed==6);
	assert(pbuf==buf+bufsize-1 && bufremain==1);
}

int mmF_ansitime2ymdhms(void *param, TCHAR *buf, int bufsize)
{
	time_t now = *(time_t*)param;
	struct tm* ptm = gmtime(&now);
	int len = mm_snprintf(buf, bufsize, t("%04d-%02d-%02d %02d:%02d:%02d"),
		ptm->tm_year+1900, ptm->tm_mon+1, ptm->tm_mday,
		ptm->tm_hour, ptm->tm_min, ptm->tm_sec
		);
	return len;
}

void test_v6()
{
	time_t now_epoch = 0x7FFFffff;
	oks = t("time_t will overflow at UTC [2038-01-19 03:14:07].");
	mprint(oks, t("time_t will overflow at UTC [%F]."), 
		MM_FPAIR_PARAM(mmF_ansitime2ymdhms, &now_epoch)
		);

	const int smallsize = 18;
	TCHAR smallbuf[smallsize];
	int retlen = mm_snprintf(smallbuf, smallsize, t("[%F]"),
		MM_FPAIR_PARAM(mmF_ansitime2ymdhms, &now_epoch));
	assert( t_strcmp(smallbuf, t("[2038-01-19 03:14"))==0 );
	assert( retlen==21 );
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

	test_fms_s();

	test_v2();

	test_v3();
	
	test_memdump();

	mprint(NULL, t("")); // print a empty line

	test_w_specifier();

	test_v5();	
	
	test_am();

	test_v6();

	printf("\n");
	printf("All %d test cases passed.\n", g_cases);

	return 0;
}

