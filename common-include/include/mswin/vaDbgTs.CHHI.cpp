// This is included by parent directory vaDbgTs.h .
// Do not compile this file alone.


#include <windows.h>
#include <shellapi.h>
#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>

//////////////////////////////// #include done ////////////////////////////////

#ifndef WinMultiMon_DEBUG
#include <CHHI_vaDBG_hide.h>
#endif

namespace ns_vaDbgTs { // to protect our private symbols

const int DBG_BUFCHARS = 16000;

static int s_dbgcount = 0;

bool s_needymd = true;

inline unsigned __int64 get_qpf()
{
	LARGE_INTEGER li = {};
	BOOL succ = QueryPerformanceFrequency(&li);
	if(!succ)
		return -1;
	else
		return li.QuadPart;
}

inline unsigned __int64 get_qpc()
{
	LARGE_INTEGER li = {};
	BOOL succ = QueryPerformanceCounter(&li);
	if(!succ)
		return -1;
	else
		return li.QuadPart;
}

} // namespace ns_vaDbgTs

using namespace ns_vaDbgTs;

bool vaDbgTs_NeedYmd(bool ymd)
{
	int oldval = s_needymd;
	s_needymd = ymd;
	return oldval;
}

unsigned int va_millisec()
{
	static __int64 s_qpf = get_qpf();

	// We should use QueryPerformanceCounter(), 
	// bcz GetTickCount() only has 15.6 ms resolution.
	DWORD millisec = DWORD(get_qpc()*1000 / s_qpf);
	return millisec;
}

TCHAR* now_timestr(TCHAR buf[], int bufchars)
{
	SYSTEMTIME st = { 0 };
	GetLocalTime(&st);
	buf[0] = '['; buf[1] = '\0';

	if (ns_vaDbgTs::s_needymd) 
	{
		_sntprintf_s(buf, bufchars, _TRUNCATE, _T("%s%04d-%02d-%02d_"), buf,
			st.wYear, st.wMonth, st.wDay);
	}

	_sntprintf_s(buf, bufchars, _TRUNCATE, _T("%s%02d:%02d:%02d]"), buf,
		st.wHour, st.wMinute, st.wSecond);

	return buf;
}



void vaDbgTs(const TCHAR *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	vlDbgTs(fmt, args);
	va_end(args);
}

void vlDbgTs(const TCHAR *fmt, va_list args)
{
	// Note: Each calling outputs one line, with timestamp prefix.
	// A '\n' will be added automatically at end.

	static int count = 0;
	static DWORD s_prev_msec = va_millisec();

	DWORD now_msec = va_millisec();

	TCHAR buf[DBG_BUFCHARS] = {0};

	// Print timestamp to show that time has elapsed for more than one second.
	DWORD delta_msec = now_msec - s_prev_msec;
	if(delta_msec>=1000)
	{
		OutputDebugString(_T(".\n"));
	}

	TCHAR timebuf[40] = {};
	now_timestr(timebuf, ARRAYSIZE(timebuf));

	_sntprintf_s(buf, _TRUNCATE, _T("[%d]%s (+%3u.%03us) "), 
		++s_dbgcount,
		timebuf, 
		delta_msec/1000, delta_msec%1000);

	int prefixlen = (int)_tcslen(buf);

	_vsntprintf_s(buf+prefixlen, ARRAYSIZE(buf)-3-prefixlen, _TRUNCATE, fmt, args);

	// add trailing \n
	int slen = (int)_tcslen(buf);
	if(slen==ARRAYSIZE(buf)-1)
		--slen;

	buf[slen] = '\n';
	buf[slen+1] = '\0';

	OutputDebugString(buf);

	s_prev_msec = now_msec;
}

void vaDbgS(const TCHAR *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	vlDbgS(fmt, args);
	va_end(args);
}

void vlDbgS(const TCHAR *fmt, va_list args)
{
	// This only has Sequential prefix.

	TCHAR buf[DBG_BUFCHARS] = {0};

	_sntprintf_s(buf, ARRAYSIZE(buf)-3, _TRUNCATE, TEXT("[%d] "), ++s_dbgcount); // prefix seq

	int prefixlen = (int)_tcslen(buf);

	_vsntprintf_s(buf+prefixlen, ARRAYSIZE(buf)-3-prefixlen, _TRUNCATE, fmt, args);

	// add trailing \n
	int slen = (int)_tcslen(buf);
	if(slen==ARRAYSIZE(buf)-1)
		--slen;

	buf[slen] = '\n';
	buf[slen+1] = '\0';

	OutputDebugString(buf);
}



#ifndef WinMultiMon_DEBUG
#include <CHHI_vaDBG_show.h>
#endif
