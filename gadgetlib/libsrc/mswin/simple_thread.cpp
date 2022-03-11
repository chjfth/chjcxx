#include "stdafx.h"

#ifndef WINCE
#include <process.h> // For Desktop Windows, _beginthreadex
#endif

#include <gadgetlib/simple_thread.h>

#define DLL_AUTO_EXPORT_STUB
extern"C" void gadgetlib_lib__simple_thread__DLL_AUTO_EXPORT_STUB(void){}


struct SwThreadParam
{
	PROC_ggt_simple_thread proc;
	void *param;
};

#ifdef _WIN32_WCE
static DWORD				// WinCE
#else 
static unsigned __stdcall	// PC Windows
#endif 
_WinThreadWrapper(void * param)
{
	SwThreadParam *pw = (SwThreadParam*)param;
	
	pw->proc(pw->param);
	delete pw;
	
	return 0;
}


GGT_HSimpleThread
ggt_simple_thread_create(PROC_ggt_simple_thread proc, void *param, int stack_size)
{
	HANDLE hThread = NULL;
	SwThreadParam *pwp = new SwThreadParam;
	if(!pwp)
		return NULL;
	
	pwp->proc = proc, pwp->param = param;
	
#ifdef _WIN32_WCE
	DWORD tid;
	hThread = CreateThread(NULL, stack_size, _WinThreadWrapper, pwp, 0, &tid);
#else
	// PC Windows
	unsigned tid;
	hThread = (HANDLE)_beginthreadex(NULL, stack_size, _WinThreadWrapper, pwp ,0, &tid);
#endif
	
	return (GGT_HSimpleThread)hThread;
}

bool 
ggt_simple_thread_waitend(GGT_HSimpleThread h)
{
	if(!h) {
		return false;
	}

	DWORD waitre = WaitForSingleObject(h, INFINITE);
	if(waitre==WAIT_OBJECT_0)
	{
		CloseHandle(h);
		return true;
	}
	else
		return false;
}

