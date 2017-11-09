#ifndef __dl_winapi_h_20170820_
#define __dl_winapi_h_20170820_

/*
dl_winapi.h: dynamic-linking(loading) WinAPI functions when SDK header is lacking

See usage example in https://nlssvn/svnreps/personal/chj/ChjTests/trunk/dl_winapi

Usage Sugguestion: Prepare two files.

(1) my_winapi.h

This file contains a series of
 
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
	#include ... // any other headers required for your dl_xxx function prototype
	
	#include "dl_winapi.h"
 
	DL_WINAPI_MAKE_WRAPPER("foo.dll", ...)
	DL_WINAPI_MAKE_WRAPPER("bar.dll", ...)
	...
	
(2) my_winapi.cpp

The file content is fixed(same for every project):

	// If using StdAfx.h, you should not pre-compile this file.
	#define DL_WINAPI_ImplementWrapperInThisFile
	#include "my_winapi.h"

*/

#include <tchar.h>

#ifdef __cplusplus
extern"C" {
#endif


#define DL_WINAPI_ERROR_FIND_ENTRY 0xE00000ee

#define DL_winapi_MAKE_STRING(s) #s

#ifdef DL_WINAPI_ImplementWrapperInThisFile

// Implement dl_SomeWinApi() function in this compilation unit.

#define DL_WINAPI_MAKE_WRAPPER(dllname, rettype, apiname, _Tparams_, _params_, retval_onfail) \
rettype WINAPI dl_ ## apiname _Tparams_ \
{ \
	typedef rettype (WINAPI * PROC_ThisApi) _Tparams_; \
	static PROC_ThisApi s_func = NULL; \
	if(!s_func) { \
		HMODULE hDll = LoadLibrary(_T(dllname)); \
		if(!hDll) return retval_onfail; \
		s_func = (PROC_ThisApi)GetProcAddress(hDll, DL_winapi_MAKE_STRING(apiname)); \
		if(!s_func) return retval_onfail; \
	} \
	return (*s_func) _params_; \
}

#else 

// Only make a function declaration of dl_SomeWinApi()

#define DL_WINAPI_MAKE_WRAPPER(dllname, rettype, apiname, _Tparams_, _params_, retval_onfail) \
	rettype WINAPI dl_ ## apiname _Tparams_ ; \


#endif

/// END ///


#if 0 // Example:
/*
	DL_WINAPI_MAKE_WRAPPER("user32.dll", int, MessageBoxW,
		(HWND hWnd, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType),
		(hWnd, lpText, lpCaption, uType),
		-2)

	will result in this:
*/
int WINAPI dl_MessageBox (HWND hWnd, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType)
{
	typedef int (WINAPI * PROC_ThisApi)(HWND hWnd, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType);
	static PROC_ThisApi s_func = NULL;

	if(!s_func)
	{
		const int retval_onfail = -2;
		HMODULE hDll = LoadLibrary(_T("user32.dll"));
		if(!hDll) return retval_onfail;

		s_func = (PROC_ThisApi)GetProcAddress(hDll, "MessageBoxW");
		if(!s_func) return retval_onfail;
	}

	return (*s_func)(hWnd, lpText, lpCaption, uType);
}
#endif




#ifdef __cplusplus
} // extern"C" {
#endif


#endif
