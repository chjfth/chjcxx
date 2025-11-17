#pragma once

#include <tchar.h>
#include <windows.h>

#include <sdring.h>

// makeTstring() is used in situation that the input string from external source 
// is known to be tied to `char` or tied to `WCHAR`, but you still want to have
// a TCHAR string(sdring) for internal process.
//
// For example, D3DXAssembleShader() returns error string definitely as narrow char,
// but you want to send that string to TCHAR-style WinAPI, you can construct a 
// sdring<TCHAR> object as soon as you get that narrow error string.

inline char* char_from_wchar(int codepage, const wchar_t* instr)
{
	int bytesreq = WideCharToMultiByte(codepage, 0, instr, -1, NULL, 0, NULL, NULL);
	char *charbuf = new char[bytesreq];
	WideCharToMultiByte(codepage, 0, instr, -1, charbuf, bytesreq, NULL, NULL);
	return charbuf;
}

inline wchar_t* wchar_from_char(int codepage, const char* instr)
{
	int wcreq = MultiByteToWideChar(codepage, 0, instr, -1, NULL, 0);
	wchar_t *wcbuf = new wchar_t[wcreq];
	MultiByteToWideChar(codepage, 0, instr, -1, wcbuf, wcreq);
	return wcbuf;
}


sdring<TCHAR> makeTsdring(const char *instr)
{
#if defined(UNICODE) || defined(_UNICODE)
	wchar_t *wcbuf = wchar_from_char(CP_ACP, instr);
	
	sdring<TCHAR> rets = wcbuf;

	delete wcbuf;
	return rets;
#else
	return sdring<char>(instr);
#endif
}

// sdring<wchar_t> from wchar_t*
sdring<TCHAR> makeTsdring(const wchar_t *instr)
{
#if defined(UNICODE) || defined(_UNICODE)
	return sdring<wchar_t>(instr);
#else
	char *charbuf = char_from_wchar(CP_ACP, instr);

	sdring<TCHAR> rets = charbuf;

	delete charbuf;
	return rets;
#endif
}

