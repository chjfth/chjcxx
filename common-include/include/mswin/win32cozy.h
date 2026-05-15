#ifndef __win32cozy_h_
#define __win32cozy_h_
#define __win32cozy_h_created_ 20171114
#define __win32cozy_h_updated_ 20260515

#include <tchar.h>
#include <stdio.h>
#include <assert.h>
#include <windows.h>

#define Ptr2Uint(ptr) ((unsigned int)(uintptr_t)(ptr))
// -- to avoid 64bit compiler warning like; 
//    warning C4311: 'type cast': pointer truncation from 'HMENU' to 'unsigned int'


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
