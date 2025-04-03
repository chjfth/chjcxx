#ifndef __VC2010_future_h_20250403_
#define __VC2010_future_h_20250403_

// This file supplies macros, enum, functions introduced after VC2010/Win7 SDK,
// so that VC2010 vcxproj can use future symbols.
//
// Be aware, please protect future symbols with proper _WIN32_WINNT version check,
// so that this file is compatible with real future Visual C++/SDK releases.
//
// Sure, this file should be included AFTER all system headers for best compatibility.


//////////////////////////////////////////////////////////////////////////////
// winnt.h
//////////////////////////////////////////////////////////////////////////////

#if _WIN32_WINNT < 0x0A00
// enum SID_NAME_USE has more values.
enum { SidTypeLogonSession = 11 };
#endif




#endif

