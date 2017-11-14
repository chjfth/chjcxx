#ifndef __win32cozy_h_20171114_
#define __win32cozy_h_20171114_

// User should include <windows.h> himself.

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


#endif
