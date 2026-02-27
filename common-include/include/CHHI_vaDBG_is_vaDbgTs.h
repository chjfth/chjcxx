#ifndef __CHHI_vaDBG_is_vaDbgTs_h_20251208_
#define __CHHI_vaDBG_is_vaDbgTs_h_20251208_

// If your CHHI.cpp wants to see debug-messages from various CHHI library modules
// (for XXX module, XXX_DEBUG defined as a premise), you need to:
// Copy this file's content to start portion of your CHHI.cpp, then (tweaking)
// replace `vaDbgTs` with your own debug-message output functions.
//
// Tweaking note: vaDBG, vaDGB1 etc must NOT be macro, they must be functions
// to support CHHI_vaDBG_hide.h/CHHI_vaDBG_show.h's code pattern.
//
// If your CHHI.cpp wants no debug messages, you don't have to use this file.


#include <utility>       // to have std::forward
#include <CxxVerCheck.h> // to see CXX11_OR_NEWER

#include <vaDbgTs.h>

#define vaDbg_ALL vaDbg_VERBOSE


#ifdef CXX11_OR_NEWER

template<typename... Args>
void vaDBG(Args&&... args) // forwards all arguments
{
	vaDbgTsl(vaDBG_ALL, std::forward<Args>(args)...);
}

template<typename... Args>
void vaDBG0(Args&&... args) // forwards all arguments
{
	vaDbgTsl(vaDbg_ERROR, std::forward<Args>(args)...);
}

template<typename... Args>
void vaDBG1(Args&&... args) // forwards all arguments
{
	vaDbgTsl(vaDbg_WARN, std::forward<Args>(args)...);
}

template<typename... Args>
void vaDBG2(Args&&... args) // forwards all arguments
{
	vaDbgTsl(vaDbg_INFO, std::forward<Args>(args)...);
}

template<typename... Args>
void vaDBG3(Args&&... args) // forwards all arguments
{
	vaDbgTsl(vaDbg_VERBOSE, std::forward<Args>(args)...);
}


#else // pre-C++11


#include <stdarg.h>
#include <ps_TCHAR.h>

void vaDBG(const TCHAR *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	vlDbgTsl(vaDbg_ALL, fmt, args);
	va_end(args);
};

void vaDBG0(const TCHAR *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	vlDbgTsl(vaDbg_ERROR, fmt, args);
	va_end(args);
};

void vaDBG1(const TCHAR *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	vlDbgTsl(vaDbg_WARN, fmt, args);
	va_end(args);
};

void vaDBG2(const TCHAR *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	vlDbgTsl(vaDbg_INFO, fmt, args);
	va_end(args);
};

void vaDBG3(const TCHAR *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	vlDbgTsl(vaDbg_VERBOSE, fmt, args);
	va_end(args);
};

#endif // CXX11_OR_NEWER/else

#endif
