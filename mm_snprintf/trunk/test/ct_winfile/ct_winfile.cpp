#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <tchar.h>
#include <assert.h>
#include <stdio.h>
#include <locale.h>

#include <mm_snprintf.h>

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

