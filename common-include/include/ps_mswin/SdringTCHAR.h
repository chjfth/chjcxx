#ifndef __SdringTCHAR_h_20250513_
#define __SdringTCHAR_h_20250513_

// This is for Windows.

/* [2025-11-14] This file may be redundant. When you need String, why not write:

	#include <ps_TCHAR.h>
	
	typedef sdring<TCHAR> Sdring

*/

#include <sdring.h>

#include <tchar.h>

#ifdef UNICODE
typedef sdring<wchar_t> Sdring;
#else
typedef sdring<char> Sdring;
#endif


#endif
