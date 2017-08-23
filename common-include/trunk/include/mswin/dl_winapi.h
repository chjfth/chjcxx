#ifndef __dl_winapi_h_20170820_
#define __dl_winapi_h_20170820_

/*

See usage example in https://nlssvn/svnreps/personal/chj/ChjTests/trunk/dl_winapi

Usage Sugguestion: Prepare two files.

(1) my_winapi.h

This file contains a series of
 
	DL_MAKE_WINAPI_WRAPPER("foo.dll", ...)
	
	DL_MAKE_WINAPI_WRAPPER("bar.dll", ...)
	
(2) impl_winapi. cpp

The file content is fixed(same for every project):

	// If using StdAfx.h, you should not pre-compile this file.
	#define DL_MAKE_WINAPI_ImplementWrapperInThisFile
	#include "my_winapi.h"

*/

#include <tchar.h>

#ifdef __cplusplus
extern"C" {
#endif

#define WINAPI_FAIL_0    // as retval_onfail
#define WINAPI_FAIL_NULL // as retval_onfail

#ifdef UNICODE
#define WINAPI_SUFFIX_AorW(apiname) apiname "W"
#else
#define WINAPI_SUFFIX_AorW(apiname) apiname "A"
#endif

#ifdef DL_MAKE_WINAPI_ImplementWrapperInThisFile

// Implement dl_SomeWinApi() function in this compilation unit.

#define DL_MAKE_WINAPI_WRAPPER(dllname, rettype, apiname, _Tparams_, _params_, retval_onfail) \
rettype WINAPI dl_ ## apiname _Tparams_ \
{ \
	typedef rettype (WINAPI * PROC_ThisApi) _Tparams_; \
	static PROC_ThisApi s_func = NULL; \
	if(!s_func) { \
		HMODULE hDll = LoadLibrary(_T(dllname)); \
		if(!hDll) return retval_onfail; \
		s_func = (PROC_ThisApi)GetProcAddress(hDll, #apiname); \
		if(!s_func) return retval_onfail; \
	} \
	return (*s_func) _params_; \
}

#else 

// Only make a function declaration of dl_SomeWinApi()

#define DL_MAKE_WINAPI_WRAPPER(dllname, rettype, apiname, _Tparams_, _params_, retval_onfail) \
	rettype WINAPI dl_ ## apiname _Tparams_ ; \


#endif

/// END ///


#if 0
/*
	DL_MAKE_WINAPI_WRAPPER("user32.dll", int, MessageBoxW,
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
