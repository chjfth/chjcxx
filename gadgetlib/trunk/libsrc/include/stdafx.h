// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#if defined(_MSC_VER)     // Visual C++ compiler:

// #include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#define _CRT_SECURE_NO_WARNINGS // to disable warning on VC++ (have to put it before sys headers)

// Windows Header Files:
#include <windows.h>
#include <windowsx.h>

// C RunTime Header Files
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <assert.h>
#include <malloc.h>
#include <time.h>
#include <memory.h>
#include <tchar.h>
#include <wchar.h>
#include <string.h>
#include <ctype.h>
#include <locale.h>

#ifndef WINCE
#include <my_winapi.h> // This is not for WinCE
#endif

#elif defined(__GNUC__)     // Linux GCC compiler

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <assert.h>
#include <malloc.h>
#include <time.h>
#include <memory.h>
#include <wchar.h>
#include <string.h>
#include <ctype.h>

#include <unistd.h>


#endif // Complier checking done



//////////////////////////////////////////////////////////////////////////

// Platform unrelated headers below:

#include <ps_TCHAR.h>
#include <commdefs.h>

#include <EnsureClnup_common.h>

#include <mm_snprintf.h>

#include <iversion.h>
#include <in_char_op.h>


