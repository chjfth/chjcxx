// This is included by parent directory makeTsdring.h .
// Do not compile this file alone.

#include <tchar.h>
#include <assert.h>
#include <windows.h>

#include <commdefs.h>

char* mTs_char_from_wchar(int codepage, const wchar_t* instr, int *p_outchars_)
{
	SETTLE_OUTPUT_PTR(int, p_outchars_, 0)

	if(!instr)
		return NULL;

	int bytesreq = WideCharToMultiByte(codepage, 0, instr, -1, NULL, 0, NULL, NULL);
	char *charbuf = new char[bytesreq];
	*p_outchars_ = WideCharToMultiByte(codepage, 0, instr, -1, charbuf, bytesreq, NULL, NULL);
	
	assert(bytesreq==*p_outchars_);

	return charbuf;
}

wchar_t* mTs_wchar_from_char(int codepage, const char* instr, int *p_outwchars_)
{
	SETTLE_OUTPUT_PTR(int, p_outwchars_, 0)

	if(!instr)
		return NULL;

	int wcreq     = MultiByteToWideChar(codepage, 0, instr, -1, NULL, 0);
	wchar_t *wcbuf = new wchar_t[wcreq];
	*p_outwchars_ = MultiByteToWideChar(codepage, 0, instr, -1, wcbuf, wcreq);

	assert(wcreq==*p_outwchars_);

	return wcbuf;
}

