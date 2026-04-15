// This is included by parent directory makeTsdring.h .
// Do not compile this file alone.

#include <assert.h>
#include <stdio.h>
#include <iconv.h> // Linux glibc
#include <wchar.h> // wcslen()

#include <commdefs.h>
#include <linux/msvc_extras.h>

const char* mTs_windows_codepage_to_iconv(int codepage, char charset[], int bufsize) 
{
	// Map Windows codepages to iconv names
	if(codepage==0 || codepage==mTs_UTF8)
	{
		snprintf(charset, bufsize, "UTF-8");
	}
	else
	{
		snprintf(charset, bufsize, "CP%d", codepage);
	}

	return charset;
}

char* mTs_char_from_wchar(int codepage, const wchar_t* instr, int *p_outchars_)
{
	SETTLE_OUTPUT_PTR(int, p_outchars_, 0)

	if(!instr)
		return NULL;

	char charset[16] = {};
	mTs_windows_codepage_to_iconv(codepage, charset, ARRAYSIZE(charset));

	iconv_t conv = iconv_open(charset, "WCHAR_T");
	if(conv == (iconv_t)-1)
		return NULL;

	size_t inwchars_ = wcslen(instr) + 1;
	size_t inbytes_ = inwchars_ * sizeof(wchar_t);
	size_t outbytes = inwchars_ * 4; // UTF-8 can expand up to 4 bytes per char
	char* outbuf = new char[outbytes];
	char* outptr = outbuf;
	char* inptr = (char*)instr;

	if(iconv(conv, &inptr, &inbytes_, &outptr, &outbytes) == (size_t)-1) {
		iconv_close(conv);
		delete[] outbuf;
		return NULL;
	}

	iconv_close(conv);
	
	assert(outptr[-1]=='\0');

	*p_outchars_ = outptr - outbuf - 1;
	return outbuf;	
}

wchar_t* mTs_wchar_from_char(int codepage, const char* instr, int *p_outwchars_)
{
	SETTLE_OUTPUT_PTR(int, p_outwchars_, 0)

	if(!instr)
		return NULL;

	char charset[16] = {};
	mTs_windows_codepage_to_iconv(codepage, charset, ARRAYSIZE(charset));

	iconv_t conv = iconv_open("WCHAR_T", charset);
	if(conv == (iconv_t)-1)
		return NULL;

	size_t inbytes_ = strlen(instr) + 1;
	size_t outbytes = inbytes_ * sizeof(wchar_t);
	wchar_t* outbuf = new wchar_t[outbytes / sizeof(wchar_t)];
	char* outptr = (char*)outbuf;
	char* inptr = (char*)instr;

	if(iconv(conv, &inptr, &inbytes_, &outptr, &outbytes) == (size_t)-1) {
		iconv_close(conv);
		delete[] outbuf;
		return NULL;
	}

	iconv_close(conv);

	assert(((wchar_t*)outptr)[-1] == L'\0');

	*p_outwchars_ = (wchar_t*)outptr - outbuf - 1;
	return outbuf;
}

