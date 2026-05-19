/*
[2026-05-15] Due to a bug in ShlObj.h from WinXP SDK, when compiling with VC2015, there is always warning like this:

C:\Program Files (x86)\Microsoft SDKs\Windows\v7.1A\include\ShlObj.h(1151): warning C4091: 'typedef ': ignored on left of 'tagGPFIDL_FLAGS' when no variable is declared

which is due to such C statement in ShlObj.h :

typedef enum tagGPFIDL_FLAGS
{
    GPFIDL_DEFAULT    = 0x0000,      // normal Win32 file name, servers and drive roots included
    GPFIDL_ALTNAME    = 0x0001,      // short file name
    GPFIDL_UNCPRINTER = 0x0002,      // include UNC printer names too (non file system item)
};

To avoid such annoyance, user can `#include <ShlOjb-winxp-patch.h>` instead.

*/

//#include <windows.h> // need this before <ShlOjb.h> ?

#pragma warning( push )

#pragma warning(disable:4091)

#include <ShlObj.h>

#pragma warning( pop )
