#ifndef __SdringTCHAR_h_20250513_
#define __SdringTCHAR_h_20250513_

// This is for Windows.

#include <sdring.h>

#include <tchar.h>

#ifdef UNICODE
typedef sdring<wchar_t> Sdring;
#else
typedef sdring<char> Sdring;
#endif


#endif
