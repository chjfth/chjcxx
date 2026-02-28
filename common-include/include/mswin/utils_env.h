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

TCHAR *parse_cmdparam_HEXRR(
	const TCHAR *T_cmdline, TCHAR outbuf[], int outbuflen, 
	int *p_retlen=nullptr, // optional
	bool *p_prefer_hexinput=nullptr // optional
	); // // return just outbuf.
	// note: was named parse_cmdparam_TCHARs() from G:\gitw\bookcode-mswin\share\vaDbg.cpp

inline int com_GetRefCount(IUnknown *piu)
{
	if(!piu)
		return 0;

	piu->AddRef();
	return piu->Release();
}


/*
////////////////////////////////////////////////////////////////////////////
 ___                 _                           _        _   _             
|_ _|_ __ ___  _ __ | | ___ _ __ ___   ___ _ __ | |_ __ _| |_(_) ___  _ __  
 | || '_ ` _ \| '_ \| |/ _ \ '_ ` _ \ / _ \ '_ \| __/ _` | __| |/ _ \| '_ \ 
 | || | | | | | |_) | |  __/ | | | | |  __/ | | | || (_| | |_| | (_) | | | |
|___|_| |_| |_| .__/|_|\___|_| |_| |_|\___|_| |_|\__\__,_|\__|_|\___/|_| |_|
              |_|                                                           
////////////////////////////////////////////////////////////////////////////
*/
// ++++++++++++++++++ Implementation Below ++++++++++++++++++
//
#if defined(utils_env_IMPL) || (defined CHHI_ALL_IMPL && !defined CHHI_ALL_IMPL_HIDE_utils_env) // [IMPL] begin
// [IMPL] //
// [IMPL] //
// Include system/OS headers here
#include <Shlwapi.h> // for StrRChr
#pragma comment(lib, "Shlwapi.lib")
// [IMPL] //
// [IMPL] //


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


#include <ShellAPI.h> // CommandLineToArgvW

TCHAR *parse_cmdparam_HEXRR(
	const TCHAR *T_cmdline, TCHAR outbuf[], int outbuflen, 
	int *p_retlen, bool *p_prefer_hexinput)
{
/*  T_cmdline should points to string from GetCommandLineA()/GetCommandLineW().

	If TCHAR is char (ANSI version), outbuf[] will be a char array.
	If TCHAR is WCHAR (Unicode version), outbuf[] will be a WCHAR array.

	Actual output TCHAR count will be returned in *p_retlen.

	If user passes more than one parameter, like this:

		EXENAME 41 42 7535 43 44

	Then each parameter will be considered a TCHAR in HEXRR representation, 
	and user gets 5 TCHARs on return.

	So in ANSI version, user will get sth. equivalent to:

		const char *outbuf = "\x41\x42\x35\x43\x44";

	In Unicode version, user will get sth. equivalent to:
		
		const WCHAR *outbuf = "\x0041\x0042\x7535\x0043\x0044";

	If only one parameter is given, like this:

		EXENAME "AB cde"

	The string of "AB cde" (6 TCHARs) will be returned.

	p_prefer_hexinput: [in/out]
		* As input, this matters only when T_cmdline has an single param, e.g.:
			
			EXENAME AB
		  
		  - If *p_prefer_hexinput==false, "AB" is considered a literal string,
		    so outbuf[] will contain two TCHARs "AB".
		  - If *p_prefer_hexinput==true, "AB" is considered a single TCHAR represented
		    in HEXRR of value 0xAB, so the outbuf[] will contain a single TCHAR.

		* As output, tells whether T_cmdline is considered literal or hex by the API.

		I think in most user test-program scenario, user don't need to care this
		p_prefer_hexinput and just pass nullptr for it.

*/
	//const TCHAR *T_cmdline = GetCommandLine();
	const WCHAR *W_cmdline = nullptr;

	if(outbuflen<=0)
		return nullptr;

	if(p_retlen)
		*p_retlen = 0;

	bool prefer_hexinput = false;
	if(p_prefer_hexinput)
	{
		prefer_hexinput = *p_prefer_hexinput;
		*p_prefer_hexinput = false;
	}

#ifdef UNICODE
	W_cmdline = T_cmdline;
#else
	WCHAR W_buf[1024] = {};
	W_cmdline = W_buf;
	MultiByteToWideChar(CP_ACP, 0, T_cmdline, -1, W_buf, ARRAYSIZE(W_buf));		
#endif

	int argc = 0;
	WCHAR **argv = CommandLineToArgvW(W_cmdline, &argc);
	// -- argv[0] is exepath itself.

	if(argc==2 && !prefer_hexinput)
	{
		// not hexform input:
#ifdef UNICODE
		_tcscpy_s(outbuf, outbuflen, argv[1]);
		int retlen = (int)wcslen(argv[1]);
#else
		int retlen = WideCharToMultiByte(CP_ACP, 0, argv[1], -1, outbuf, outbuflen, NULL, NULL);
#endif
		if(p_retlen)
			*p_retlen = retlen;

		LocalFree(argv);
		return outbuf;
	}

	// hexform input: (including argc==1 case)

	int cycles = min(outbuflen-1, argc-1);

	int i;
	for(i=1; i<=cycles; i++)
	{
		outbuf[i-1] = (TCHAR)wcstoul(argv[i], nullptr, 16);
	}

	// To make it caller friendly, we always append a NUL char, but don't count it.
	outbuf[cycles] = '\0';

	if(p_retlen)
		*p_retlen = cycles;

	if(p_prefer_hexinput)
		*p_prefer_hexinput = true;

	LocalFree(argv);
	return outbuf;
}



#endif // utils_env_IMPL


#endif
