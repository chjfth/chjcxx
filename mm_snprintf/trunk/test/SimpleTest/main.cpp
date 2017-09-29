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
#include <_MINMAX_.h>

#include <mm_snprintf.h>

#ifdef UNICODE

#define t(str) L##str 
const wchar_t *oks = 0; 
//#define mprint mprintW
//#define mprintN mprintNW
#define t_strcmp wcscmp
#define t_strlen wcslen

#else

const char *oks = 0; 
#define t(str) str
//#define mprint mprintA
//#define mprintN mprintNA
#define t_strcmp strcmp
#define t_strlen strlen

#endif

__int64 i64 = (__int64)(0xF12345678); // decimal 64729929336
double d = 1.2345678;

#define IS64BIT ( sizeof(void*)==8 ? true : false )
#define ISWCHAR2B ( sizeof(wchar_t)==2 ? true : false )

#ifdef _UNICODE
const int isUnicodeBuild = 1;
#else
const int isUnicodeBuild = 0;
#endif

#define BUFMAX 8000


int g_cases = 0;

struct Custout_st
{
	int pos;
	TCHAR custbuf[BUFMAX];
};

void mmct_CustomOutput(void *user_ctx, const TCHAR *pcontent, int nchars, 
					   const mmctexi_st *pcti)
{
	assert(nchars>0);

	Custout_st &co = *(Custout_st*)user_ctx;
	memcpy(co.custbuf+co.pos, pcontent, sizeof(TCHAR)*nchars);

	co.pos += nchars;
}

int mprint(const TCHAR *cmp, const TCHAR *fmt, ...)
{
	g_cases++;
	mm_printf(_T("\n// Case %d:\n"), g_cases);

	TCHAR buf[BUFMAX];
	int bufsize = sizeof(buf);
	va_list args;
	va_start(args, fmt);
	int ret = mm_vsnprintf(buf, bufsize, fmt, args);
	mm_printf(_T("%s\n"), buf);
	va_end(args);

	if(cmp && t_strcmp(cmp, buf)!=0)
	{
		mm_printf(_T("\nExpect:\n%s"), cmp);
		mm_printf(_T("\nError:\n%s"), buf);
		mm_printf(_T("\n"));
		assert(0);
	}

	// Verify custom output result.

	va_start(args, fmt);

	Custout_st co = {0};
	ret = mm_printf_ct(mmct_CustomOutput, &co, _T("%w"), MM_WPAIR_PARAM(fmt, args));
	assert(ret==co.pos);
	co.custbuf[co.pos] = _T('\0');

	if(cmp && t_strcmp(cmp, co.custbuf)!=0)
	{
		mm_printf(_T("\nExpect:\n%s"), cmp);
		mm_printf(_T("\nCustom output Error:\n%s"), co.custbuf);
		mm_printf(_T("\n"));
		assert(0);
	}

	va_end(args);

	return ret;
}

int mprintN(const TCHAR *cmp, TCHAR buf[], int bufsize, const TCHAR *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	int ret = mm_vsnprintf(buf, bufsize, fmt, args);
	mm_printf(_T("%s\n"), buf);
	va_end(args);
	
	if(cmp && t_strcmp(cmp, buf)!=0)
	{
		mm_printf(_T("\nExpect:\n%s"), cmp);
		mm_printf(_T("\nError:\n%s"), buf);
		mm_printf(_T("\n"));
		assert(0);
	}

	g_cases++;
	return ret;
}

TCHAR * T_strupr_copy(const TCHAR ins[], TCHAR outs[], int bufchars)
{
	memset(outs, 0, bufchars*sizeof(TCHAR));
	int i;
	for(i=0; i<bufchars-1 && ins[i]; i++)
	{
		if(ins[i]>='a' && ins[i]<='z')
			outs[i] = ins[i] + ('A'-'a');
		else
			outs[i] = ins[i];
	}
	return outs;
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

int test_v4_memdump()
{
	// [2014-07-12] bytes dump
	const TCHAR *str=t("ABN");
	int i;
	unsigned char mem[1024];
	for(i=0; i<sizeof(mem); i++) 
		mem[i]=i;
//	mprintA("Hexdump1:\n%k%8r%*b!\n", "_", 4, 17, bytes);

	oks = t("000102030405060708");
	mprint(oks, t("%9m"), mem);
	mprint(oks, t("%*m"), 9, mem);

	mprint(t("[]"), t("[%0m]"), mem);
	mprint(t("{}"), t("{%*m}"), 0, mem);

	////

	const TCHAR * hexoutA_ABN = t("41424e");
	const TCHAR * hexoutW2_ABN = t("410042004e00");
	const TCHAR * hexoutW4_ABN = t("41000000420000004e000000");
	
	const TCHAR *hexout_ABN = hexoutA_ABN;
	if(isUnicodeBuild)
	{
		hexout_ABN = ISWCHAR2B ? hexoutW2_ABN : hexoutW4_ABN;
	}

	
	TCHAR hexout_ABN_up[40];
	T_strupr_copy(hexout_ABN, hexout_ABN_up, GetEleQuan(hexout_ABN_up));

	mprint(hexout_ABN, t("%m"), t("ABN"));
	mprint(hexout_ABN_up, t("%M"), t("ABN"));

	oks = t("00 01 02 03 04,41 42 4E");
	if(isUnicodeBuild)
		oks = ISWCHAR2B ? t("00 01 02 03 04,41 00 42 00 4E 00") : 
					t("00 01 02 03 04,41 00 00 00 42 00 00 00 4E 00 00 00");
	mprint(oks, t("%k%5m,%M"), t(" "), mem, t("ABN")); // %k separator is space

	oks = t("0001020304,41424E");
	if(isUnicodeBuild)
		oks = ISWCHAR2B ? t("0001020304,410042004E00") : t("0001020304,41000000420000004E000000");
	mprint(oks, t("%k%5m,%M"), t(""), mem, t("ABN")); // %k separator is NUL sstring

	oks = t("00--01--02--03--04");
	mprint(oks, t("%k%5m"), t("--"), mem); // %k separator has multi-chars

	oks = t("<00><01><02><03><04>");
	mprint(oks, t("%K%5m"), t("<>"), mem); // %K (upper K) assigns pincher

	oks = t("<00>-<01>-<02>-<03>-<04>");
	mprint(oks, t("%k%K%5m"), t("-"), t("<>"), mem); // %k %K both


	const int row_width = 8; // data for %r and %R
	// %r : multi row mode
	oks = t("\
00 01 02 03 04 05 06 07\n\
08 09 0a 0b 0c 0d 0e 0f\n\
10");
	mprint(oks, t("%k%r%17m"), t(" "), row_width, mem); // 8 relates to %r

	// %R : multi row mode with ruler

	oks = t("\
---00-01-02-03-04-05-06-07\n\
0: 00 01 02 03 04 05 06 07\n\
8: 08 09 0a");
	mprint(oks, t("%k%R%11m"), t(" "), row_width, mem);

	oks = t("\
---00-01-02-03-04-05-06-07\n\
0: 00 01 02 03 04 05 06 07\n\
8: 08 09 0a 0b 0c 0d 0e 0f");
	mprint(oks, t("%k%R%16m"), t(" "), row_width, mem);
	
	oks = t("\
----00-01-02-03-04-05-06-07\n\
00: 00 01 02 03 04 05 06 07\n\
08: 08 09 0a 0b 0c 0d 0e 0f\n\
10: 10");
	mprint(oks, t("%k%R%17m"), t(" "), row_width, mem);

	oks = t("\
    00 01 02 03 04 05 06 07\n\
    08 09 0a 0b 0c 0d 0e 0f\n\
    10");
	mprint(oks, t("%k%4r%17m"), t(" "), row_width, mem); // %r ruler with 4 char indent

	oks = t("\
    ----00-01-02-03-04-05-06-07\n\
    00: 00 01 02 03 04 05 06 07\n\
    08: 08 09 0a 0b 0c 0d 0e 0f\n\
    10: 10");
	mprint(oks, t("%k%*R%*m"), t(" "), 4, row_width, 17, mem); // %R ruler with 4 char indent

	oks = t("\
         00 01 02 03 04\n\
05 06 07 08 09 0a 0b 0c\n\
0d 0e 0f 10");
	mprint(oks, t("%k%0.3r%17m"), t(" "), row_width, mem); // %0.3r : column skip = 3

    oks = t("\
      ----00-01-02-03-04-05-06-07\n\
      FFFFFFFFFFFFFFFD:          00 01 02 03 04\n\
      05: 05 06 07 08 09 0a 0b 0c\n\
      0D: 0d 0e 0f 10"); 
	mprint(oks, t("%k%*.*R%17m"), t(" "), 6, 3, row_width, mem); // line indent 6 + colum skip 3
		// This case is a bit weird, adcol_width is 2 bcz addr_end is 0x10(width=2).
		// Just leave this special case alone bcz this only happens when %v addr is not rounded to %R count.
		// Sanity people are not likely to do that.

	oks = t("\
    ----00-01-02-03-04-05-06-07\n\
    0C:          00 01 02 03 04\n\
    14: 05 06 07 08 09 0a 0b 0c\n\
    1C: 0d 0e 0f 10");
	mprint(oks, t("%k%*.*R%v%17m"), t(" "), 4, 3, row_width, (Uint64_mmv)0xF, mem);

	oks = t("\
    --------00-01-02-03-04-05-06-07\n\
    427BFD:          00 01 02 03 04\n\
    427C05: 05 06 07 08 09 0a 0b 0c\n\
    427C0D: 0d 0e 0f 10");
	mprint(oks, t("%k%*.*R%v%17m"), t(" "), 4, 3, row_width, (Uint64_mmv)0x427c00, mem);

	oks = t("\
             00 01 02 03 04\n\
    05 06 07 08 09 0a 0b 0c\n\
    0d 0e 0f 10");
	mprint(oks, t("%k%*.*r%v%17m"), t(" "), 4, 3, row_width, (Uint64_mmv)0x427c00, mem);

//	mprintW(L"%k%*.*R%v%17m", " ", 4, 3, 8, 0x7777123, mem);

	// verify %*.*v

	oks = t("\
  ------00-01-02-03-04-05-06-07\n\
  000C:          00 01 02 03 04\n\
  0014: 05 06 07 08 09 0a 0b 0c\n\
  001C: 0d 0e 0f 10");
	mprint(oks, t("%k%*.*R%t%.*v%17m"), 
	   t(" "), // %k
	   2, 3, row_width, // %*.*R
	   t("."),  // %t
	   4, (Uint64_mmv)0xF, // %.*v
	   mem // %17m
	   );

	oks = t("\
  -----------00-01-02-03-04-05-06-07\n\
  0000.000C:          00 01 02 03 04\n\
  0000.0014: 05 06 07 08 09 0a 0b 0c\n\
  0000.001C: 0d 0e 0f 10");
	mprint(oks, t("%k%*.*R%t%*.*v%17m"), 
		t(" "), // %k
		2, 3, row_width, // %*.*R
		t("."),  // %t
		4, 8, (Uint64_mmv)0xF, // %*.*v
		mem // %17m
		);

	const int row_width_16 = 16;
	oks = t("\
  -------------------00-01-02-03-04-05-06-07-08-09-0A-0B-0C-0D-0E-0F\n\
  00000000`0000000C:          00 01 02 03 04 05 06 07 08 09 0a 0b 0c\n\
  00000000`0000001C: 0d 0e 0f 10");
	mprint(oks, t("%k%*.*R%t%*.*v%17m"), 
		t(" "), // %k
		2, 3, row_width_16, // %*.*R
		t("`"),  // %t
		8, 16, (Uint64_mmv)0xF, // %*.*v
		mem // %17m
		);

	oks = t("\
    --------00-01-02-03-04-05-06-07-08-09-0A-0B-0C-0D-0E-0F\n\
    0.FFF0:          03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f\n\
    1.0000: 10 11 12 13");
	mprint(oks, t("%k%*.*R%t%4v%17m"), 
		t(" "), // %k
		4, 3, row_width_16, // %*.*R, indent=4, col-skip=3
		t("."),  // %t
		(Uint64_mmv)0xFFF3, // %4v
		mem+3);

	oks = t("\
    -----------00-01-02-03-04-05-06-07-08-09-0A-0B-0C-0D-0E-0F\n\
    0000.FFF0:          03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f\n\
    0001.0000: 10 11 12 13");
	mprint(oks, t("%k%*.*R%t%4.0v%17m"), 
		t(" "), // %k
		4, 3, row_width_16, // %*.*R, indent=4, col-skip=3
		t("."),  // %t
		(Uint64_mmv)0xFFF3, // %4.0v //!// Is_IsoZeros()
		mem+3);

	oks = t("\
    -------00-01-02-03-04-05-06-07-08-09-0A-0B-0C-0D-0E-0F\n\
    0,0,0:          03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f\n\
    0,1,0: 10 11 12 13 14 15 16 17 18 19 1a 1b 1c 1d 1e 1f\n\
    0,2,0: 20 21 22 23 24 25 26 27 28 29 2a 2b 2c 2d 2e 2f\n\
    0,3,0: 30 31 32 33 34 35 36 37 38 39 3a 3b 3c 3d 3e 3f\n\
    0,4,0: 40 41 42 43 44 45 46 47 48 49 4a 4b 4c 4d 4e 4f\n\
    0,5,0: 50 51 52 53 54 55 56 57 58 59 5a 5b 5c 5d 5e 5f\n\
    0,6,0: 60 61 62 63 64 65 66 67 68 69 6a 6b 6c 6d 6e 6f\n\
    0,7,0: 70 71 72 73 74 75 76 77 78 79 7a 7b 7c 7d 7e 7f\n\
    0,8,0: 80 81 82 83 84 85 86 87 88 89 8a 8b 8c 8d 8e 8f\n\
    0,9,0: 90 91 92 93 94 95 96 97 98 99 9a 9b 9c 9d 9e 9f\n\
    0,A,0: a0 a1 a2 a3 a4 a5 a6 a7 a8 a9 aa ab ac ad ae af\n\
    0,B,0: b0 b1 b2 b3 b4 b5 b6 b7 b8 b9 ba bb bc bd be bf\n\
    0,C,0: c0 c1 c2 c3 c4 c5 c6 c7 c8 c9 ca cb cc cd ce cf\n")
	t("\
    0,D,0: d0 d1 d2 d3 d4 d5 d6 d7 d8 d9 da db dc dd de df\n\
    0,E,0: e0 e1 e2 e3 e4 e5 e6 e7 e8 e9 ea eb ec ed ee ef\n\
    0,F,0: f0 f1 f2 f3 f4 f5 f6 f7 f8 f9 fa fb fc fd fe ff\n\
    1,0,0: 00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f\n\
    1,1,0: 10 11 12 13 14 15 16 17 18 19 1a 1b 1c 1d 1e 1f\n\
    1,2,0: 20 21 22 23 24 25 26 27 28 29 2a 2b 2c 2d 2e");
	mprint(oks, t("%t%k%*.*R%*.0v%*m"),
		t(","), t(" "),
		4, 3, row_width_16, // %*.*R, indent=4, col-skip=3
		1, (Uint64_mmv)3, // %*.0v
		300, mem+3 // %*m
		);


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

	oks = t("");
	mprint(oks, t("%k%*m"), t(" "), 0, mem); // dump 0 byte

	oks = t("");
	mprint(oks, t("%k%R%*m"), t(" "), 
		row_width,
		0, mem); // dump 0 byte

	mm_set_crlf_style(mm_crlf_crlf);
	oks = t("\
---00-01-02-03-04-05-06-07\r\n\
0: 00 01 02 03 04 05 06 07\r\n\
8: 08 09 0a");
	mprint(oks, t("%k%R%11m"), t(" "), row_width, mem);
	mm_set_crlf_style(mm_crlf_default);

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
	// 
	oks = t("[00012,345]");
	mprint(oks, t("%t[%.9u]"), t(","), 12345); // "[00012,345]" same as ANSI printf

	
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

int mmF_ansitime2ymdhms(void *param, const mmv7_st &mmi)
{
	assert( mmi.bufsize==0 || (mmi.buf_output && mmi.buf_output[0]==_T('\0')) );

	// mmi.pstock will point to "time_t will overflow at UTC ["

	time_t now = *(time_t*)param;
	struct tm* ptm = gmtime(&now);
	int len = mm_snprintf_v7(mmi, t("%04d-%02d-%02d %02d:%02d:%02d"),
		ptm->tm_year+1900, ptm->tm_mon+1, ptm->tm_mday,
		ptm->tm_hour, ptm->tm_min, ptm->tm_sec
		);
	return len;
}

int mmF_ansitime2ymdhms_method2(void *param, const mmv7_st &mmi)
{
	assert( mmi.bufsize==0 || (mmi.buf_output && mmi.buf_output[0]==_T('\0')) );
	
	// pstock[] will be "time_t will overflow at UTC ["
	
	time_t now = *(time_t*)param;
	struct tm* ptm = gmtime(&now);

	// Two code flows according to whether mmi.proc_output is provided.

	int len = 0;
	if(mmi.proc_output)
	{	
		// should call custom-output with real content
		TCHAR timebuf[80];
		len = mm_snprintf(timebuf, GetEleQuan(timebuf), t("%04d-%02d-%02d %02d:%02d:%02d"),
			ptm->tm_year+1900, ptm->tm_mon+1, ptm->tm_mday,
			ptm->tm_hour, ptm->tm_min, ptm->tm_sec
			);
		int reallen = t_strlen(timebuf);
		mmi.proc_output(mmi.ctx_output, timebuf, _MIN_(len, reallen), NULL);
	}
	else
	{
		len = mm_snprintf(mmi.buf_output, mmi.bufsize, t("%04d-%02d-%02d %02d:%02d:%02d"),
			ptm->tm_year+1900, ptm->tm_mon+1, ptm->tm_mday,
			ptm->tm_hour, ptm->tm_min, ptm->tm_sec
			);
	}

	return len;
}


struct mmF_from_nullbuf_st
{
	int chk_offset;
	int call_count;
};

int mmF_callback_from_nullbuf(void *param, const mmv7_st &mmi)
{
	mmF_from_nullbuf_st &ctx = *(mmF_from_nullbuf_st*)param;

	ctx.call_count++;

	assert(mmi.pstock==NULL);
	assert(mmi.buf_output-(TCHAR*)0==ctx.chk_offset);
	return 0;
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

	oks = t("time_t WILL overflow at UTC [2038-01-19 03:14:07].");
	mprint(oks, t("time_t WILL overflow at UTC [%F]."), 
		MM_FPAIR_PARAM(mmF_ansitime2ymdhms_method2, &now_epoch)
		);

	mmF_from_nullbuf_st ctx = {0, 0};
	retlen = mm_snprintf(NULL, 0, _T("%F"), 
		MM_FPAIR_PARAM(mmF_callback_from_nullbuf, &ctx));
	assert( retlen==0 );
	assert( ctx.call_count==1 );

#define STRING_BEFORE_F _T("ABC")
	ctx.chk_offset = t_strlen(STRING_BEFORE_F);
	retlen = mm_snprintf(NULL, 0, 
		STRING_BEFORE_F _T("%F"), 
		MM_FPAIR_PARAM(mmF_callback_from_nullbuf, &ctx));
	assert( retlen==ctx.chk_offset );
	assert( ctx.call_count==2 );
}

/////////////////////////////// v7: /////////////////////////////////////

struct mmct_Result_st
{
	int mmLevel;
	const TCHAR *pfmt;
	int fmtpos;
	int fmtnc;
	bool has_width;
	int width;
	bool has_precision;
	int precision;
	int valsize; // 1,2,4,8,12

	int output_totchars;
};

struct mmct_Verify_st
{
	int idx;
	int output_accums;

	const mmct_Result_st *rs;
	int rs_count;
	int cur_rs;
	int cur_partials;
	const TCHAR *fmt_partial;

	void Reset(const mmct_Result_st *_rs, int count)
	{ 
		memset(this, 0, sizeof(*this));
		rs = _rs; rs_count = count;
	} 
};

int mmF_desc_widpreci(void *ctx_user, const mmv7_st &mmi)
{
	mmctexi_st &cti = *(mmctexi_st*)ctx_user;

	TCHAR *pbuf = mmi.buf_output;
	int bufremain = mmi.bufsize;

	int len = 0;
	len += mm_snprintf_am(pbuf, bufremain, t("width:%s"), 
		cti.has_width?t("true"):t("false"));
	//
	if(cti.has_width)
		len += mm_snprintf_am(pbuf, bufremain, t(",%d"), cti.width);

	len += mm_snprintf_am(pbuf, bufremain, t(" preci:%s"), 
		cti.has_precision?t("true"):t("false"));
	//
	if(cti.has_precision)
		len += mm_snprintf_am(pbuf, bufremain, t(",%d"), cti.precision);

	return len;
}


void mmct_Verify(void *user_ctx, const TCHAR *pcontent, int nchars, 
				 const mmctexi_st *pctexi)
{
	assert(pctexi);
	const mmctexi_st &cti = *pctexi;
	
	mmct_Verify_st &iverify = *(mmct_Verify_st*)user_ctx;

	if(iverify.idx==0)
	{
		mm_printf(t("v7ct: %s\n"), cti.pfmt);
	}

	mm_printf(t("  <%.*s> fmtpos:%d+%d %F output:%d+%d\n"), 
		cti.fmtnc, cti.pfmt+cti.fmtpos,  cti.fmtpos, cti.fmtnc, 
		MM_FPAIR_PARAM(mmF_desc_widpreci, &cti), // %F
		iverify.output_accums, nchars // output:%d+%d
		);

	// verify mm_snprintf's data with manual ones.

	const mmct_Result_st &rs = iverify.rs[iverify.cur_rs];
	assert( cti.mmlevel==rs.mmLevel );
	assert( t_strcmp(cti.pfmt, rs.pfmt)==0 );
	assert( cti.fmtpos==rs.fmtpos );
	assert( cti.fmtnc==rs.fmtnc );
	assert( cti.has_width==rs.has_width && cti.width==rs.width );
	assert( cti.has_precision==rs.has_precision && cti.precision==rs.precision );
	assert( cti.valsize==rs.valsize );
//	assert( memcmp(cti.placehldr, ) );
	
	if(iverify.fmt_partial)
		assert( t_strcmp(cti.pfmt, iverify.fmt_partial)==0 );

	iverify.cur_partials += nchars;
	assert(iverify.cur_partials<=rs.output_totchars);
	if(iverify.cur_partials==rs.output_totchars)
	{
		// one fmtspec completes
		assert( rs.output_totchars );

		iverify.cur_partials = 0;
		iverify.fmt_partial = NULL;
		iverify.cur_rs++; // we'll proceed to nex fmtspec
	}
	else
	{
		iverify.fmt_partial = cti.pfmt;
	}


	iverify.idx++;
	iverify.output_accums += nchars;
}

enum { vs0=0, vs_char=1, vs_wchar=2, 
	vs_TCHAR = sizeof(TCHAR),
	vs_int=sizeof(int), vs_long=sizeof(long), vs_int64=sizeof(__int64), 
	vs_double=sizeof(double), vsptr=sizeof(void*) };

void test_v7_ct()
{
	mm_printf(t("\n"));

	const TCHAR *szfmt = NULL;

	mmct_Verify_st iverify;

	szfmt = t("%cABC%d%06.4d!");
	mmct_Result_st rs[] =
	{
		{1, szfmt, 0,2, false,0, false,0, vs_TCHAR, 1}, // %c
		{1, szfmt, 2,3, false,0, false,0, vs0,  3}, // ABC
		{1, szfmt, 5,2, false,0, false,0, vs_int, 3}, // %d
		{1, szfmt, 7,6, true,6, true,4, vs_int, 6}, // %06.4d
		{1, szfmt, 13,1, false,0, false,0, vs0, 6}, // !
	};
	iverify.Reset(rs, GetEleQuan(rs));
	mm_printf_ct(mmct_Verify, &iverify, szfmt, t('@'), 123, 456);
}

int _tmain()
{
	// note: glibc bans mixing printf and wprintf, so avoid using mprintA here. (?)

	setlocale(LC_ALL, "");

	unsigned short mmver = mmsnprintf_getversion();
	int ver1 = mmver>>8, ver2 = mmver&0xff;

	mm_printf(_T("Running %s build! (ver %u.%u) sizeof(TCHAR)=%d\n)"), 
		isUnicodeBuild ? _T("Unicode") : _T("ANSI"),
		ver1, ver2,
		sizeof(TCHAR));

	test_fms_s();

	test_v2();

	test_v3();
	
	test_v4_memdump();

	mprint(NULL, t("")); // print a empty line

	test_w_specifier();

	test_v5();	
	
	test_am();

	test_v6();

	test_v7_ct();

	mm_printf(_T("\nAll %d test cases passed.\n"), g_cases);

	return 0;
}

