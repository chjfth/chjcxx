#ifndef __vaDbgTs_util_h_20251117_
#define __vaDbgTs_util_h_20251117_

#include <vaDbgTs.h>

namespace vaDbgTs_util {

//////////////////////////////////////////////////////////////////////////

enum { vaDbgTs_util_nothing = 0 };

class DbgEnterExit
{
	// Print debug message on function Enter/Exit.
	// For Visual C++, write this line as function's first statement:
	//
	//	vaDbgTs_util::DbgEnterExit(_T(__FUNCTION__));
	//
	// and you get the Enter/Exit message.

public:
	DbgEnterExit(const TCHAR *msg)
	{
		m_msg = msg;
		vaDbgTs(_T("%s() >>>"), m_msg);
	}

	~DbgEnterExit()
	{
		vaDbgTs(_T("%s() <<<"), m_msg);
	}

private:
	const TCHAR *m_msg;
};


inline BOOL vaSetWindowText(HWND hwnd, const TCHAR *fmt, ...)
{
	va_list args;
	va_start(args, fmt);

	TCHAR buf[1000] = {0};
	_vsntprintf_s(buf, _TRUNCATE, fmt, args);

	BOOL succ = SetWindowText(hwnd, buf);

	va_end(args);
	return succ;
}


inline BOOL vaSetDlgItemText(HWND hdlg, int ctrlid, const TCHAR *fmt, ...)
{
	va_list args;
	va_start(args, fmt);

	TCHAR buf[1000] = {0};
	_vsntprintf_s(buf, _TRUNCATE, fmt, args);

	BOOL succ = SetDlgItemText(hdlg, ctrlid, buf);

	va_end(args);
	return succ;
}


inline int vaMsgBox(HWND hwnd, UINT utype, const TCHAR *szTitle, const TCHAR *szfmt, ...)
{
	va_list args;
	va_start(args, szfmt);

	TCHAR msgtext[4000] = {};
	_vsntprintf_s(msgtext, _TRUNCATE, szfmt, args);

	if(!hwnd)
		hwnd = GetActiveWindow();

	if(!szTitle)
		szTitle = _T("Info"); // maybe show current exe name?

	int ret = MessageBox(hwnd, msgtext, szTitle, utype);

	va_end(args);
	return ret;
}



//////////////////////////////////////////////////////////////////////////

} // namespace

#endif

