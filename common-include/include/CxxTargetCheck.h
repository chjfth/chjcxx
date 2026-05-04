#ifndef __CHHI__CxxTargetCheck_h_
#define __CHHI__CxxTargetCheck_h_
#define __CHHI__CxxTargetCheck_h_created_ 20260504
#define __CHHI__CxxTargetCheck_h_updated_ 20260504


#if defined(_WIN32) && !defined(__unix__)

// This compiler generates code for running on Microsoft Windows
// (using WinAPI) to interact with the system.
// Visual C++ and MinGW-gcc belong to this.

#define CXX_TARGET_WINDOWS

// Special note: Compile using Cygwin, we see both _WIN32 and __unix__,
// and Cygwin is not considered targeting Windows(cannot use WinAPI).

#endif



#if defined(__unix__)

// This compiler generates code for Unix-like env. Linux included.

#define CXX_TARGET_UNIX_ENV

#endif




#endif // include once guard
