#ifndef __win32cozy_h_20171114_
#define __win32cozy_h_20171114_

#include <tchar.h>
#include <stdio.h>
#include <windows.h>

///////////// Win32Mutex ///////////////

struct Win32AutoMutex
{
	HANDLE m_hmutex;
	Win32AutoMutex(HANDLE hmutex) : m_hmutex(hmutex) 
	{  
		WaitForSingleObject(m_hmutex, INFINITE);
	}
	~Win32AutoMutex()
	{ 
		ReleaseMutex(m_hmutex); 
	}
};

/* Usage:

void CMyClass::FlushBuffer()
{
	Win32AutoMutex am(m_hmutex); // m_hmutex is a member of CMyClass

	// Do sth.
{

*/

// [2025-05-17] 

inline TCHAR* RECTtext(const RECT &r, TCHAR textbuf[], int buflen)
{
	// buflen suggestion: 60
	_sntprintf_s(textbuf, buflen, _TRUNCATE, _T("LT(%d,%d)RB(%d,%d)[%d*%d]"),
		r.left, r.top, r.right, r.bottom, (r.right-r.left), (r.bottom-r.top));
	return textbuf;
}



#endif
