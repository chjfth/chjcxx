#ifndef __utils_env_h_20250830_
#define __utils_env_h_20250830_

#include <windows.h>
#include <windowsx.h>
#include <tchar.h>


unsigned __int64 get_qpf();
unsigned __int64 get_qpc();

DWORD TrueGetMillisec();

inline const TCHAR *str_ANSIorUnicode()
{
	return sizeof(TCHAR)==1 ? _T("ANSI") : _T("Unicode");
}

BOOL Is_UserAnAdmin();

const TCHAR* GetExeFilename();

const TCHAR* env_GetCpuArch();



///////////////////////////////////////////////////////////////
// Implementation Below:
///////////////////////////////////////////////////////////////

#ifdef utils_env_IMPL


unsigned __int64 get_qpf()
{
	LARGE_INTEGER li = {};
	BOOL succ = QueryPerformanceFrequency(&li);
	if(!succ)
		return -1;
	else
		return li.QuadPart;
}

unsigned __int64 get_qpc()
{
	LARGE_INTEGER li = {};
	BOOL succ = QueryPerformanceCounter(&li);
	if(!succ)
		return -1;
	else
		return li.QuadPart;
}

DWORD TrueGetMillisec()
{
	static __int64 s_qpf = get_qpf();

	// We should use QueryPerformanceCounter(), 
	// bcz GetTickCount only has 15.6 ms resolution.
	DWORD millisec = DWORD(get_qpc()*1000 / s_qpf);
	return millisec;
}


//#pragma comment(lib, "Shell32.lib")

#include <ShlObj.h>

BOOL Is_UserAnAdmin()
{
	// Define this wrapper instead of using IsUserAnAdmin(), bcz on VC2015 blames warning on a buggy line from ShlObj.h:
	/*
1>C:\Program Files (x86)\Microsoft SDKs\Windows\v7.1A\include\ShlObj.h(1151): warning C4091: 'typedef ': ignored on left of 'tagGPFIDL_FLAGS' when no variable is declared
1>         utils.cpp

The buggy line is(L#1146):

typedef enum tagGPFIDL_FLAGS
{
	GPFIDL_DEFAULT    = 0x0000,      // normal Win32 file name, servers and drive roots included
	GPFIDL_ALTNAME    = 0x0001,      // short file name
	GPFIDL_UNCPRINTER = 0x0002,      // include UNC printer names too (non file system item)
};

WinSDK 8.1 has fixed it.
	*/
	
	return IsUserAnAdmin();
}

#include <Shlwapi.h> // for StrRChr
#pragma comment(lib, "Shlwapi.lib")

const TCHAR* GetExeFilename()
{
	static TCHAR exepath[MAX_PATH] = _T("Unknown exepath");
	GetModuleFileName(NULL, exepath, ARRAYSIZE(exepath));

	const TCHAR *pfilename = StrRChr(exepath, NULL, _T('\\'));
	if(pfilename && pfilename[1])
		pfilename++;
	else
		pfilename = exepath;

	return pfilename;
}


const TCHAR* env_GetCpuArch()
{
#if defined(_M_IX86)
	return _T("x86");
#elif defined(_M_X64)
	return _T("x64");
#elif defined(_M_ARM)
	return _T("ARM32");
#elif defined(_M_ARM64)
	return _T("ARM64");
#else
	return _T("CpuArch_Unknown");
#endif
}


#endif // utils_env_IMPL


#endif
