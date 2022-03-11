// If using StdAfx.h, you should not pre-compile this file.

#define DLL_AUTO_EXPORT_STUB
extern"C" void gadgetlib_lib__my_winapi__DLL_AUTO_EXPORT_STUB(void){}

#define DL_WINAPI_ImplementWrapperInThisFile
#include "my_winapi.h"


#define AUTOBUF_IMPL
#include <AutoBuf.h>

#define JULAYOUT_IMPL
#include <mswin/JULayout.h>


