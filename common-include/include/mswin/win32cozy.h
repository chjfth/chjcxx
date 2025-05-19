#ifndef __win32cozy_h_20171114_
#define __win32cozy_h_20171114_

#include <tchar.h>
#include <stdio.h>
#include <assert.h>
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


template<typename T_int, size_t eles>
void Enable_Uics(BOOL isEnable, HWND hParent, const T_int (&ar)[eles])
{
	for (int i = 0; i < eles; i++)
	{
		HWND hUic = GetDlgItem(hParent, ar[i]);
		assert(IsWindow(hUic));

		EnableWindow(hUic, isEnable);
	}
}



#endif
