#define WIN32_LEAN_AND_MEAN
#include <windows.h>
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

void mmct_CustomOutput(void *user_ctx, const TCHAR *pcontent, int nchars)
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

struct mmct_WriteFile_st
{
	DWORD winerr;
	HANDLE hFile;

	UINT nCalls; 
	UINT nChars;
};

void mmct_WriteFile(void *ctx_user, const TCHAR *pcontent, int nchars)
{
	// Caution: pcontent is not NUL-terminated.

	assert(nchars>0);

	mmct_WriteFile_st &ctx = *(mmct_WriteFile_st*)ctx_user;

	ctx.nCalls++;
	ctx.nChars += nchars;

	if(!ctx.winerr)
	{
		DWORD nwr = 0;
		BOOL b = WriteFile(ctx.hFile, pcontent, nchars*sizeof(TCHAR), &nwr, NULL);
		if(b)
		{
			mm_printf(_T("[Call #%d, nchars=%d]\n%.*s\n"), 
				ctx.nCalls, nchars, nchars, pcontent);
		}
		else
		{
			ctx.winerr = GetLastError();
			mm_printf(_T("WriteFile() error: Winerr=%u."), ctx.winerr);
		}
	}
}


int _tmain()
{
	// note: glibc bans mixing printf and wprintf, so avoid using mprintA here. (?)

	setlocale(LC_ALL, "");

	mm_set_crlf_style(mm_crlf_crlf);

#ifdef UNICODE
	TCHAR *filename = _T("123_mmct.utf16.txt");
#else
	TCHAR *filename = _T("123_mmct.ansi.txt");
#endif

	unsigned short mmver = mmsnprintf_getversion();
	int ver1 = mmver>>8, ver2 = mmver&0xff;

	mmct_WriteFile_st ctx = {0};

	ctx.hFile = CreateFile(filename,
        GENERIC_READ|GENERIC_WRITE,
        FILE_SHARE_READ,
        NULL, // no security attribute
        CREATE_ALWAYS, // create a new file
        0, // FILE_FLAG_OVERLAPPED,
        NULL);

#ifdef UNICODE
	// Write BOM mark so notepad can recognize it.
	DWORD nwr = 0;
	WriteFile(ctx.hFile, "\xFF\xFE", 2, &nwr, NULL);
#endif

	mm_printf_ct(mmct_WriteFile, &ctx, 
		_T("mm_snprintf version %d.%d .\r\n"), ver1, ver2);

	mm_printf_ct(mmct_WriteFile, &ctx, 
		_T("Hex-dumping the output filename %s:\r\n")
		_T("%k%R%m")
		,
		filename,
		_T(" "), 8, filename
		);

	CloseHandle(ctx.hFile);

	return 0;
}

