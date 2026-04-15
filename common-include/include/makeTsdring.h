#ifndef __CHHI__makeTsdring_h_
#define __CHHI__makeTsdring_h_
#define __CHHI__makeTsdring_h_created_ 20251217
#define __CHHI__makeTsdring_h_updated_ 20260406


#include <ps_TCHAR.h>
#include <sdring.h>


// makeTstring() is used in situation that the input string from external source 
// is known to be tied to `char` or tied to `WCHAR`, but you still want to have
// a TCHAR string(Sdring) for internal process.
//
// For example, D3DXAssembleShader() returns error string definitely as narrow char,
// but you want to send that string to TCHAR-style WinAPI, you can construct a 
// sdring<TCHAR> object as soon as you get that narrow string.

char* mTs_char_from_wchar(int codepage, const wchar_t* instr, int *p_outchars_=nullptr); // OS specific

wchar_t* mTs_wchar_from_char(int codepage, const char* instr, int *p_outwchars_=nullptr); // OS specific

enum { mTs_UTF8 = 65001 };


// sdring<TCHAR> from char*
sdring<TCHAR> makeTsdring(const char *instr, int codepage=0);
sdring<TCHAR> makeTsdring(sdring<char>&& instr, int codepage=0);

// sdring<TCHAR> from wchar_t*
sdring<TCHAR> makeTsdring(const wchar_t *instr, int codepage=0);
sdring<TCHAR> makeTsdring(sdring<wchar_t>&& instr, int codepage=0);

// sdring<char> from TCHAR*
sdring<char> makeAsdring(const TCHAR *instr, int codepage=0);
sdring<char> makeAsdring(sdring<TCHAR>&& instr, int codepage=0);

// sdring<wchar_t> from TCHAR*
sdring<wchar_t> makeWsdring(const TCHAR *instr, int codepage=0);


#define makeTsdring_UTF8(s) makeTsdring(s, mTs_UTF8)
#define makeAsdring_UTF8(s) makeAsdring(s, mTs_UTF8)
#define makeWsdring_UTF8(s) makeWsdring(s, mTs_UTF8)


/*
////////////////////////////////////////////////////////////////////////////
 ___                 _                           _        _   _             
|_ _|_ __ ___  _ __ | | ___ _ __ ___   ___ _ __ | |_ __ _| |_(_) ___  _ __  
 | || '_ ` _ \| '_ \| |/ _ \ '_ ` _ \ / _ \ '_ \| __/ _` | __| |/ _ \| '_ \ 
 | || | | | | | |_) | |  __/ | | | | |  __/ | | | || (_| | |_| | (_) | | | |
|___|_| |_| |_| .__/|_|\___|_| |_| |_|\___|_| |_|\__\__,_|\__|_|\___/|_| |_|
              |_|                                                           
////////////////////////////////////////////////////////////////////////////
*/
// ++++++++++++++++++ Implementation Below ++++++++++++++++++
//
#if defined(makeTsdring_IMPL) || (defined CHHI_ALL_IMPL && !defined CHHI_ALL_IMPL_HIDE_makeTsdring) // [IMPL]

#ifdef _MSC_VER 
// Using Microsoft Visual C++ as compiler
#include "mswin\makeTsdring.CHHI.h"
#else 
// Consider it Linux/GCC
#include "linux/makeTsdring.CHHI.h"
#endif

#include <utility> // for std::move()

//
// sdring<TCHAR> from char* (Note: the pair of makeTsdring() code seems to be C++ code consistent, can be templatized?)
//
sdring<TCHAR> makeTsdring(const char *instr, int codepage)
{
#if defined(UNICODE) || defined(_UNICODE)
	int outs_ = 0;
	wchar_t *wcbuf = mTs_wchar_from_char(codepage, instr, &outs_);
	// instr = nullptr;

	sdring<TCHAR> rets;
	rets.takeover(wcbuf, outs_-1);
	return rets;
#else
	return sdring<char>(instr);
#endif
}

sdring<TCHAR> makeTsdring(sdring<char>&& instr, int codepage)
{
#if defined(UNICODE) || defined(_UNICODE)
	int outs_ = 0;
	wchar_t *wcbuf = mTs_wchar_from_char(codepage, instr, &outs_);
	instr = nullptr; // release old sdring

	sdring<TCHAR> rets;
	rets.takeover(wcbuf, outs_-1);
	return rets;
#else
	return sdring<char>(std::move(instr));
#endif
}


//
// sdring<TCHAR> from wchar_t*
//
sdring<TCHAR> makeTsdring(const wchar_t *instr, int codepage)
{
#if defined(UNICODE) || defined(_UNICODE)
	return sdring<wchar_t>(instr);
#else
	int outs_ = 0;
	char *charbuf = mTs_char_from_wchar(codepage, instr, &outs_);
	// instr = nulltpr;

	sdring<TCHAR> rets;
	rets.takeover(charbuf, outs_-1);
	return rets;
#endif
}

sdring<TCHAR> makeTsdring(sdring<wchar_t>&& instr, int codepage)
{
#if defined(UNICODE) || defined(_UNICODE)
	return sdring<wchar_t>(std::move(instr));
#else
	int outs_ = 0;
	char *charbuf = mTs_char_from_wchar(codepage, instr, &outs_);
	instr = nullptr;

	sdring<TCHAR> rets;
	rets.takeover(charbuf, outs_-1);
	return rets;
#endif
}


//
// sdring<char> from TCHAR*
//
sdring<char> makeAsdring(const TCHAR *instr, int codepage)
{
#if defined(UNICODE) || defined(_UNICODE)
	int outs_ = 0;
	char *charbuf = mTs_char_from_wchar(codepage, instr, &outs_);
	// instr = nullptr;

	sdring<char> rets;
	rets.takeover(charbuf, outs_-1);
	return rets;
#else
	return sdring<char>(instr);
#endif
}

sdring<char> makeAsdring(sdring<TCHAR>&& instr, int codepage)
{
#if defined(UNICODE) || defined(_UNICODE)
	int outs_ = 0;
	char *charbuf = mTs_char_from_wchar(codepage, instr, &outs_);
	instr = nullptr; // release old sdring

	sdring<char> rets;
	rets.takeover(charbuf, outs_-1);
	return rets;
#else
	return sdring<char>(std::move(instr));
#endif

}


//
// sdring<wchar_t> from TCHAR*
//
sdring<wchar_t> makeWsdring(const TCHAR *instr, int codepage)
{
#if defined(UNICODE) || defined(_UNICODE)
	return sdring<wchar_t>(instr);
#else
	wchar_t *wcbuf = mTs_wchar_from_char(codepage, instr);

	sdring<wchar_t> rets = wcbuf;

	delete wcbuf;
	return rets;
#endif
}



#endif // [IMPL]

#endif // include once guard
