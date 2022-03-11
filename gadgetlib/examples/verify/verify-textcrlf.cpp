#undef NDEBUG // for assert() to work.

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>

#include <commdefs.h>
#include <ps_TCHAR.h>
#include <ps_TypeDecl.h>
#include <mm_snprintf.h>
#include <gadgetlib/T_string.h>
#include <gadgetlib/textcrlf.h>
//#include <getopt/sgetopt.h>


#include "share.h"

const TCHAR null_0xff = (TCHAR)0xffff;

static int icase = 0; 

const int obufsize = 100;
static TCHAR obuf[obufsize];

static void 
verify_case(const TCHAR *szcrlf, const TCHAR *inbuf, int obuf_limit, const TCHAR *sret, int nret)
{
	assert(obuf_limit<=obufsize);

	memset(obuf, null_0xff, obufsize*sizeof(TCHAR));
	
	int nchars = ggt_normalize_crlf(inbuf, obuf, obuf_limit, szcrlf);
	if(nret!=nchars)
	{
		mm_printf(_T("Verify textcrlf case %d return-value error. Return: %d, Expect: %d\n"),  
			icase, nchars, nret);
		assert(nret==nchars);
	}

	if(sret==NULL)
	{
		assert(obuf[0]==(TCHAR)(char)null_0xff);
	}
	else
	{
		int olen = T_strlen(sret);
		int olen_ = olen+1;

		int i;
		for(i=0; i<olen_; i++)
		{
			if(obuf[i]!=sret[i])
			{
				mm_printf(_T("Verify textcrlf case %d output error at offset %d:\n"), icase, i);
				mm_printf(_T("Input:\n%s\n"), inbuf);
				mm_printf(_T("Output:\n%s\n"), obuf);
				mm_printf(_T("Correct:\n%s\n"), sret);
				assert(0);
			}
		}
	}

	icase++;
}

void verify_textcrlf()
{
	const TCHAR *cr = _T("\r");
	const TCHAR *lf = _T("\n");
	const TCHAR *crlf = _T("\r\n");

	mm_printf(_T("Verifying textcrlf...\n"));

	verify_case(crlf, _T(""), obufsize, _T(""), 0);
	verify_case(crlf, _T(""), 0, NULL, 0);
	verify_case(crlf, _T(""), -1, NULL, 0);
	verify_case(crlf, _T(""), -2, NULL, 0);

	verify_case(crlf, _T("abc"), obufsize, _T("abc"), 3);

	verify_case(crlf, _T("line1\r\nline2"), obufsize, _T("line1\r\nline2"), 12);
	verify_case(lf, _T("line1\r\nline2"), obufsize, _T("line1\nline2"), 11);

	verify_case(crlf, _T("line1\nline2"), obufsize, _T("line1\r\nline2"), 12);
	verify_case(crlf, _T("line1\nline2"), 10, _T("line1\r\nli"), 12); // short buf
	verify_case(crlf, _T("line1\nline2"), -2, NULL, 12); // short buf

	verify_case(crlf, _T("line1\nline2\n"), obufsize, _T("line1\r\nline2\r\n"), 14);

	verify_case(crlf, _T("ab\r\n\ncd"), obufsize, _T("ab\r\n\r\ncd"), 8);
	verify_case(crlf, _T("ab\n\r\ncd"), obufsize, _T("ab\r\n\r\ncd"), 8);
	verify_case(crlf, _T("ab\r\rcd"), obufsize, _T("ab\r\n\r\ncd"), 8);
	verify_case(crlf, _T("ab\n\n\rcd"), obufsize, _T("ab\r\n\r\n\r\ncd"), 10);

	verify_case(cr, _T("ab\ncd"), obufsize, _T("ab\rcd"), 5);
	verify_case(lf, _T("ab\ncd"), obufsize, _T("ab\ncd"), 5);
	verify_case(lf, _T("ab\rcd"), obufsize, _T("ab\ncd"), 5);

	verify_case(crlf, _T("ab\ncd\nef"), obufsize, _T("ab\r\ncd\r\nef"), 10);

	verify_case(crlf, _T("ab\ncd\r"), obufsize, _T("ab\r\ncd\r\n"), 8);
	verify_case(crlf, _T("ab\ncd\n"), obufsize, _T("ab\r\ncd\r\n"), 8);

	verify_case(crlf, _T("ab\ncd\n"), 8, _T("ab\r\ncd\r"), 8); // short buf

	mm_printf(_T("Verifying textcrlf %d cases success.\n"), icase);
}

